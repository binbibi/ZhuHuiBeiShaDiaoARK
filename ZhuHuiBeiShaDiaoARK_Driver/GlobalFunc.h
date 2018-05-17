#ifndef _GLOBAFUNC
#define _GLOBAFUNC
#include <ntddk.h>

#define MAX_PATH		260
#define kprintf		DbgPrint
#define kmalloc(_s)	ExAllocatePoolWithTag(NonPagedPool, _s, 'SYSQ')
#define kfree(_p)	ExFreePool(_p)

#define SystemModuleInformationClass	11

#ifndef _LDR_
#define _LDR_
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY        InLoadOrderLinks;
	LIST_ENTRY        InMemoryOrderLinks;
	LIST_ENTRY        InInitializationOrderLinks;
	PVOID                        DllBase;
	PVOID                        EntryPoint;
	ULONG                        SizeOfImage;
	UNICODE_STRING        FullDllName;
	UNICODE_STRING         BaseDllName;
	//...剩下的成员省略
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
//
typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32                InLoadOrderLinks;
	LIST_ENTRY32                InMemoryOrderLinks;
	LIST_ENTRY32                InInitializationOrderLinks;
	ULONG                                DllBase;
	ULONG                                EntryPoint;
	ULONG                                SizeOfImage;
	UNICODE_STRING32        FullDllName;
	UNICODE_STRING32         BaseDllName;
	ULONG                                Flags;
	USHORT                                LoadCount;
	USHORT                                TlsIndex;
	//下面的省略
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;
#endif // !_LDR_


typedef struct _HANDLE_INFO{
	ULONG_PTR	ObjectTypeIndex;
	ULONG_PTR	HandleValue;
	ULONG_PTR	ReferenceCount;
	ULONG_PTR	GrantedAccess;
	ULONG_PTR	CountNum;
	ULONG_PTR	Object;
	WCHAR	ProcessName[256*2];
	WCHAR	TypeName[256*2];
	WCHAR	HandleName[256*2];
} HANDLE_INFO, *PHANDLE_INFO;

PHANDLE_INFO pHandleInfo = NULL;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO{
	USHORT	UniqueProcessId;
	USHORT	CreatorBackTraceIndex;
	UCHAR	ObjectTypeIndex;
	UCHAR	HandleAttributes;
	USHORT	HandleValue;
	PVOID	Object;
	ULONG	GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG64 NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef enum _OBJECT_INFORMATION_CLASSEx {
    ObjectBasicInformation1,
    ObjectNameInformation1,
    ObjectTypeInformation1,
    ObjectAllInformation1,
    ObjectDataInformation1
} OBJECT_INFORMATION_CLASSEx, *POBJECT_INFORMATION_CLASSEx;

typedef struct _OBJECT_BASIC_INFORMATION {
  ULONG                   Attributes;
  ACCESS_MASK             DesiredAccess;
  ULONG                   HandleCount;
  ULONG                   ReferenceCount;
  ULONG                   PagedPoolUsage;
  ULONG                   NonPagedPoolUsage;
  ULONG                   Reserved[3];
  ULONG                   NameInformationLength;
  ULONG                   TypeInformationLength;
  ULONG                   SecurityDescriptorLength;
  LARGE_INTEGER           CreationTime;
} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

/*typedef struct _OBJECT_NAME_INFORMATION {
  UNICODE_STRING          Name;
  WCHAR                   NameBuffer[0];
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;*/

typedef struct _OBJECT_TYPE_INFORMATION {
  UNICODE_STRING          TypeName;
  ULONG                   TotalNumberOfHandles;
  ULONG                   TotalNumberOfObjects;
  WCHAR                   Unused1[8];
  ULONG                   HighWaterNumberOfHandles;
  ULONG                   HighWaterNumberOfObjects;
  WCHAR                   Unused2[8];
  ACCESS_MASK             InvalidAttributes;
  GENERIC_MAPPING         GenericMapping;
  ACCESS_MASK             ValidAttributes;
  BOOLEAN                 SecurityRequired;
  BOOLEAN                 MaintainHandleCount;
  USHORT                  MaintainTypeList;
  POOL_TYPE               PoolType;
  ULONG                   DefaultPagedPoolCharge;
  ULONG                   DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct
{
	PVOID section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	char ImageName[MAXIMUM_FILENAME_LENGTH];
}SYSTEM_MODULE,*PSYSTEM_MODULE;

//typedef struct _KAPC_STATE 
//{
//	LIST_ENTRY ApcListHead[2];
//	PVOID Process;
//	BOOLEAN KernelApcInProgress;
//	BOOLEAN KernelApcPending;
//	BOOLEAN UserApcPending;
//}KAPC_STATE, *PKAPC_STATE;

typedef struct _OBJECT_HANDLE_FLAG_INFORMATION{
	BOOLEAN Inherit;
	BOOLEAN ProtectFromClose;
}OBJECT_HANDLE_FLAG_INFORMATION, *POBJECT_HANDLE_FLAG_INFORMATION;


//系统模块结构     SystemModuleInformation, 11
typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY
{
	ULONG Unknow1;
	ULONG Unknow2;
	ULONG Unknow3;
	ULONG Unknow4;
	PVOID Base;  //模块地址指针
	ULONG Size;   //模块大小
	ULONG Flags;
	USHORT Index; //模块索引
	USHORT NameLength;   
	USHORT LoadCount;
	USHORT ModuleNameOffset; //名字的偏移
	char ImageName[256];   //模块全局路径
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;


typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;//内核中以加载的模块的个数 
	SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _SYSTEM_MODULE_INFORMATIONEX
{
	ULONG Count;//内核中以加载的模块的个数 
	SYSTEM_MODULE Module[0];
} SYSTEM_MODULE_INFORMATIONEX, *PSYSTEM_MODULE_INFORMATIONEX;

CHAR	*CallbacksType[8] =
{
	"LoadImage",//0
	"CreateProcess",//1
	"CreateThread",//2
	"CmpRegister",//3
	"ShutDowdn",//4
	"BugCheck",//5
	"BugCheckReason"//6
};
	

/*
回调信息
CallbackType: 见CallbacksType索引
回调入口
回调所在镜像路径
*/
typedef struct _NOTIFY_INFO
{
	ULONG	Count; // 0号索引存放个数
	ULONG	CallbackType;
	ULONG64	CallbacksAddr;
	ULONG64	Cookie; // just work to cmpcallback
	CHAR	ImgPath[MAX_PATH];
}NOTIFY_INFO, *PNOTIFY_INFO;

// Ob 回调信息
typedef struct _OBCALLBACKS_INFO
{
	ULONG	Count; // 0号索引存放个数
	ULONG	ObType;// 0 进程 1 线程
	ULONG64	PreCallbackAddr;
	ULONG64	PostCallbackAddr; 
	ULONG64	ObHandle;
	CHAR	PreImgPath[MAX_PATH];
	CHAR	PostImgPaht[MAX_PATH];
}OBCALLBACKS_INFO, *POBCALLBACKS_INFO;

NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(IN HANDLE ProcessId,OUT PEPROCESS *Process);
NTKERNELAPI NTSTATUS PsLookupThreadByThreadId(IN HANDLE ThreadId,OUT PETHREAD *Thread);
NTKERNELAPI NTSTATUS MmUnmapViewOfSection(IN PEPROCESS Process, IN ULONG BaseAddress); 
NTKERNELAPI UCHAR* PsGetProcessImageFileName(PEPROCESS Process); 
NTKERNELAPI PEPROCESS IoThreadToProcess(IN PETHREAD Thread);
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId( IN PEPROCESS Process);
NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS Process);
NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process(PEPROCESS Process);
NTKERNELAPI VOID NTAPI KeAttachProcess(PEPROCESS Process);
NTKERNELAPI VOID NTAPI KeDetachProcess();

NTKERNELAPI 
NTSTATUS 	
ObSetHandleAttributes (HANDLE Handle, POBJECT_HANDLE_FLAG_INFORMATION HandleFlags, KPROCESSOR_MODE PreviousMode);

NTKERNELAPI 
VOID 
KeStackAttachProcess(PEPROCESS PROCESS, PKAPC_STATE ApcState);

NTKERNELAPI 
VOID 
KeUnstackDetachProcess(PKAPC_STATE ApcState);

//NTSYSAPI
//NTSTATUS
//NTAPI
//ZwQueryObject
//(
//	HANDLE	Handle,
//	ULONG	ObjectInformationClass,
//	PVOID	ObjectInformation,
//	ULONG	ObjectInformationLength,
//	PULONG	ReturnLength OPTIONAL
//);



NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation
(   
	ULONG	SystemInformationClass,
	PVOID	SystemInformation,
	ULONG	SystemInformationLength,
	PULONG	ReturnLength
);

NTSYSAPI
NTSTATUS
NTAPI
ZwDuplicateObject
(
	HANDLE		SourceProcessHandle,
	HANDLE		SourceHandle,
	HANDLE		TargetProcessHandle OPTIONAL,
	PHANDLE 	TargetHandle OPTIONAL,
	ACCESS_MASK	DesiredAccess,
	ULONG		HandleAttributes,
	ULONG		Options
);

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcess
(    
	PHANDLE				ProcessHandle,
	ACCESS_MASK			AccessMask,
	POBJECT_ATTRIBUTES	ObjectAttributes,
	PCLIENT_ID			ClientId
);


typedef NTSTATUS (__fastcall *PSPTERMINATETHREADBYPOINTER)
(
	IN PETHREAD Thread,
	IN NTSTATUS ExitStatus,
	IN BOOLEAN DirectTerminate
);

PSPTERMINATETHREADBYPOINTER PspTerminateThreadByPointer = NULL;


VOID UnicodeStringToCharArray(PUNICODE_STRING dst, char *src);
PVOID GetFunctionAddr(PCWSTR FunctionName);
VOID CharToWchar(PCHAR src, PWCHAR dst);

//相当于GetProAddress
PVOID GetFunctionAddr(PCWSTR FunctionName)
{
    UNICODE_STRING UniCodeFunctionName;
    RtlInitUnicodeString(&UniCodeFunctionName, FunctionName);
    return MmGetSystemRoutineAddress(&UniCodeFunctionName);   
}

VOID UnicodeStringToCharArray(PUNICODE_STRING dst, char *src)
{
	ANSI_STRING string;
	if( dst->Length>260 ) return;
	RtlUnicodeStringToAnsiString(&string,dst, TRUE); 
	strcpy(src,string.Buffer);
	RtlFreeAnsiString(&string); 
}

VOID CharToWchar(PCHAR src, PWCHAR dst)
{
	UNICODE_STRING uString;
	ANSI_STRING aString;
	RtlInitAnsiString(&aString,src);
	RtlAnsiStringToUnicodeString(&uString,&aString,TRUE);
	wcscpy(dst,uString.Buffer);
	RtlFreeUnicodeString(&uString);
}

//根据进程ID返回进程EPROCESS，失败返回NULL
PEPROCESS LookupProcess(HANDLE Pid)
{
	PEPROCESS eprocess=NULL;
	if( NT_SUCCESS(PsLookupProcessByProcessId(Pid, &eprocess)) )
		return eprocess;
	else
		return NULL;
}

NTSTATUS getSystemImageInfoByAddress(ULONG_PTR address,SYSTEM_MODULE* __out lpsystemModule)
{
	NTSTATUS		st;
	ULONG			count;
	ULONG_PTR		sizeOfBuf;
	SYSTEM_MODULE_INFORMATIONEX*	lpsystemModuleInfo;

	if (MmIsAddressValid(lpsystemModule) == FALSE)
	{
		return STATUS_UNSUCCESSFUL;
	}

	if (address == 0)
	{
		return STATUS_UNSUCCESSFUL;
	}

	sizeOfBuf = 0;


	st = ZwQuerySystemInformation(SystemModuleInformationClass,NULL,0,&sizeOfBuf);
	if (st != STATUS_INFO_LENGTH_MISMATCH)
	{
		return st;
	}

	lpsystemModuleInfo = (SYSTEM_MODULE_INFORMATIONEX*)ExAllocatePool(NonPagedPool,sizeOfBuf);
	if (lpsystemModuleInfo == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}

	st = ZwQuerySystemInformation(SystemModuleInformationClass,lpsystemModuleInfo,sizeOfBuf,&sizeOfBuf);
	if (!NT_SUCCESS(st))
	{
		ExFreePool(lpsystemModuleInfo);
		return st;
	}

	st = STATUS_UNSUCCESSFUL;
	for (count = 0;count < lpsystemModuleInfo->Count;count++)
	{
		if (address > (ULONG_PTR)lpsystemModuleInfo->Module[count].ImageBase &&
			address < ((ULONG_PTR)lpsystemModuleInfo->Module[count].ImageBase + lpsystemModuleInfo->Module[count].ImageSize))
		{
			*lpsystemModule = lpsystemModuleInfo->Module[count];
			st = STATUS_SUCCESS;
			break;
		}
	}

	ExFreePool(lpsystemModuleInfo);
	return st;
}



#endif