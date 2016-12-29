#pragma once

typedef struct _MY_IO_TIMER                        // 6 elements, 0x30 bytes (sizeof) 
{
	/*0x000*/     INT16        Type;
	/*0x002*/     INT16        TimerFlag;
	/*0x004*/     UINT8        _PADDING0_[0x4];
	/*0x008*/     struct _LIST_ENTRY TimerList;               // 2 elements, 0x10 bytes (sizeof) 
	/*0x018*/     PVOID		TimerRoutine;
	/*0x020*/     VOID*        Context;
	/*0x028*/     struct _DEVICE_OBJECT* DeviceObject;
}MY_IO_TIMER, *PMY_IO_TIMER;

typedef struct _IO_TIMER_INFO
{
	ULONG	count;
	ULONG	status;
	ULONG64	IoTimer;
	ULONG64	DeviceObject;
	ULONG64	TimerRoutine;
	CHAR	ImgPath[MAX_PATH];
}IO_TIMER_INFO, *PIO_TIMER_INFO;

PLIST_ENTRY GetIopTimerQueueHead(PUCHAR pIoInitializeTimer)
{
	/*
	nt!IoInitializeTimer+0x53:
	fffff800`042c8403 41bb09000000    mov     r11d,9
	fffff800`042c8409 48897b28        mov     qword ptr [rbx+28h],rdi
	fffff800`042c840d 6644891b        mov     word ptr [rbx],r11w
	fffff800`042c8411 48895f28        mov     qword ptr [rdi+28h],rbx
	fffff800`042c8415 488d5308        lea     rdx,[rbx+8]
	fffff800`042c8419 4c8d0520ade3ff  lea     r8,[nt!IopTimerLock (fffff800`04103140)]
	fffff800`042c8420 488d0dd94ce0ff  lea     rcx,[nt!IopTimerQueueHead (fffff800`040cd100)
	fffff800`042c8427 48896b18        mov     qword ptr [rbx+18h],rbp
	// 48 8d 0d				+7 == 48
	*/
	ULONG64	uIopTimerQueueHead = 0;

	if (pIoInitializeTimer == NULL || !MmIsAddressValid(pIoInitializeTimer))
		return NULL;

	for (size_t i = 0; i < 0xFF; i++)
	{
		if (*pIoInitializeTimer == 0x48 &&
			*(pIoInitializeTimer + 1) == 0x8d &&
			*(pIoInitializeTimer + 2) == 0x0d &&
			*(pIoInitializeTimer + 7) == 0x48)
		{
			LARGE_INTEGER tmp;
			tmp.HighPart = 0xffffffff;

			RtlCopyMemory(&tmp.LowPart, pIoInitializeTimer + 3, 4);
			uIopTimerQueueHead = (ULONG64)pIoInitializeTimer + tmp.QuadPart + 7;
			DbgPrint("uIopTimerQueueHead:0x%p\n", uIopTimerQueueHead);
			return (PLIST_ENTRY)uIopTimerQueueHead;
		}

		pIoInitializeTimer++;
	}

	return NULL;
}

PIO_TIMER_INFO EnumIoTimer()
{
	ULONG n = 0;
	PLIST_ENTRY pNextList = NULL;
	PMY_IO_TIMER pTimer = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };
	PIO_TIMER_INFO pInfo = NULL;

	// ÕÒÌØÕ÷Âë
	PLIST_ENTRY pIopTimerQueueHead = GetIopTimerQueueHead((PUCHAR)IoInitializeTimer);
	if (pIopTimerQueueHead == NULL || !MmIsAddressValid(pIopTimerQueueHead))
		return NULL;

	pInfo = ExAllocatePool(NonPagedPool, sizeof(IO_TIMER_INFO) * 100);
	if (pInfo == NULL)
		return NULL;

	RtlZeroMemory(pInfo, sizeof(IO_TIMER_INFO) * 100);

	for (pNextList = pIopTimerQueueHead->Blink; pNextList != pIopTimerQueueHead; pNextList = pNextList->Blink)
	{
		pTimer = CONTAINING_RECORD(pNextList, MY_IO_TIMER, TimerList);

		if (MmIsAddressValid(pTimer) &&
			MmIsAddressValid(pTimer->DeviceObject) &&
			MmIsAddressValid(&pTimer->TimerFlag) &&
			MmIsAddressValid(pTimer->TimerRoutine))
		{
			/*DbgPrint(
				"pTimer:0x%p\r\n"
				"DeviceObject:0x%p\r\n"
				"Status:%d\r\n"
				"TimerRoutine:0x%p\r\n"
				"DriverName:%wZ\r\n"
				, pTimer
				, pTimer->DeviceObject
				, pTimer->TimerFlag
				, pTimer->TimerRoutine
				, &pTimer->DeviceObject->DriverObject->DriverName
			);*/

			pInfo[n].IoTimer = pTimer;
			pInfo[n].DeviceObject = pTimer->DeviceObject;
			pInfo[n].status = pTimer->TimerFlag;
			pInfo[n].TimerRoutine = pTimer->TimerRoutine;
			
			if(NT_SUCCESS(getSystemImageInfoByAddress(pInfo[n].TimerRoutine, &Sysmodule)) && strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pInfo[n].ImgPath, Sysmodule.ImageName, MAX_PATH);
			}

			RtlZeroMemory(&Sysmodule, sizeof(SYSTEM_MODULE));
			n++;
		}
	}

	pInfo[0].count = n;
	return pInfo;

}