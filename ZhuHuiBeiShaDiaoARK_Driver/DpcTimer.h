#pragma once

typedef struct _DPC_TIMER_
{
	ULONG Count;
	ULONG Period;// 周期
	ULONG_PTR TimerObject;			
	ULONG_PTR TimeRoutine;
	ULONG_PTR Dpc;
	CHAR	  ImgPath[MAX_PATH];
}DPC_TIMER, *PDPC_TIMER;

NTSTATUS RemoveDPCTimer(ULONG_PTR TimerObject)
{
	if (TimerObject&&MmIsAddressValid((PVOID)TimerObject))
	{

		if (KeCancelTimer((PKTIMER)TimerObject))
		{
			return STATUS_SUCCESS;
		}
	}
	return STATUS_UNSUCCESSFUL;
}


VOID FindKiWaitFunc(PULONG64 *KiWaitNeverAddr, PULONG64 *KiWaitAlwaysAddr)
{
	long Temp;
	PUCHAR StartAddress, i;
	UNICODE_STRING  uniFuncName;
	WCHAR wzFunName[] = L"KeSetTimer";
	RtlInitUnicodeString(&uniFuncName, wzFunName);
	StartAddress = (PUCHAR)MmGetSystemRoutineAddress(&uniFuncName);
	for (i = StartAddress; i<StartAddress + 0xFF; i++)
	{
		if (*i == 0x48 && *(i + 1) == 0x8B && *(i + 2) == 0x05)
		{
			memcpy(&Temp, i + 3, 4);
			*KiWaitNeverAddr = (PULONG64)((ULONGLONG)Temp + (ULONGLONG)i + 7);
			i = i + 7;
			memcpy(&Temp, i + 3, 4);
			*KiWaitAlwaysAddr = (PULONG64)((ULONGLONG)Temp + (ULONGLONG)i + 7);
			return;
		}
	}
}


KDPC* TransTimerDpcEx(
	IN PKTIMER InTimer,
	IN ULONGLONG InKiWaitNever,
	IN ULONGLONG InKiWaitAlways)
{
	ULONGLONG			RDX = (ULONGLONG)InTimer->Dpc;
	RDX ^= InKiWaitNever;
	RDX = _rotl64(RDX, (UCHAR)(InKiWaitNever & 0xFF));
	RDX ^= (ULONGLONG)InTimer;
	RDX = _byteswap_uint64(RDX);
	RDX ^= InKiWaitAlways;
	return (KDPC*)RDX;
}


PDPC_TIMER EnumDpcTimer()
{
	ULONG CPUNumber = KeNumberProcessors;   //系统变量
	PUCHAR CurrentKPRCBAddress = NULL;
	PUCHAR CurrentTimerTableEntry = NULL;
	PLIST_ENTRY CurrentEntry = NULL;
	PLIST_ENTRY NextEntry = NULL;
	PULONG64    KiWaitAlways = NULL;
	PULONG64    KiWaitNever = NULL;
	ULONG i = 0;
	ULONG j = 0;
	ULONG n = 0;
	PKTIMER Timer;
	PDPC_TIMER pDpcTimerInfo = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };

	typedef struct _KTIMER_TABLE_ENTRY
	{
		ULONG64			Lock;
		LIST_ENTRY		Entry;
		ULARGE_INTEGER	Time;
	} KTIMER_TABLE_ENTRY, *PKTIMER_TABLE_ENTRY;

	pDpcTimerInfo = ExAllocatePool(NonPagedPool, sizeof(DPC_TIMER)* 500);

	if (pDpcTimerInfo == NULL)
		return NULL;

	RtlZeroMemory(pDpcTimerInfo, sizeof(DPC_TIMER) * 500);
	RtlZeroMemory(&Sysmodule, sizeof(SYSTEM_MODULE));

	for (j = 0; j<CPUNumber; j++)
	{
		KeSetSystemAffinityThread(j + 1);   //使当前线程运行在第一个处理器上
		CurrentKPRCBAddress = (PUCHAR)__readmsr(0xC0000101) + 0x20;
		KeRevertToUserAffinityThread();   //恢复线程运行的处理器

		CurrentTimerTableEntry = (PUCHAR)(*(ULONG64*)CurrentKPRCBAddress + 0x2200 + 0x200);
		FindKiWaitFunc(&KiWaitNever, &KiWaitAlways);  //找KiWaitAlways 函数的地址
		for (i = 0; i<0x100; i++)
		{
			CurrentEntry = (PLIST_ENTRY)(CurrentTimerTableEntry + sizeof(KTIMER_TABLE_ENTRY) * i + 8);
			NextEntry = CurrentEntry->Blink;
			if (MmIsAddressValid(CurrentEntry) && MmIsAddressValid(CurrentEntry))
			{
				while (NextEntry != CurrentEntry)
				{
					PKDPC RealDpc;
					//获得首地址
					Timer = CONTAINING_RECORD(NextEntry, KTIMER, TimerListEntry);
					RealDpc = TransTimerDpcEx(Timer, *KiWaitNever, *KiWaitAlways);
					if (MmIsAddressValid(Timer) && MmIsAddressValid(RealDpc) && MmIsAddressValid(RealDpc->DeferredRoutine))
					{
						/*DbgPrint(
								"Dpc:0x%p\r\n"
								"Period:%d\r\n"
								"TimeDisPatch:0x%p\r\n"
								"TimerObject:0x%p\r\n"
								, RealDpc
								, Timer->Period
								, RealDpc->DeferredRoutine
								, Timer
							);*/

						RtlZeroMemory(&Sysmodule, sizeof(SYSTEM_MODULE));

						pDpcTimerInfo[n].Dpc = RealDpc;
						pDpcTimerInfo[n].Period = Timer->Period;
						pDpcTimerInfo[n].TimeRoutine = RealDpc->DeferredRoutine;
						pDpcTimerInfo[n].TimerObject = Timer;

						if (NT_SUCCESS(getSystemImageInfoByAddress(pDpcTimerInfo[n].TimeRoutine, &Sysmodule)) && strlen(Sysmodule.ImageName) < MAX_PATH)
						{
							RtlCopyMemory(pDpcTimerInfo[n].ImgPath, Sysmodule.ImageName, MAX_PATH);
						}

						n++;
					}
					NextEntry = NextEntry->Blink;
				}
			}
		}
	}

	pDpcTimerInfo[0].Count = n;
	DbgPrint("Count:%d\n",n);
	return pDpcTimerInfo;
}

