#pragma once

//#define Offset_BasePriority 0x1f1
//#define Offset_KernelStack 0x38
//// 这个要自己去找
//#define Offset_ThreadRoutine 0x2d8

ULONG	offset_BasePriority = 0;
ULONG	offset_kernelStack = 0;
ULONG	offset_ThreadRoutine = 0;

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
	"CriticalWorkQueue",//13
	"DelayedWorkQueue",//14
	"HyperCriticalWorkQueue"//15
};

BOOLEAN IsWorkThread(_In_ CHAR* pEThread, _Outptr_ ULONG *WordType)
{
	CHAR tmp = 0;
	__try {
		tmp = *(pEThread + offset_BasePriority);
		if (tmp > 12 && tmp <16)
		{
			*WordType = tmp - 13;
			return TRUE;
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
	CHAR*	  KernelStack = NULL;
	PETHREAD pThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PWORK_THREAD_INFO pWorkInfo = NULL;
	SYSTEM_MODULE SystemModule = { 0 };

	pWorkInfo = ExAllocatePool(NonPagedPool, sizeof(WORK_THREAD_INFO)*300);
	if (pWorkInfo == NULL)
		return NULL;

	RtlZeroMemory(pWorkInfo, sizeof(WORK_THREAD_INFO) * 300);

	for (size_t i = 4; i < 20000; i += 4)
	{
		status = PsLookupThreadByThreadId((HANDLE)i, &pThread);
		if (NT_SUCCESS(status))
		{
			if (IsWorkThread((CHAR*)pThread, &WorkType))
			{
				KernelStack = (CHAR*)(*((ULONG_PTR*)((CHAR*)pThread + offset_kernelStack)));
				if (MmIsAddressValid(KernelStack) && MmIsAddressValid(KernelStack + offset_ThreadRoutine))
				{
					ThreadRoutine = *(ULONG_PTR*)(KernelStack + offset_ThreadRoutine);
				}

				pWorkInfo[Count].ThreadId = i;
				pWorkInfo[Count].eThread = pThread;
				pWorkInfo[Count].WorkType = WorkType; // 已经减去13

				if (ThreadRoutine > MmUserProbeAddress)
				{
					pWorkInfo[Count].ThreadRoutine = ThreadRoutine;

					if (NT_SUCCESS(getSystemImageInfoByAddress(ThreadRoutine, &SystemModule)) && strlen(SystemModule.ImageName) < MAX_PATH)
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