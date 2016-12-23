#pragma once
#pragma warning(disable:4214)
#pragma warning(disable:4201)
#include <ntddk.h>

/*
Type
Directory
SymbolicLink
Token
Job
Process
Thread
UserApcReserve
IoCompletionReserve
DebugObject
Event
EventPair
Mutant
Callback
Semaphore
Timer
Profile
KeyedEvent
WindowStation
Desktop
TpWorkerFactory
Adapter
Controller
Device
Driver
IoCompletion
File
TmTm
TmTx
TmRm
TmEn
Section
Session
Key
ALPC Port
PowerRequest
WmiGuid
EtwRegistration
EtwConsumer
FilterConnectionPort
FilterCommunicationPort
PcwObject
*/

#ifndef _MAX_PATH_
#define MAX_PATH 260
#endif

NTKERNELAPI PVOID NTAPI
ObGetObjectType(
	IN PVOID pObject
);

typedef struct _OBJECT_TYPE_INITIALIZER                                                                                                                                         // 26 elements, 0x70 bytes (sizeof) 
{
	/*0x000*/     UINT16       Length;
	union                                                                                                                                                                       // 2 elements, 0x1 bytes (sizeof)   
	{
		/*0x002*/         UINT8        ObjectTypeFlags;
		struct                                                                                                                                                                  // 8 elements, 0x1 bytes (sizeof)   
		{
			/*0x002*/             UINT8        CaseInsensitive : 1;                                                                                                                                   // 0 BitPosition                    
			/*0x002*/             UINT8        UnnamedObjectsOnly : 1;                                                                                                                                // 1 BitPosition                    
			/*0x002*/             UINT8        UseDefaultObject : 1;                                                                                                                                  // 2 BitPosition                    
			/*0x002*/             UINT8        SecurityRequired : 1;                                                                                                                                  // 3 BitPosition                    
			/*0x002*/             UINT8        MaintainHandleCount : 1;                                                                                                                               // 4 BitPosition                    
			/*0x002*/             UINT8        MaintainTypeList : 1;                                                                                                                                  // 5 BitPosition                    
			/*0x002*/             UINT8        SupportsObjectCallbacks : 1;                                                                                                                           // 6 BitPosition                    
			/*0x002*/             UINT8        CacheAligned : 1;                                                                                                                                      // 7 BitPosition                    
		};
	};
	/*0x004*/     ULONG32      ObjectTypeCode;
	/*0x008*/     ULONG32      InvalidAttributes;
	/*0x00C*/     struct _GENERIC_MAPPING GenericMapping;                                                                                                                                     // 4 elements, 0x10 bytes (sizeof)  
	/*0x01C*/     ULONG32      ValidAccessMask;
	/*0x020*/     ULONG32      RetainAccess;
	/*0x024*/     enum _POOL_TYPE PoolType;
	/*0x028*/     ULONG32      DefaultPagedPoolCharge;
	/*0x02C*/     ULONG32      DefaultNonPagedPoolCharge;
	/*0x030*/     ULONG64 DumpProcedure;
	/*0x038*/     ULONG64 OpenProcedure;
	/*0x040*/     ULONG64 CloseProcedure;
	/*0x048*/     ULONG64 DeleteProcedure;
	/*0x050*/     ULONG64 ParseProcedure;
	/*0x058*/     ULONG64 SecurityProcedure;
	/*0x060*/     ULONG64 QueryNameProcedure;
	/*0x068*/     ULONG64 OkayToCloseProcedure;
}OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

typedef struct _EX_PUSH_LOCKE                 // 7 elements, 0x8 bytes (sizeof) 
{
	union                                    // 3 elements, 0x8 bytes (sizeof) 
	{
		struct                               // 5 elements, 0x8 bytes (sizeof) 
		{
			/*0x000*/             UINT64       Locked : 1;         // 0 BitPosition                  
			/*0x000*/             UINT64       Waiting : 1;        // 1 BitPosition                  
			/*0x000*/             UINT64       Waking : 1;         // 2 BitPosition                  
			/*0x000*/             UINT64       MultipleShared : 1; // 3 BitPosition                  
			/*0x000*/             UINT64       Shared : 60;        // 4 BitPosition                  
		};
		/*0x000*/         UINT64       Value;
		/*0x000*/         VOID*        Ptr;
	};
}EX_PUSH_LOCKE, *PEX_PUSH_LOCKE;

typedef struct _MYOBJECT_TYPEEX                   // 12 elements, 0xD0 bytes (sizeof) 
{
	/*0x000*/     struct _LIST_ENTRY TypeList;              // 2 elements, 0x10 bytes (sizeof)  
	/*0x010*/     struct _UNICODE_STRING Name;              // 3 elements, 0x10 bytes (sizeof)  
	/*0x020*/     VOID*        DefaultObject;
	/*0x028*/     UINT8        Index;
	/*0x029*/     UINT8        _PADDING0_[0x3];
	/*0x02C*/     ULONG32      TotalNumberOfObjects;
	/*0x030*/     ULONG32      TotalNumberOfHandles;
	/*0x034*/     ULONG32      HighWaterNumberOfObjects;
	/*0x038*/     ULONG32      HighWaterNumberOfHandles;
	/*0x03C*/     UINT8        _PADDING1_[0x4];
	/*0x040*/     struct _OBJECT_TYPE_INITIALIZER TypeInfo; // 26 elements, 0x70 bytes (sizeof) 
	/*0x0B0*/     struct _EX_PUSH_LOCKE TypeLock;            // 7 elements, 0x8 bytes (sizeof)   
	/*0x0B8*/     ULONG32      Key;
	/*0x0BC*/     UINT8        _PADDING2_[0x4];
	/*0x0C0*/     struct _LIST_ENTRY CallbackList;          // 2 elements, 0x10 bytes (sizeof)  
}MYOBJECT_TYPEEX, *MYPOBJECT_TYPEEX;


typedef struct _OBJECT_TYPE_INFO
{
	// 是否支持回掉
	BOOLEAN	bSupportsObjectCallbacks;
	// 第0个才用到 标记有多少个对象
	ULONG	Count;
	// index
	ULONG	index;
	// hehe
	ULONG	ValidAccessMask;
	ULONG64	ObjectAddr;
	/*ULONG64	DumpProcedrure;
	ULONG64	OpenProcedure;
	ULONG64	CloseProcedrure;
	ULONG64	DeleteProcedure;
	ULONG64	ParseProcedure;
	ULONG64	SecurityProcedure;
	ULONG64	QueryNameProcedure;
	ULONG64	OkayToCloseProcedure;*/
	CHAR	imgPath[MAX_PATH];
	WCHAR	ObjectName[MAX_PATH];
}OBJECT_TYPE_INFO, *POBJECT_TYPE_INFO;

typedef struct _OBJECT_PROCEDURE_INFO
{
	ULONG64	DumpProcedrure;
	ULONG64	OpenProcedure;
	ULONG64	CloseProcedrure;
	ULONG64	DeleteProcedure;
	ULONG64	ParseProcedure;
	ULONG64	SecurityProcedure;
	ULONG64	QueryNameProcedure;
	ULONG64	OkayToCloseProcedure;
	CHAR	DumpImgPath[MAX_PATH];
	CHAR	OpenImgPath[MAX_PATH];
	CHAR	CloseImgPath[MAX_PATH];
	CHAR	DeleteImgPath[MAX_PATH];
	CHAR	ParseImgPath[MAX_PATH];
	CHAR	SecurityImgPath[MAX_PATH];
	CHAR	QueryNameImgPath[MAX_PATH];
	CHAR	OkayToCloseImgPath[MAX_PATH];
}OBJECT_PROCEDURE_INFO, *POBJECT_PROCEDURE_INFO;

MYPOBJECT_TYPEEX GetObjectTypeByNameWin7x64(WCHAR *ObjectName)
{
	BOOLEAN	i = 2;
	ULONG64 ObjectType;
	UNICODE_STRING	usObjectName = {0};
	usObjectName.Buffer = (WCHAR*)ExAllocatePool(NonPagedPool, MAX_PATH*2);
	usObjectName.Length = usObjectName.MaximumLength = MAX_PATH * 2;

	RtlZeroMemory(usObjectName.Buffer, MAX_PATH * 2);

	while (ObGetObjectType(&i + 0x18))
	{
		ObjectType = (ULONG64)ObGetObjectType(&i + 0x18);
		KdPrint(("对象类型[%d]=%wZ\r\nAddress:0x%p\n", i, ObjectType + 0x10, ObjectType));
		RtlCopyUnicodeString(&usObjectName,	(PUNICODE_STRING)(ObjectType + 0x10));
		if (wcsstr(usObjectName.Buffer, ObjectName)){
			RtlFreeUnicodeString(&usObjectName);
			return (MYPOBJECT_TYPEEX)ObjectType;
		}
		i++;
	}

	RtlFreeUnicodeString(&usObjectName);

	return NULL;

	/*
	ObGetObjectType(&i + 0x18) 里面就要一个地址我们给他i的地址, + 0x18就是还原TypeIndex的
	相当于把这个指令变为
	movzx   eax,byte ptr [rcx-18h]  --> movzx    eax,i

	nt!ObGetObjectType:
	fffff800`0613c854 0fb641e8        movzx   eax,byte ptr [rcx-18h] //rcx-18h = rcx - sizeof(OBJECT_TYPE)0x30 + TypeIndex偏移0x18
	fffff800`0613c858 488d0da1e2efff  lea     rcx,[nt!ObTypeIndexTable (fffff800`0603ab00)]
	fffff800`0613c85f 488b04c1        mov     rax,qword ptr [rcx+rax*8]
	fffff800`0613c863 c3              ret
	*/
}

BOOLEAN GetObjectProcedureByIndex(UCHAR index ,_Inout_ PVOID *lpProcedureInfo)
{
	POBJECT_PROCEDURE_INFO pObjectProcedureInfo = (POBJECT_PROCEDURE_INFO)ExAllocatePool(NonPagedPool, sizeof(OBJECT_PROCEDURE_INFO));
	MYPOBJECT_TYPEEX pObjectType = NULL;
	SYSTEM_MODULE sysmodule = { 0 };
	BOOLEAN i = index;

	if (i < 2)
		i = 2;
	if (pObjectProcedureInfo == NULL)
		return FALSE;

	RtlZeroMemory(pObjectProcedureInfo, sizeof(OBJECT_PROCEDURE_INFO));


	pObjectType = (MYPOBJECT_TYPEEX)ObGetObjectType(&index + 0x18);
	if (pObjectType == NULL)
	{
		ExFreePool(pObjectProcedureInfo);
		pObjectProcedureInfo = NULL;
		*lpProcedureInfo = NULL;
		return FALSE;
	}


	pObjectProcedureInfo->CloseProcedrure = pObjectType->TypeInfo.CloseProcedure;
	if (pObjectProcedureInfo->CloseProcedrure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->CloseProcedrure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->CloseImgPath, sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->DeleteProcedure = pObjectType->TypeInfo.DeleteProcedure;
	if (pObjectProcedureInfo->DeleteProcedure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->DeleteProcedure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->DeleteImgPath, sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->DumpProcedrure = pObjectType->TypeInfo.DumpProcedure;
	if (pObjectProcedureInfo->DumpProcedrure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->DumpProcedrure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->DumpImgPath, sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->OkayToCloseProcedure = pObjectType->TypeInfo.OkayToCloseProcedure;
	if (pObjectProcedureInfo->OkayToCloseProcedure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->OkayToCloseProcedure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->OkayToCloseImgPath ,sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->OpenProcedure = pObjectType->TypeInfo.OpenProcedure;
	if (pObjectProcedureInfo->OpenProcedure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->OpenProcedure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->OpenImgPath, sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->ParseProcedure = pObjectType->TypeInfo.ParseProcedure;
	if (pObjectProcedureInfo->ParseProcedure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->ParseProcedure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->ParseImgPath, sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->QueryNameProcedure = pObjectType->TypeInfo.QueryNameProcedure;
	if (pObjectProcedureInfo->QueryNameProcedure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->QueryNameProcedure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->QueryNameImgPath, sysmodule.ImageName, 256);
		}
	}

	RtlZeroMemory(&sysmodule, sizeof(sysmodule));

	pObjectProcedureInfo->SecurityProcedure = pObjectType->TypeInfo.SecurityProcedure;
	if (pObjectProcedureInfo->SecurityProcedure != NULL)
	{
		if (NT_SUCCESS(getSystemImageInfoByAddress(pObjectProcedureInfo->SecurityProcedure, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pObjectProcedureInfo->SecurityImgPath, sysmodule.ImageName, 256);
		}
	}


	*(ULONG64*)lpProcedureInfo = (ULONG64)pObjectProcedureInfo;
	return TRUE;
}

void EnumObjectTypeInfo(_Inout_ PVOID *lpObjectInfo)
{
	BOOLEAN index = 2;
	ULONG Count = 0;
	POBJECT_TYPE_INFO pObjectInfo = NULL;
	MYPOBJECT_TYPEEX pObjectType = NULL;
	//SYSTEM_MODULE sysmodule = { 0 };
	pObjectInfo = ExAllocatePool(NonPagedPool, sizeof(OBJECT_TYPE_INFO) * 50);

	if (pObjectInfo == NULL)
		return;

	RtlZeroMemory(pObjectInfo, sizeof(OBJECT_TYPE_INFO) * 50);

	while (ObGetObjectType(&index + 0x18))
	{
		pObjectType = (MYPOBJECT_TYPEEX)ObGetObjectType(&index + 0x18);
		
		/*if (NT_SUCCESS(getSystemImageInfoByAddress((ULONG_PTR)pObjectType, &sysmodule)) &&
			strlen(sysmodule.ImageName) < MAX_PATH)
		{
			DbgPrint("%s\n", sysmodule.ImageName);
			RtlCopyMemory(pObjectInfo[Count].imgPath, sysmodule.ImageName, 256);
		}*/

		pObjectInfo[Count].index = index;
		pObjectInfo[Count].bSupportsObjectCallbacks = pObjectType->TypeInfo.SupportsObjectCallbacks;
		/*pObjectInfo[Count].CloseProcedrure = pObjectType->TypeInfo.CloseProcedure;
		pObjectInfo[Count].DeleteProcedure = pObjectType->TypeInfo.DeleteProcedure;
		pObjectInfo[Count].DumpProcedrure = pObjectType->TypeInfo.DumpProcedure;
		pObjectInfo[Count].OkayToCloseProcedure = pObjectType->TypeInfo.OkayToCloseProcedure;
		pObjectInfo[Count].OpenProcedure = pObjectType->TypeInfo.OpenProcedure;
		pObjectInfo[Count].ParseProcedure = pObjectType->TypeInfo.ParseProcedure;
		pObjectInfo[Count].QueryNameProcedure = pObjectType->TypeInfo.QueryNameProcedure;
		pObjectInfo[Count].SecurityProcedure = pObjectType->TypeInfo.SecurityProcedure;*/
		pObjectInfo[Count].ValidAccessMask = pObjectType->TypeInfo.ValidAccessMask;
		pObjectInfo[Count].ObjectAddr = (ULONG64)pObjectType;
		RtlCopyMemory(pObjectInfo[Count].ObjectName, pObjectType->Name.Buffer, pObjectType->Name.Length * 2);
		Count++;
		index++;

		
	}

	pObjectInfo[0].Count = Count;

	*(ULONG64*)lpObjectInfo = (ULONG64)pObjectInfo;

}
