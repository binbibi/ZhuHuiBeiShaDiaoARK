#pragma once
#define dprintf				DbgPrint

ULONG NtBuildNumber = 0;
ULONG ObjectCallbackListOffset = 0;

extern	ULONG	offset_BasePriority;
extern	ULONG	offset_kernelStack;
extern	ULONG	offset_ThreadRoutine;
extern	ULONG	FltFilterOperationsOffset;

typedef struct _OB_CALLBACK
{
	LIST_ENTRY	ListEntry;
	ULONG64		Unknown;
	ULONG64		ObHandle;
	ULONG64		ObjTypeAddr;
	ULONG64		PreCall;
	ULONG64		PostCall;
} OB_CALLBACK, *POB_CALLBACK;

BOOLEAN GetVersionAndHardCode()
{
	BOOLEAN b = FALSE;
	RTL_OSVERSIONINFOW	osi;
	osi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	RtlFillMemory(&osi, sizeof(RTL_OSVERSIONINFOW), 0);
	RtlGetVersion(&osi);
	NtBuildNumber = osi.dwBuildNumber;
	DbgPrint("NtBuildNumber: %ld\n", NtBuildNumber);
	switch (NtBuildNumber)
	{
	case 7600:
	case 7601:
	{
		ObjectCallbackListOffset = 0xC0;
		/*
		#define Offset_BasePriority 0x1f1
		#define Offset_KernelStack 0x38
		// 这个要自己去找
		#define Offset_ThreadRoutine 0x2d8
		*/
		offset_BasePriority = 0x1f1;
		offset_kernelStack = 0x38;
		offset_ThreadRoutine = 0x2d8;
		FltFilterOperationsOffset = 0x188; //WIN7 OFFSET of fltmgr!_FLT_FILTER->PFLT_OPERATION_REGISTRATION
		b = TRUE;
		break;
	}
	case 9200:
	{
		ObjectCallbackListOffset = 0xC8;	//OBJECT_TYPE.CallbackList
		b = TRUE;
		break;
	}
	case 9600:
	{
		ObjectCallbackListOffset = 0xC8;	//OBJECT_TYPE.CallbackList
		b = TRUE;
		break;
	}
	default:
		break;
	}
	return b;
}

//VOID DisableObcallbacks(PVOID Address)
//{
//	KIRQL irql;
//	CHAR patchCode[] = "\x33\xC0\xC3";	//xor eax,eax + ret
//	if (!Address)
//		return;
//	if (MmIsAddressValid(Address))
//	{
//		irql = WPOFFx64();
//		memcpy(Address, patchCode, 3);
//		WPONx64(irql);
//	}
//}


POBCALLBACKS_INFO EnumObCallbacks()
{
	ULONG c = 0;
	POBCALLBACKS_INFO pNotify = NULL;
	PLIST_ENTRY CurrEntry = NULL;
	POB_CALLBACK pObCallback;
	SYSTEM_MODULE Sysmodule = { 0 };
	ULONG64 ObProcessCallbackListHead = *(ULONG64*)PsProcessType + ObjectCallbackListOffset;
	ULONG64 ObThreadCallbackListHead = *(ULONG64*)PsThreadType + ObjectCallbackListOffset;

	pNotify = ExAllocatePool(NonPagedPool, sizeof(OBCALLBACKS_INFO)*100);
	if (pNotify == NULL)
		return NULL;


	RtlZeroMemory(pNotify, sizeof(OBCALLBACKS_INFO) * 100);
	//
	//dprintf("ObProcessCallbackListHead: %p\n", ObProcessCallbackListHead);
	CurrEntry = ((PLIST_ENTRY)ObProcessCallbackListHead)->Flink;	//list_head的数据是垃圾数据，忽略
	do
	{
		pObCallback = (POB_CALLBACK)CurrEntry;
		if (pObCallback->ObHandle != 0)
		{
			//dprintf("ObHandle: %p\n", pObCallback->ObHandle);
			//dprintf("PreCall: %p\n", pObCallback->PreCall);
			//dprintf("PostCall: %p\n", pObCallback->PostCall);
			pNotify[c].PreCallbackAddr = pObCallback->PreCall;
			pNotify[c].PostCallbackAddr = pObCallback->PostCall;
			pNotify[c].ObHandle = pObCallback->ObHandle;
			pNotify[c].ObType = 0;

			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[c].PreCallbackAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[c].PreImgPath, Sysmodule.ImageName, MAX_PATH);
			}

			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[c].PostCallbackAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[c].PostImgPaht ,Sysmodule.ImageName, MAX_PATH);
			}

			c++;
		}
		CurrEntry = CurrEntry->Flink;
	} while (CurrEntry != (PLIST_ENTRY)ObProcessCallbackListHead);
	//
	//dprintf("ObThreadCallbackListHead: %p\n", ObThreadCallbackListHead);
	CurrEntry = ((PLIST_ENTRY)ObThreadCallbackListHead)->Flink;	//list_head的数据是垃圾数据，忽略
	do
	{
		pObCallback = (POB_CALLBACK)CurrEntry;
		if (pObCallback->ObHandle != 0)
		{
			//dprintf("ObHandle: %p\n", pObCallback->ObHandle);
			//dprintf("PreCall: %p\n", pObCallback->PreCall);
			//dprintf("PostCall: %p\n", pObCallback->PostCall);
			pNotify[c].PreCallbackAddr = pObCallback->PreCall;
			pNotify[c].PostCallbackAddr = pObCallback->PostCall;
			pNotify[c].ObHandle = pObCallback->ObHandle;
			pNotify[c].ObType = 1;

			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[c].PreCallbackAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[c].PreImgPath, Sysmodule.ImageName, MAX_PATH);
			}

			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[c].PostCallbackAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[c].PostImgPaht, Sysmodule.ImageName, MAX_PATH);
			}
			c++;
		}
		CurrEntry = CurrEntry->Flink;
	} while (CurrEntry != (PLIST_ENTRY)ObThreadCallbackListHead);
	//dprintf("ObCallback count: %ld\n", c);

	pNotify[0].Count = c;
	return pNotify;
}