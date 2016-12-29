#pragma once
#include <ntddk.h>

/*
作者：Tesla.Angela


关机回调和蓝屏回调就是在关机前和蓝屏前执行的回调，可能被某些病毒木马用于关机前或蓝屏前回写。所以ARK工具也提供枚举和删除这些回调的功能。
关机回调只有1种（ShutdownNotification），不过蓝屏回调有2种：BugCheckCallback和BugCheckReasonCallback。
这2种（或者说3种）回调有类似的地方：都存在于一个结构体链表里。找到各自的链表头，即可枚举出所有的关机和蓝屏回调。
不过关机回调略为奇葩：它的回调地址并非记录在系统的结构体连表里，而是记录在各个驱动的分发例程（IRP_MJ_SHUTDOWN）里。
代码跟枚举CreateProcess、CreateThread、LoadImage等回调大同小异。*/


ULONG64 FindIopNotifyShutdownQueueHead()
{
	ULONG64			i = 0, pCheckArea = 0;
	UNICODE_STRING	unstrFunc;
	RtlInitUnicodeString(&unstrFunc, L"IoRegisterShutdownNotification");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&unstrFunc);
	//DbgPrint("IoRegisterShutdownNotification: %llx\n", pCheckArea);
	for (i = pCheckArea; i<pCheckArea + 0xff; i++)
	{
		if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x0d)	//lea rcx,xxxx
		{
			LONG OffsetAddr = 0;
			memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
			return OffsetAddr + 7 + i;
		}
	}
	return 0;
}

PNOTIFY_INFO EnumShutdownCallback()
{
	PDEVICE_OBJECT DevObj = NULL;
	PDRIVER_OBJECT DrvObj = NULL;
	PLIST_ENTRY g_IopNotifyShutdownQueueHead = NULL;
	PLIST_ENTRY CurrEntry = NULL;
	ULONG64 Dispatch = 0;
	ULONG	i = 0;
	PNOTIFY_INFO pNotify = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };
	ULONG64 IopNotifyShutdownQueueHead = FindIopNotifyShutdownQueueHead();

	pNotify = ExAllocatePool(NonPagedPool, sizeof(NOTIFY_INFO) * 100);

	if (pNotify == NULL)
		return NULL;


	//DbgPrint("IopNotifyShutdownQueueHead: %llx\n", IopNotifyShutdownQueueHead);
	g_IopNotifyShutdownQueueHead = (PLIST_ENTRY)IopNotifyShutdownQueueHead;
	//DbgPrint("Callback\t\tDrvName\t\tDevObj\n");
	for (CurrEntry = g_IopNotifyShutdownQueueHead->Flink;
		CurrEntry != g_IopNotifyShutdownQueueHead;
		CurrEntry = CurrEntry->Flink)
	{
		DevObj = (PDEVICE_OBJECT)(*(PULONG64)((ULONG64)CurrEntry + sizeof(LIST_ENTRY)));
		DrvObj = DevObj->DriverObject;
		Dispatch = (ULONG64)(DrvObj->MajorFunction[IRP_MJ_SHUTDOWN]);
		//DbgPrint("[shutdown]0x%X\t\t%wZ\t\t0x%X\n",Dispatch,&DrvObj->DriverName,DevObj);
		//DbgPrint("[Shutdown]%llx\n", Dispatch);

		pNotify[i].CallbacksAddr = Dispatch;
		pNotify[i].CallbackType = 4; // shutdown

		if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[i].CallbacksAddr, &Sysmodule)) &&
			strlen(Sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pNotify[i].ImgPath, Sysmodule.ImageName, MAX_PATH);
		}
		i++;

	}

	pNotify[0].Count = i;

	return pNotify;
}

ULONG64 GetKeBugCheckReasonCallbackListHead() //取消可用：KeDeregisterBugCheckReasonCallback
{
	ULONG64			i = 0, pCheckArea = 0;
	UNICODE_STRING	unstrFunc;
	RtlInitUnicodeString(&unstrFunc, L"KeRegisterBugCheckReasonCallback");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&unstrFunc);
	//DbgPrint("KeRegisterBugCheckReasonCallback: %llx\n", pCheckArea);
	for (i = pCheckArea; i<pCheckArea + 0xfff; i++)
	{
		if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8b && *(PUCHAR)(i + 2) == 0x05)	//mov rax,xxxx
		{
			LONG OffsetAddr = 0;
			memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
			return OffsetAddr + 7 + i;
		}
	}
	return 0;
}

ULONG64 GetKeBugCheckCallbackListHead() //取消可用：KeDeregisterBugCheckCallback
{
	ULONG64			i = 0, pCheckArea = 0;
	UNICODE_STRING	unstrFunc;
	RtlInitUnicodeString(&unstrFunc, L"KeRegisterBugCheckCallback");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&unstrFunc);
	//DbgPrint("KeRegisterBugCheckCallback: %llx\n", pCheckArea);
	for (i = pCheckArea; i<pCheckArea + 0xfff; i++)
	{
		if (*(PUCHAR)i == 0x48 && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x0d &&
			*(PUCHAR)(i - 1) == 0xc1 && *(PUCHAR)(i - 2) == 0x03 && *(PUCHAR)(i - 3) == 0x48)	//lea rcx,xxxx
		{
			LONG OffsetAddr = 0;
			memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
			return OffsetAddr + 7 + i;
		}
	}
	return 0;
}

PNOTIFY_INFO EnumBugcheckCallback()
{
	ULONG64 Dispatch = 0;
	ULONG	i = 0;
	PNOTIFY_INFO pNotify = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };
	PLIST_ENTRY CurrEntry = NULL, g_KeBugCheckCallbackListHead = NULL, g_KeBugCheckReasonCallbackListHead = NULL;
	ULONG64 KeBugCheckReasonCallbackListHead, KeBugCheckCallbackListHead;
	KeBugCheckReasonCallbackListHead = GetKeBugCheckReasonCallbackListHead();
	KeBugCheckCallbackListHead = GetKeBugCheckCallbackListHead();
	//DbgPrint("KeBugCheckReasonCallbackListHead: %llx\n", KeBugCheckReasonCallbackListHead);
	//DbgPrint("KeBugCheckCallbackListHead: %llx\n", KeBugCheckCallbackListHead);
	g_KeBugCheckReasonCallbackListHead = (PLIST_ENTRY)KeBugCheckReasonCallbackListHead;
	g_KeBugCheckCallbackListHead = (PLIST_ENTRY)KeBugCheckCallbackListHead;

	pNotify = ExAllocatePool(NonPagedPool, sizeof(NOTIFY_INFO) * 100);

	if (pNotify == NULL)
		return NULL;

	//enum bugcheck reason callback
	CurrEntry = g_KeBugCheckReasonCallbackListHead->Flink;
	do
	{
		Dispatch = *(ULONG64 *)((ULONG64)CurrEntry + sizeof(LIST_ENTRY));
		//DbgPrint("Entry:%llx\n", CurrEntry);
		//DbgPrint("[BugCheckReason]%llx\n", Dispatch);
		CurrEntry = CurrEntry->Flink;

		pNotify[i].CallbacksAddr = Dispatch;
		pNotify[i].CallbackType = 6; // BugCheckReason
		memset(&Sysmodule,0,sizeof(SYSTEM_MODULE));

		if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[i].CallbacksAddr, &Sysmodule)) &&
			strlen(Sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pNotify[i].ImgPath, Sysmodule.ImageName, MAX_PATH);
		}
		i++;

	} while (CurrEntry != g_KeBugCheckReasonCallbackListHead);
	//enum bugcheck callback
	CurrEntry = g_KeBugCheckCallbackListHead->Flink;
	do
	{
		Dispatch = *(ULONG64 *)((ULONG64)CurrEntry + sizeof(LIST_ENTRY));
		//DbgPrint("Entry:%llx\n", CurrEntry);
		//DbgPrint("[BugCheck]%llx\n", Dispatch);
		CurrEntry = CurrEntry->Flink;

		pNotify[i].CallbacksAddr = Dispatch;
		pNotify[i].CallbackType = 5; // BugCheck
		memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));

		if (NT_SUCCESS(getSystemImageInfoByAddress(pNotify[i].CallbacksAddr, &Sysmodule)) &&
			strlen(Sysmodule.ImageName) < MAX_PATH)
		{
			RtlCopyMemory(pNotify[i].ImgPath, Sysmodule.ImageName, MAX_PATH);
		}
		i++;

	} while (CurrEntry != g_KeBugCheckCallbackListHead);

	pNotify[0].Count = i;
	return pNotify;
}