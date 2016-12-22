#include <ntddk.h>
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

extern NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
	IN PUNICODE_STRING ObjectName,
	IN ULONG Attributes,
	IN PACCESS_STATE PassedAccessState OPTIONAL,
	IN ACCESS_MASK DesiredAccess OPTIONAL,
	IN POBJECT_TYPE ObjectType,
	IN KPROCESSOR_MODE AccessMode,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *Object
);

extern POBJECT_TYPE *IoDriverObjectType;


// 驱动模块地址信息
typedef struct _DRIVER_INFO
{
	// 加载顺序
	ULONG index;
	// 大小
	ULONG	size;
	// 加载次数
	ULONG LoadCount;
    // 基地址
	ULONG_PTR Base;
    // 入口地址
    ULONG_PTR EntryPoint;
    // 驱动对象
	ULONG_PTR DriverObject;
	// 设备对象
	ULONG_PTR DeviceObject;
	// 服务名
	WCHAR ServerName[MAX_PATH];
	// 驱动名
	WCHAR DriverName[MAX_PATH];
    // 模块路径
    WCHAR ImagePath[MAX_PATH];
    // 模块数量
    ULONG Count;
} DRIVER_INFO, *PDRIVER_INFO;

// 保存驱动信息
PDRIVER_INFO pDriverInfo = NULL;


typedef struct  _IRP_DISPATCH_INFO
{
	ULONG index;
	ULONG_PTR CurrentAddr;
	ULONG_PTR OrgAddr;
	WCHAR	  szModuleName[MAX_PATH];
}IRP_DISPATCH_INFO, *PIRP_DISPATCH_INFO;

PIRP_DISPATCH_INFO pIrpDispatchInfo = NULL;

NTSTATUS GetDriverObject(IN WCHAR *DriverName, OUT PDRIVER_OBJECT *lpDriverObject);

typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY64 InLoadOrderLinks;
	ULONG64 __Undefined1;
	ULONG64 __Undefined2;
	ULONG64 __Undefined3;
	ULONG64 NonPagedDebugInfo;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG   Flags;
	USHORT  LoadCount;
	USHORT  __Undefined5;
	ULONG64 __Undefined6;
	ULONG   CheckSum;
	ULONG   __padding1;
	ULONG   TimeDateStamp;
	ULONG   __padding2;
}KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;


NTSTATUS GetDriverObject(IN WCHAR *DriverName, OUT PDRIVER_OBJECT *lpDriverObject)
{
	NTSTATUS		Status;
	UNICODE_STRING	usObjectName;
	UNICODE_STRING	usFileObject;
	PDRIVER_OBJECT	pDriverObject = NULL;
	WCHAR			szDriver[MAX_PATH] = L"\\Driver\\";
	WCHAR			szFileSystem[MAX_PATH] = L"\\FileSystem\\";

	if(lpDriverObject == NULL)
		return STATUS_INVALID_PARAMETER;

	wcscat(szDriver, DriverName);
	wcscat(szFileSystem, DriverName);
	RtlInitUnicodeString(&usObjectName, szDriver);
	RtlInitUnicodeString(&usFileObject, szFileSystem);

	// 有些是文件系统 "\\FileSystem\\Ntfs"  http://bbs.pediy.com/showthread.php?t=99970
	Status = ObReferenceObjectByName(
		&usObjectName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&pDriverObject
	);
	if (!NT_SUCCESS(Status))
	{
		Status = ObReferenceObjectByName(
			&usFileObject,
			OBJ_CASE_INSENSITIVE,
			NULL,
			0,
			*IoDriverObjectType,
			KernelMode,
			NULL,
			(PVOID*)&pDriverObject
			);

		if(!NT_SUCCESS(Status))
		{
			*lpDriverObject = NULL;
			//KdPrint(("failed! code :0x%x\n", Status));
			return Status;
		}
	}


	*lpDriverObject = pDriverObject;

	//KdPrint(("Driver Name:%wZ ServerKeyName:%wZ\n", &(pDriverObject->DriverName), &(pDriverObject->DriverExtension->ServiceKeyName)));

	ObDereferenceObject(pDriverObject);

	return STATUS_SUCCESS;
}

NTSTATUS EnumDriverMajorFunction(WCHAR *DriverName)
{
	NTSTATUS		Status;
	UNICODE_STRING	usObjectName;
	UNICODE_STRING	usFileObject;
	PDRIVER_OBJECT	pDriverObject;
	SYSTEM_MODULE	systemModule;
	WCHAR			szDriver[MAX_PATH] = L"\\Driver\\";
	WCHAR			szFileSystem[MAX_PATH] = L"\\FileSystem\\";
	WCHAR			szModuleName[MAX_PATH] = {0};
	size_t i = 0;
	pIrpDispatchInfo = (PIRP_DISPATCH_INFO)ExAllocatePool(NonPagedPool, sizeof(IRP_DISPATCH_INFO)*100);

	if(pIrpDispatchInfo == NULL)
		return STATUS_UNSUCCESSFUL;

	wcscat(szDriver, DriverName);
	wcscat(szFileSystem, DriverName);
	RtlInitUnicodeString(&usObjectName, szDriver);
	RtlInitUnicodeString(&usFileObject, szFileSystem);

	// 有些是文件系统 "\\FileSystem\\Ntfs"  http://bbs.pediy.com/showthread.php?t=99970
	Status = ObReferenceObjectByName(
		&usObjectName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&pDriverObject
	);
	if (!NT_SUCCESS(Status))
	{
		Status = ObReferenceObjectByName(
			&usFileObject,
			OBJ_CASE_INSENSITIVE,
			NULL,
			0,
			*IoDriverObjectType,
			KernelMode,
			NULL,
			(PVOID*)&pDriverObject
			);
		if(!NT_SUCCESS(Status))
		{
			KdPrint(("failed! code :0x%x\n", Status));
			ExFreePool(pIrpDispatchInfo);
			pIrpDispatchInfo = NULL;
			return Status;
		}
	}

	//KdPrint(("Driver Name:%wZ ServerKeyName:%wZ\n", &(pDriverObject->DriverName), &(pDriverObject->DriverExtension->ServiceKeyName)));

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; i++)
	{
		if (pDriverObject->MajorFunction[i] != NULL)
		{
			if (NT_SUCCESS(getSystemImageInfoByAddress((ULONG_PTR)pDriverObject->MajorFunction[i],&systemModule)) &&
					strlen(systemModule.ImageName) < MAX_PATH)
			{
					ANSI_STRING		ansiString;
					UNICODE_STRING	usString;

					RtlZeroMemory(szModuleName,sizeof(WCHAR)*MAX_PATH);

					RtlInitAnsiString(&ansiString,systemModule.ImageName);
					RtlAnsiStringToUnicodeString(&usString,&ansiString,TRUE);

					memcpy(szModuleName,usString.Buffer,usString.Length);

					/*
					// 只要驱动名
					memcpy(szModuleName,usString.Buffer,usString.Length);
					if (wcsrchr(szModuleName,L'\\') != NULL)
					{
						wcscpy(lptimerRecord[count].szModuleName,wcsrchr(szModuleName,L'\\')+1);
					}else{
						wcscpy(lptimerRecord[count].szModuleName,szModuleName);
					}
					*/
					wcscpy(pIrpDispatchInfo[i].szModuleName,szModuleName);

					pIrpDispatchInfo[i].index = i;
					pIrpDispatchInfo[i].CurrentAddr = (ULONG_PTR)pDriverObject->MajorFunction[i];
					//DbgPrint("%d -- 0x%llx -- %S\n", i, pDriverObject->MajorFunction[i],szModuleName);
					RtlFreeUnicodeString(&usString);
			}
		}
	}

	ObDereferenceObject(pDriverObject);

	return STATUS_SUCCESS;
}

BOOLEAN EnumDriver(PDRIVER_OBJECT pDriverObject)
{
	ULONG count = 0;
	//ULONG t = 0;
	PKLDR_DATA_TABLE_ENTRY entry = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PKLDR_DATA_TABLE_ENTRY firstentry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Blink;
	WCHAR *szDriver = (WCHAR*)ExAllocatePool(NonPagedPool, MAX_PATH * 2);
	PDRIVER_OBJECT pOutDriverObject = NULL;
	pDriverInfo = (PDRIVER_INFO)ExAllocatePool(NonPagedPool, sizeof(DRIVER_INFO)*500);

	if(pDriverInfo == NULL)
		return FALSE;

	RtlZeroMemory(pDriverInfo, sizeof(DRIVER_INFO)*500);

	if (szDriver == NULL)
		return FALSE;

	RtlZeroMemory(szDriver, MAX_PATH * 2);

	//DbgPrint("%p\t%p\t%wZ\t%wZ\t\n", firstentry->DllBase, firstentry->EntryPoint, firstentry->BaseDllName,firstentry->FullDllName);
	pDriverInfo[0].Base = firstentry->DllBase;
	pDriverInfo[0].EntryPoint = firstentry->EntryPoint;
	pDriverInfo[0].LoadCount = firstentry->LoadCount;
	pDriverInfo[0].size = firstentry->SizeOfImage;
	__try{
		RtlCopyMemory(pDriverInfo[0].DriverName, firstentry->BaseDllName.Buffer, firstentry->BaseDllName.Length);
		RtlCopyMemory(pDriverInfo[0].ImagePath, firstentry->FullDllName.Buffer, firstentry->FullDllName.Length);
	}
	__except(1){

	}

	RtlCopyMemory(szDriver, firstentry->BaseDllName.Buffer, firstentry->BaseDllName.Length - 8);
	GetDriverObject(szDriver, &pOutDriverObject);
	if(pOutDriverObject != NULL)
	{
		__try
		{
			RtlCopyMemory(pDriverInfo[0].ServerName, pOutDriverObject->DriverExtension->ServiceKeyName.Buffer, pOutDriverObject->DriverExtension->ServiceKeyName.Length);
		}
		__except(1)
		{
			DbgPrint("11111111\n");
		}

		pDriverInfo[0].DriverObject = (ULONG_PTR)pOutDriverObject;
		pDriverInfo[0].DeviceObject = (ULONG_PTR)pOutDriverObject->DeviceObject;
	}
	
	count++;

	pOutDriverObject = NULL;
	//EnumDriverMajorFunction(szDriver);

	entry = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	firstentry = entry;
	while ((PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != firstentry)
	{
		RtlZeroMemory(szDriver, MAX_PATH * 2);
		if (entry->EntryPoint>0xFFFFF80000000000)
		{
			//DbgPrint("%p\t%p\t%wZ\t%wZ\t\n", entry->DllBase, entry->EntryPoint, entry->BaseDllName,entry->FullDllName);
			pDriverInfo[count].Base = entry->DllBase;
			pDriverInfo[count].EntryPoint = entry->EntryPoint;
			pDriverInfo[count].LoadCount = entry->LoadCount;
			pDriverInfo[count].size = entry->SizeOfImage;
			__try{
				RtlCopyMemory(pDriverInfo[count].DriverName, entry->BaseDllName.Buffer, entry->BaseDllName.Length);
				RtlCopyMemory(pDriverInfo[count].ImagePath, entry->FullDllName.Buffer, entry->FullDllName.Length);
			}
			__except(1){

			}

			RtlCopyMemory(szDriver, entry->BaseDllName.Buffer, entry->BaseDllName.Length - 8);
			GetDriverObject(szDriver, &pOutDriverObject);
			if(pOutDriverObject != NULL)
			{
				__try
				{
					RtlCopyMemory(pDriverInfo[count].ServerName, pOutDriverObject->DriverExtension->ServiceKeyName.Buffer, pOutDriverObject->DriverExtension->ServiceKeyName.Length);
				}
				__except(1)
				{
					DbgPrint("222222\n");
				}

				pDriverInfo[count].DriverObject = (ULONG_PTR)pOutDriverObject;
				pDriverInfo[count].DeviceObject = (ULONG_PTR)pOutDriverObject->DeviceObject;
			}
			//EnumDriverMajorFunction(szDriver);
			pOutDriverObject = NULL;
			count++;
		}
		entry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}

	pDriverInfo[0].Count = count;

	if(count == 0)
	{
		ExFreePool(pDriverInfo);
		pDriverInfo = NULL;
		return FALSE;
	}

	/*for (t = 0; t < count; t++)
	{
		DbgPrint(
			"index:%d\r\n"
			"size:%d\r\n"
			"LoadCount:%d\r\n"
			"Base:0x%llx\r\n"
			"EntryPoint:0x%llx\r\n"
			"DriverObject:0x%llx\r\n"
			"DeviceObject:0x%llx\r\n"
			"ServerName:%S\r\n"
			"DriverName:%S\r\n"
			"ImagePath:%S\r\n",
			pDriverInfo[t].index,
			pDriverInfo[t].size,
			pDriverInfo[t].LoadCount,
			pDriverInfo[t].Base,
			pDriverInfo[t].EntryPoint,
			pDriverInfo[t].DriverObject,
			pDriverInfo[t].DeviceObject,
			pDriverInfo[t].ServerName,
			pDriverInfo[t].DriverName,
			pDriverInfo[t].ImagePath
			);
	}*/

	DbgPrint("共有模块:%d\n", pDriverInfo[0].Count);
	ExFreePool(szDriver);
	//ExFreePool(pDriverInfo);
	return TRUE;
}