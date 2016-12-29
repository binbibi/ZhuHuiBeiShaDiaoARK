
ULONG64 FindPspLoadImageNotifyRoutine()
{
	ULONG64			i=0,pCheckArea=0;
	UNICODE_STRING	unstrFunc;
	RtlInitUnicodeString(&unstrFunc, L"PsSetLoadImageNotifyRoutine");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress (&unstrFunc);
	//DbgPrint("PsSetLoadImageNotifyRoutine: %llx\n",pCheckArea);
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

PNOTIFY_INFO EnumLoadImageNotify()
{
	int i=0;
	ULONG count = 0;
	PNOTIFY_INFO pNotify = NULL;
	SYSTEM_MODULE Sysmodule = { 0 };
	ULONG64	NotifyAddr=0,MagicPtr=0;
	ULONG64	PspLoadImageNotifyRoutine=FindPspLoadImageNotifyRoutine();
	//DbgPrint("PspLoadImageNotifyRoutine: %llx\n",PspLoadImageNotifyRoutine);
	pNotify = ExAllocatePool(NonPagedPool, sizeof(NOTIFY_INFO) * 100);

	if (pNotify == NULL)
		return NULL;

	if(!PspLoadImageNotifyRoutine)
		return NULL;

	for(i=0;i<8;i++)
	{
		MagicPtr=PspLoadImageNotifyRoutine+i*8;
		NotifyAddr=*(PULONG64)(MagicPtr);
		if(MmIsAddressValid((PVOID)NotifyAddr) && NotifyAddr!=0)
		{
			NotifyAddr=*(PULONG64)(NotifyAddr & 0xfffffffffffffff8);
			pNotify[count].CallbacksAddr = NotifyAddr;
			pNotify[count].CallbackType = 0; // loadimage
			memset(&Sysmodule, 0, sizeof(SYSTEM_MODULE));
			if (NT_SUCCESS(getSystemImageInfoByAddress(NotifyAddr, &Sysmodule)) &&
				strlen(Sysmodule.ImageName) < MAX_PATH)
			{
				RtlCopyMemory(pNotify[count].ImgPath, Sysmodule.ImageName, MAX_PATH);
			}

			//DbgPrint("[LoadImage]%llx\n",NotifyAddr);
			count++;
		}
	}

	pNotify[0].Count = count;
	return pNotify;
}