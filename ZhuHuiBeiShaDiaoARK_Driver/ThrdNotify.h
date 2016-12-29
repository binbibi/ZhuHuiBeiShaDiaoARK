
void CreateThreadNotify1
(
    IN HANDLE  ProcessId,
    IN HANDLE  ThreadId,
    IN BOOLEAN  Create
)
{
	//DbgPrint("CreateThreadNotify1\n");
}

void CreateThreadNotify2
(
    IN HANDLE  ProcessId,
    IN HANDLE  ThreadId,
    IN BOOLEAN  Create
)
{
	//DbgPrint("CreateThreadNotify2\n");
}

void CreateThreadNotifyTest(BOOLEAN Remove)
{
	if(!Remove)
	{
		PsSetCreateThreadNotifyRoutine(CreateThreadNotify1);
		PsSetCreateThreadNotifyRoutine(CreateThreadNotify2);
	}
	else
	{
		PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify1);
		PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify2);
	}
}

ULONG64 FindPspCreateThreadNotifyRoutine()
{
	ULONG64			i=0,pCheckArea=0;
	UNICODE_STRING	unstrFunc;
	RtlInitUnicodeString(&unstrFunc, L"PsSetCreateThreadNotifyRoutine");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress (&unstrFunc);
	//DbgPrint("PsSetCreateThreadNotifyRoutine: %llx\n",pCheckArea);
	for(i=pCheckArea;i<pCheckArea+0xff;i++)
	{
		if(*(PUCHAR)i==0x48 && *(PUCHAR)(i+1)==0x8d && *(PUCHAR)(i+2)==0x0d)	//lea rcx,xxxx
		{
			LONG OffsetAddr=0;
			memcpy(&OffsetAddr,(PUCHAR)(i+3),4);
			return OffsetAddr+7+i;
		}
	}
	return 0;
}

PNOTIFY_INFO EnumCreateThreadNotify()
{
	int i=0;
	ULONG	count = 0;
	ULONG64	NotifyAddr=0,MagicPtr=0;
	ULONG64	PspCreateThreadNotifyRoutine=FindPspCreateThreadNotifyRoutine();

	PNOTIFY_INFO pNotify = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };

	pNotify = ExAllocatePool(NonPagedPool, sizeof(NOTIFY_INFO) * 100);

	if (pNotify == NULL)
		return NULL;

	//DbgPrint("PspCreateThreadNotifyRoutine: %llx\n",PspCreateThreadNotifyRoutine);
	if(!PspCreateThreadNotifyRoutine)
		return NULL;
	for(i=0;i<64;i++)
	{
		MagicPtr=PspCreateThreadNotifyRoutine+i*8;
		NotifyAddr=*(PULONG64)(MagicPtr);
		if(MmIsAddressValid((PVOID)NotifyAddr) && NotifyAddr!=0)
		{
			NotifyAddr=*(PULONG64)(NotifyAddr & 0xfffffffffffffff8);
			//DbgPrint("[CreateThread]%llx\n",NotifyAddr);

			pNotify[count].CallbacksAddr = NotifyAddr;
			pNotify[count].CallbackType = 2; // creatrprocess
			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(NotifyAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[count].ImgPath, Sysmodule.ImageName, MAX_PATH);
			}
			count++;
		}
	}

	pNotify[0].Count = count;
	return pNotify;

}