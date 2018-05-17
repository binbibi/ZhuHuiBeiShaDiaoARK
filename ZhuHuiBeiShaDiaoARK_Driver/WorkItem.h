#pragma once

//#define Offset_BasePriority 0x1f1
//#define Offset_KernelStack 0x38
//// 这个要自己去找
//#define Offset_ThreadRoutine 0x2d8

ULONG	offset_BasePriority = 0;
ULONG	offset_kernelStack = 0;
ULONG   offset_InitialStack = 0;
ULONG   offset_Queue = 0;
ULONG	offset_ThreadRoutine = 0;
ULONG   Offset_KTHREAD_MiscFlags = 0;

typedef struct  _KTHREAD_MiscFlags
{
	ULONG KernelStackResident : 1;
	ULONG ReadyTransition : 1;
	ULONG ProcessReadyQueue : 1;
	ULONG WaitNext : 1;
	ULONG SystemAffinityActive : 1;
	ULONG Alertable : 1;
	ULONG GdiFlushActive : 1;
	ULONG Reserved : 25;

}KTHREAD_MiscFlags, *pKTHREAD_MiscFlags;

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationThread(
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	OUT PVOID ThreadInformation,
	IN ULONG ThreadInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);


typedef struct _WORK_THRAD_INFO
{
	ULONG	Count;
	ULONG	ThreadId;
	ULONG	WorkType;
	ULONG64	eThread;
	ULONG64	ThreadRoutine;
	CHAR	ImgPath[MAX_PATH];
}WORK_THREAD_INFO, *PWORK_THREAD_INFO;

static char *WORD_TYPE[4] =
{
	"DelayedWorkQueue", //12
	"CriticalWorkQueue",//13
	"UnKownWorkQueue",//14
	"HyperCriticalWorkQueue"//15
};

BOOLEAN IsWorkThread(_In_ BYTE* pEThread, _Outptr_ ULONG *WordType)
{
	CHAR tmp = 0;
	if (PsIsThreadTerminating((PETHREAD)pEThread))
	{
		return FALSE;
	}

	if (!PsIsSystemThread((PETHREAD)pEThread))
	{
		return FALSE;
	}
	
	__try {
		tmp = *(pEThread + offset_BasePriority);
		if (tmp > 11 && tmp <16)
		{
			*WordType = tmp - 12;

			if (*((PULONG_PTR)(pEThread+ offset_Queue)) != 0x0)
			{
				return TRUE;
			}
			
		}
	}
	__except (1) {

	}

	return FALSE;
}

PWORK_THREAD_INFO EnumWorkThread()
{
	ULONG WorkType = 0;
	ULONG Count = 0;
	ULONG_PTR ThreadRoutine = 0;
	ULONG_PTR WorkerRoutine = 0;
	pKTHREAD_MiscFlags pkthread_miscflags = NULL;
	ULONG_PTR*	   KernelStack = NULL;
	ULONG_PTR*     KernelInitialStack = NULL;
	PETHREAD pThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PWORK_THREAD_INFO pWorkInfo = NULL;
	SYSTEM_MODULE SystemModule = { 0 };
	ULONG_PTR nCount = 0;
	ULONG_PTR j = 0;

	pWorkInfo = ExAllocatePool(NonPagedPool, sizeof(WORK_THREAD_INFO)*300);
	if (pWorkInfo == NULL)
		return NULL;

	RtlZeroMemory(pWorkInfo, sizeof(WORK_THREAD_INFO) * 300);

	for (size_t i = 4; i < 20000; i += 4)
	{
		status = PsLookupThreadByThreadId((HANDLE)i, &pThread);
		if (NT_SUCCESS(status))
		{
			if (IsWorkThread((BYTE*)pThread, &WorkType))
			{
				
				KernelStack = (ULONG_PTR*)(*((PULONG_PTR)((CHAR*)pThread + offset_kernelStack)));
				KernelInitialStack = (ULONG_PTR*)(*((ULONG_PTR*)((CHAR*)pThread + offset_InitialStack)));
				ThreadRoutine = (ULONG_PTR)(*((ULONG_PTR*)((CHAR*)pThread + offset_ThreadRoutine)));
				pkthread_miscflags =  (PBYTE)pThread + Offset_KTHREAD_MiscFlags;
				
				if (pkthread_miscflags->KernelStackResident)
				{
					if (KernelStack > MmSystemRangeStart && KernelInitialStack > MmSystemRangeStart && ThreadRoutine > MmSystemRangeStart)
					{
						nCount = ((ULONG_PTR)KernelInitialStack - (ULONG_PTR)KernelStack) / sizeof(ULONG_PTR);

						for (j = 0; j < nCount; j++)
						{
							if ((ThreadRoutine < *(KernelStack + j)) && (*(KernelStack + j) < ThreadRoutine + 0x100))
							{
								if (j > 6)
								{
									WorkerRoutine = *(KernelStack + j - 6);
								}
							}
						}

					}
				}
				
				if (WorkerRoutine < (ULONG_PTR)MmSystemRangeStart)
				{
					WorkerRoutine = 0;
				}

	
				pWorkInfo[Count].ThreadId = (ULONG)i;
				pWorkInfo[Count].eThread = (ULONG_PTR)pThread;
				pWorkInfo[Count].WorkType = WorkType; // 已经减去12

				if (WorkerRoutine)
				{
					pWorkInfo[Count].ThreadRoutine = WorkerRoutine;

					if (NT_SUCCESS(getSystemImageInfoByAddress(WorkerRoutine, &SystemModule)) && strlen(SystemModule.ImageName) < MAX_PATH)
					{
						// 惊现BUG !!!
						RtlCopyMemory(pWorkInfo[Count].ImgPath, SystemModule.ImageName, MAX_PATH);
					}

				}	
				else
				{
					pWorkInfo[Count].ThreadRoutine = 0;
					RtlZeroMemory(pWorkInfo[Count].ImgPath, MAX_PATH);
				}
					
				RtlZeroMemory(&SystemModule, sizeof(SYSTEM_MODULE));
				

				/*DbgPrint("\r\n");

				DbgPrint(
					"Thread:0x%p\r\n"
					"WorkType:%s\r\n"
					"ThreadRoute:0x%p\r\n"
					"ImageName:%s\r\n"
					"KernelStack:0x%p\r\n"
					, pThread
					, WORD_TYPE[WorkType]
					, ThreadRoutine
					, PsGetProcessImageFileName(IoThreadToProcess(pThread))
					, KernelStack
				);*/

				Count++;

			}
			ObDereferenceObject(pThread);
		}
	}

	pWorkInfo[0].Count = Count;
	return pWorkInfo;
}