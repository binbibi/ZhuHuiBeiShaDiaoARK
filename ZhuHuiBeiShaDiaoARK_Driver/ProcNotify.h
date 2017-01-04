
ULONG64 FindPspCreateProcessNotifyRoutine()
{
	LONG			OffsetAddr=0;
	ULONG64			i=0,pCheckArea=0;
	UNICODE_STRING	unstrFunc;
	//获得PsSetCreateProcessNotifyRoutine的地址
	RtlInitUnicodeString(&unstrFunc, L"PsSetCreateProcessNotifyRoutine");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress (&unstrFunc);
	//获得PspSetCreateProcessNotifyRoutine的地址
	memcpy(&OffsetAddr,(PUCHAR)pCheckArea+4,4);
	pCheckArea=(pCheckArea+3)+5+OffsetAddr;
	//获得PspCreateProcessNotifyRoutine的地址
	for(i=pCheckArea;i<pCheckArea+0xff;i++)
	{
		if(*(PUCHAR)i==0x4c && *(PUCHAR)(i+1)==0x8d && *(PUCHAR)(i+2)==0x35)	//lea r14,xxxx
		{
			LONG OffsetAddrtmp=0;
			memcpy(&OffsetAddrtmp,(PUCHAR)(i+3),4);
			return OffsetAddrtmp+7+i;
		}
	}
	return 0;
}

PNOTIFY_INFO EnumCreateProcessNotify()
{
	int i=0;
	ULONG count = 0;
	PNOTIFY_INFO pNotify = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };
	ULONG64	NotifyAddr=0,MagicPtr=0;
	ULONG64	PspCreateProcessNotifyRoutine=FindPspCreateProcessNotifyRoutine();
	//DbgPrint("PspCreateProcessNotifyRoutine: %llx\n",PspCreateProcessNotifyRoutine);

	pNotify = ExAllocatePool(NonPagedPool, sizeof(NOTIFY_INFO) * 100);

	if (pNotify == NULL)
		return NULL;

	if(!PspCreateProcessNotifyRoutine)
		return NULL;
	for(i=0;i<64;i++)
	{
		MagicPtr=PspCreateProcessNotifyRoutine+i*8;
		NotifyAddr=*(PULONG64)(MagicPtr);
		if(MmIsAddressValid((PVOID)NotifyAddr) && NotifyAddr!=0)
		{
			NotifyAddr=*(PULONG64)(NotifyAddr & 0xfffffffffffffff8);

			pNotify[count].CallbacksAddr = NotifyAddr;
			pNotify[count].CallbackType = 1; // creatrprocess
			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(NotifyAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[count].ImgPath, Sysmodule.ImageName, MAX_PATH);
			}

			//DbgPrint("[CreateProcess]%llx\n",NotifyAddr);
			count++;
		}
	}

	pNotify[0].Count = count;
	return pNotify;
}