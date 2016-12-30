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

extern PDRIVER_OBJECT g_DriverObject;

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

typedef struct _DEVICE_DRIVER_INFO
{
	ULONG Count;
	PDRIVER_OBJECT pDrvObj;// 有设备对象的驱动对象
	WCHAR szObjName[MAX_PATH * 2]; // 对象名 给ObReferenceObjectByNam调用时用到
}DEVICE_DRIVER_INFO, *PDEVICE_DRIVER_INFO;

typedef struct  _IRP_DISPATCH_INFO
{
	ULONG index;
	ULONG_PTR CurrentAddr;
	ULONG_PTR OrgAddr;
	WCHAR	  szModuleName[MAX_PATH];
}IRP_DISPATCH_INFO, *PIRP_DISPATCH_INFO;

PIRP_DISPATCH_INFO pIrpDispatchInfo = NULL;


typedef struct _FILTER_DRIVER_INFO
{
	ULONG	Count;
	ULONG	DeviceType;
	ULONG64	AttachedDevice;
	WCHAR	SysName[MAX_PATH];
	WCHAR	HostSysName[MAX_PATH];
	WCHAR	HostSysPath[MAX_PATH];
}FILTER_DRIVER_INFO, *PFILTER_DRIVER_INFO;

NTSTATUS GetDriverObject(IN WCHAR *DriverName, OUT PDRIVER_OBJECT *lpDriverObject);
BOOLEAN EnumDriver(PDRIVER_OBJECT pDriverObject, BOOLEAN bEnumDeviceDriver, PDEVICE_DRIVER_INFO *lpDeviceDriverBuff);

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


PFILTER_DRIVER_INFO EnumFilterDriver()
{
	/*
	1.枚举驱动 得到有设备对象的驱动
	2.检查设备对象中AttachDevice是否有值
	3.如果存在 循环 直到NextDevice为NULL
	*/
	ULONG			Count = 0;
	ULONG			uAttchNum = 0;
	PDEVICE_OBJECT CurrentDevice = NULL;
	PDEVICE_OBJECT pAttachedDevice = NULL;
	PDEVICE_OBJECT	LastDevice = NULL;
	PDRIVER_OBJECT pDrvObj = NULL;
	PDEVICE_DRIVER_INFO pInfo = NULL;
	PLDR_DATA_TABLE_ENTRY pLdr = NULL;
	PFILTER_DRIVER_INFO pFilterInfo = NULL;

	if (!EnumDriver(g_DriverObject, TRUE, &pInfo))
		return NULL;

	pFilterInfo = ExAllocatePool(NonPagedPool, sizeof(FILTER_DRIVER_INFO) * 500);

	if (pFilterInfo == NULL)
	{
		ExFreePool(pInfo);
		return NULL;
	}

	RtlZeroMemory(pFilterInfo, sizeof(FILTER_DRIVER_INFO) * 500);

	Count = pInfo[0].Count;

	for (size_t i = 0; i < Count; i++)
	{
		pDrvObj = (PDRIVER_OBJECT)pInfo[i].pDrvObj;
		CurrentDevice = pDrvObj->DeviceObject;
		if (CurrentDevice == NULL || CurrentDevice->AttachedDevice == NULL )
			continue;
		// Ldr = (PLDR_DATA_TABLE_ENTRY)(AttachObject->DriverObject->DriverSection)
		while (CurrentDevice != NULL)
		{
			if (MmIsAddressValid(CurrentDevice) &&
				MmIsAddressValid(CurrentDevice->AttachedDevice))
			{
				pLdr = (PLDR_DATA_TABLE_ENTRY)CurrentDevice->AttachedDevice->DriverObject->DriverSection;
				/*DbgPrint(
					"ObjectType:%d\r\n"
					"AttachedDevice:0x%p\r\n"
					"SysName:%wZ\r\n"
					"HostSysName:%wZ\r\n"
					"SysPath:%wZ\r\n:"
					, CurrentDevice->AttachedDevice->DeviceType
					, CurrentDevice->AttachedDevice
					, &CurrentDevice->AttachedDevice->DriverObject->DriverName
					, &pDrvObj->DriverName
					, &pLdr->FullDllName
				);*/
				
				pFilterInfo[uAttchNum].DeviceType = CurrentDevice->AttachedDevice->DeviceType;
				pFilterInfo[uAttchNum].AttachedDevice = CurrentDevice->AttachedDevice;

				RtlZeroMemory(pFilterInfo[uAttchNum].SysName, MAX_PATH * 2);
				RtlCopyMemory(pFilterInfo[uAttchNum].SysName, CurrentDevice->AttachedDevice->DriverObject->DriverName.Buffer, CurrentDevice->AttachedDevice->DriverObject->DriverName.Length );

				RtlZeroMemory(pFilterInfo[uAttchNum].HostSysName, MAX_PATH * 2);
				RtlCopyMemory(pFilterInfo[uAttchNum].HostSysName, pDrvObj->DriverName.Buffer, pDrvObj->DriverName.Length);

				if (pLdr->FullDllName.Buffer != NULL)
				{
					RtlZeroMemory(pFilterInfo[uAttchNum].HostSysPath, MAX_PATH * 2);
					RtlCopyMemory(pFilterInfo[uAttchNum].HostSysPath, pLdr->FullDllName.Buffer, pLdr->FullDllName.Length * 2);
				}

				uAttchNum++;

				// 指向上一个附加设备
				if (MmIsAddressValid(CurrentDevice->AttachedDevice->AttachedDevice))
				{
					pAttachedDevice = CurrentDevice->AttachedDevice->AttachedDevice;
					LastDevice = CurrentDevice->AttachedDevice;
					while (pAttachedDevice)
					{
						pLdr = (PLDR_DATA_TABLE_ENTRY)pAttachedDevice->DriverObject->DriverSection;
						pFilterInfo[uAttchNum].AttachedDevice = pAttachedDevice;

						pFilterInfo[uAttchNum].DeviceType = pAttachedDevice->DeviceType;

						RtlZeroMemory(pFilterInfo[uAttchNum].SysName, MAX_PATH * 2);
						RtlCopyMemory(pFilterInfo[uAttchNum].SysName, pAttachedDevice->DriverObject->DriverName.Buffer, pAttachedDevice->DriverObject->DriverName.Length);

						RtlZeroMemory(pFilterInfo[uAttchNum].HostSysName, MAX_PATH * 2);
						RtlCopyMemory(pFilterInfo[uAttchNum].HostSysName, LastDevice->DriverObject->DriverName.Buffer, LastDevice->DriverObject->DriverName.Length);

						if (pLdr->FullDllName.Buffer != NULL)
						{
							RtlZeroMemory(pFilterInfo[uAttchNum].HostSysPath, MAX_PATH * 2);
							RtlCopyMemory(pFilterInfo[uAttchNum].HostSysPath, pLdr->FullDllName.Buffer, pLdr->FullDllName.Length * 2);
						}

						uAttchNum++;

						if (MmIsAddressValid(pAttachedDevice->AttachedDevice))
						{
							LastDevice = pAttachedDevice;
							pAttachedDevice = pAttachedDevice->AttachedDevice;	
						}
						else
						{
							break;
						}
					}

				}
			}
			else
			{
				break;
			}
			CurrentDevice = CurrentDevice->NextDevice;
		}
	}

	DbgPrint("nCount:%d\r\n", uAttchNum);

	pFilterInfo[0].Count = uAttchNum;
	ExFreePool(pInfo);
	return pFilterInfo;
}

VOID DisableAllFilters(PWSTR lpwName, SIZE_T *OriFsFlt, ULONG MaxCount, ULONG Action)
{
	ULONG i = 0;
	UNICODE_STRING TName;
	PDRIVER_OBJECT TDrvObj;
	PDEVICE_OBJECT CurrentDevice;
	NTSTATUS status;
	RtlInitUnicodeString(&TName, lpwName);
	status = ObReferenceObjectByName(&TName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		&TDrvObj);

	if (!NT_SUCCESS(status)) return;
	if (!TDrvObj) return;
	CurrentDevice = TDrvObj->DeviceObject;
	while (CurrentDevice != NULL)
	{
		if (!Action)	//bypass
		{
			OriFsFlt[i] = (SIZE_T)InterlockedExchangePointer((PVOID*)&CurrentDevice->AttachedDevice, NULL);
		}
		else
		{
			OriFsFlt[i] = (SIZE_T)InterlockedExchangePointer((PVOID*)&CurrentDevice->AttachedDevice, (PDEVICE_OBJECT)(OriFsFlt[i]));
		}
		CurrentDevice = CurrentDevice->NextDevice;
		i++;
		if (i >= MaxCount)
			break;
	}
	ObDereferenceObject(TDrvObj);
	return;
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


BOOLEAN EnumDriver(PDRIVER_OBJECT pDriverObject, BOOLEAN bEnumDeviceDriver, PDEVICE_DRIVER_INFO *lpDeviceDriverBuff)
{
	ULONG count = 0;
	ULONG uDeviceDriverNum = 0;
	PKLDR_DATA_TABLE_ENTRY entry = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PKLDR_DATA_TABLE_ENTRY firstentry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Blink;
	WCHAR *szDriver = (WCHAR*)ExAllocatePool(NonPagedPool, MAX_PATH * 2);
	PDRIVER_OBJECT pOutDriverObject = NULL;
	PDEVICE_DRIVER_INFO pDeviceDriverInfo = NULL;
	WCHAR szPnpManager[MAX_PATH] = L"PnpManager";
	pDriverInfo = (PDRIVER_INFO)ExAllocatePool(NonPagedPool, sizeof(DRIVER_INFO)*500);
	
	if(pDriverInfo == NULL)
		return FALSE;

	// 枚举过滤驱动用
	if (bEnumDeviceDriver)
	{
		pDeviceDriverInfo = ExAllocatePool(NonPagedPool, sizeof(DEVICE_DRIVER_INFO) * 500);
		if (pDeviceDriverInfo == NULL)
			goto Exit;

		RtlZeroMemory(pDeviceDriverInfo, sizeof(DEVICE_DRIVER_INFO) * 500);
	}

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

		if (bEnumDeviceDriver && pOutDriverObject->DeviceObject)
		{
			pDeviceDriverInfo[uDeviceDriverNum].pDrvObj = (ULONG_PTR)pOutDriverObject;

			RtlCopyMemory(pDeviceDriverInfo[uDeviceDriverNum].szObjName, firstentry->BaseDllName.Buffer, firstentry->BaseDllName.Length - 8);
			uDeviceDriverNum++;
		}
		
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

				if (bEnumDeviceDriver && pOutDriverObject->DeviceObject)
				{
					pDeviceDriverInfo[uDeviceDriverNum].pDrvObj = (ULONG_PTR)pOutDriverObject;

					RtlCopyMemory(pDeviceDriverInfo[uDeviceDriverNum].szObjName, entry->BaseDllName.Buffer, entry->BaseDllName.Length - 8);
					uDeviceDriverNum++;
				}

			}
			//EnumDriverMajorFunction(szDriver);
			pOutDriverObject = NULL;
			count++;
		}
		entry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}

	pDriverInfo[0].Count = count;
	if (bEnumDeviceDriver)
	{
		PDRIVER_OBJECT pDrv = NULL;
		if (NT_SUCCESS(GetDriverObject(szPnpManager, &pDrv)))
		{
			pDeviceDriverInfo[uDeviceDriverNum].pDrvObj = pDrv;
			pDeviceDriverInfo[0].Count = uDeviceDriverNum + 1;
		}
		else
		{
			pDeviceDriverInfo[0].Count = uDeviceDriverNum;
		}
		
	}
		

Exit:
	if(count == 0)
	{
		if(pDriverInfo)
			ExFreePool(pDriverInfo);
		pDriverInfo = NULL;
		if(szDriver)
			ExFreePool(szDriver);
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
	if (bEnumDeviceDriver)
	{
		DbgPrint("设备驱动:%d\n", pDeviceDriverInfo[0].Count);
		ExFreePool(pDriverInfo);
		*lpDeviceDriverBuff = pDeviceDriverInfo;
	}

	//ExFreePool(pDriverInfo);
	return TRUE;
}