#pragma once

ULONG FltFilterOperationsOffset = 0;

typedef struct _FLT_OPERATION_REGISTRATION
{
	UCHAR	MajorFunction;
	ULONG	Flags;
	PVOID	PreOperation;
	PVOID	PostOperation;
	PVOID	Reserved1;
} FLT_OPERATION_REGISTRATION, *PFLT_OPERATION_REGISTRATION;

typedef struct _FLT_FILTER
{
	UCHAR buffer[1024];
} FLT_FILTER, *PFLT_FILTER;

typedef struct _MINIFILTER_INFO
{
	ULONG	FltNum;	//过滤器的个数
	ULONG	IrpCount; // Irp的总数
	ULONG	Irp;
	ULONG64	Object;
	ULONG64	PreFunc;
	ULONG64	PostFunc;
	CHAR	PreImgPath[MAX_PATH];
	CHAR	PostImgPath[MAX_PATH];
}MINIFILTER_INFO, *PMINIFILTER_INFO;

extern	NTSTATUS
__fastcall
FltEnumerateFilters
(
	PFLT_FILTER *FilterList,
	ULONG FilterListSize,
	PULONG NumberFiltersReturned
);

extern	NTSTATUS
__fastcall
FltObjectDereference
(
	PVOID FltObject
);

PMINIFILTER_INFO EnumMiniFilter()
{
	long	ntStatus;
	ULONG	uNumber = 0;
	ULONG	IrpCount = 0;
	PVOID	pBuffer = NULL;
	ULONG	DrvCount = 0;
	PVOID	pCallBacks = NULL, pFilter = NULL;
	SYSTEM_MODULE	SysModuel = {0};
	PMINIFILTER_INFO pFltInfo = NULL;
	PFLT_OPERATION_REGISTRATION pNode = NULL;
	

	pFltInfo = ExAllocatePool(NonPagedPool, sizeof(MINIFILTER_INFO) * 1000);
	if (pFltInfo == NULL)
		return NULL;

	RtlZeroMemory(pFltInfo, sizeof(MINIFILTER_INFO) * 1000);

	do
	{
		if (pBuffer != NULL)
		{
			ExFreePool(pBuffer);
			pBuffer = NULL;
		}
		ntStatus = FltEnumerateFilters(NULL, 0, &uNumber);
		if (ntStatus != STATUS_BUFFER_TOO_SMALL)
			break;
		pBuffer = ExAllocatePoolWithTag(NonPagedPool, sizeof(PFLT_FILTER) * uNumber, 'mnft');
		if (pBuffer == NULL)
		{
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
		ntStatus = FltEnumerateFilters(pBuffer, uNumber, &uNumber);
	} while (ntStatus == STATUS_BUFFER_TOO_SMALL);
	if (!NT_SUCCESS(ntStatus))
	{
		if (pBuffer != NULL)
			ExFreePool(pBuffer);
		return 0;
	}
	DbgPrint("MiniFilter Count: %ld\n", uNumber);
	pFltInfo[0].FltNum = uNumber;
	DbgPrint("------\n");
	__try
	{
		while (DrvCount<uNumber)
		{
			pFilter = (PVOID)(*(PULONG64)((PUCHAR)pBuffer + DrvCount * 8));
			pCallBacks = (PVOID)((PUCHAR)pFilter + FltFilterOperationsOffset);
			pNode = (PFLT_OPERATION_REGISTRATION)(*(PULONG64)pCallBacks);
			__try
			{
				while (pNode->MajorFunction != 0x80)	//IRP_MJ_OPERATION_END
				{
					if (pNode->MajorFunction<28)	//MajorFunction id is 0~27
					{
						DbgPrint("Object=%p\tPreFunc=%p\tPostFunc=%p\tIRP=%d\n",
							pFilter,
							pNode->PreOperation,
							pNode->PostOperation,
							pNode->MajorFunction);


						pFltInfo[IrpCount].Irp = pNode->MajorFunction;
						pFltInfo[IrpCount].Object = pFilter;
						
						if (pNode->PreOperation != 0)
						{
							pFltInfo[IrpCount].PreFunc = pNode->PreOperation;

							if (NT_SUCCESS(getSystemImageInfoByAddress(pFltInfo[IrpCount].PreFunc, &SysModuel)) && strlen(SysModuel.ImageName) < MAX_PATH)
							{
								RtlCopyMemory(pFltInfo[IrpCount].PreImgPath, SysModuel.ImageName, MAX_PATH);
							}

						}

						RtlZeroMemory(&SysModuel, sizeof(SYSTEM_MODULE));

						if (pNode->PostOperation != 0)
						{
							pFltInfo[IrpCount].PostFunc = pNode->PostOperation;
							if (NT_SUCCESS(getSystemImageInfoByAddress(pFltInfo[IrpCount].PostFunc, &SysModuel)) && strlen(SysModuel.ImageName) < MAX_PATH)
							{
								RtlCopyMemory(pFltInfo[IrpCount].PostImgPath, SysModuel.ImageName, MAX_PATH);
							}
						}

						RtlZeroMemory(&SysModuel, sizeof(SYSTEM_MODULE));

						IrpCount++;
					}
					pNode++;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				FltObjectDereference(pFilter);
				DbgPrint("[EnumMiniFilter]EXCEPTION_EXECUTE_HANDLER: pNode->MajorFunction\n");
				ntStatus = GetExceptionCode();
				ExFreePool(pBuffer);
				ExFreePool(pFltInfo);
				return NULL;
			}
			DrvCount++;
			FltObjectDereference(pFilter);
			DbgPrint("------\n");
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		FltObjectDereference(pFilter);
		DbgPrint("[EnumMiniFilter]EXCEPTION_EXECUTE_HANDLER\n");
		ntStatus = GetExceptionCode();
		ExFreePool(pBuffer);
		ExFreePool(pFltInfo);
		return NULL;
	}
	if (pBuffer != NULL)
	{
		pFltInfo[0].IrpCount = IrpCount;
		pFltInfo[0].FltNum = uNumber;
		ExFreePool(pBuffer);
		ntStatus = STATUS_SUCCESS;
	}

	return pFltInfo;
}