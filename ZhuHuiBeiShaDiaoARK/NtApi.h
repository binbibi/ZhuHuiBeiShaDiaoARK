#pragma once

#ifndef NTAPI_H_
#define NTAPI_H_

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY
{
    ULONG Unknow1;
    ULONG Unknow2;
    ULONG Unknow3;
	ULONG Unknow4;
    PVOID64 Base;
    ULONG Size;
    ULONG Flags;
    USHORT Index;
    USHORT NameLength;
    USHORT LoadCount;
    USHORT ModuleNameOffset;
    char ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
    ULONG Count;//内核中以加载的模块的个数
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _UNICODE_STRING {
	USHORT Length;			// 字符串的长度（字节数）
	USHORT MaximumLength;	// 字符串缓冲区的长度（字节数）
	PWSTR  Buffer;			// 字符串缓冲区
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _PROCESS_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;
    PVOID PebBaseAddress;
    ULONG_PTR AffinityMask;
    LONG BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION,*PPROCESS_BASIC_INFORMATION;

typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS               ExitStatus;
	PVOID                  TebBaseAddress;
	CLIENT_ID              ClientId;
	KAFFINITY              AffinityMask;
	LONG		           Priority;
	LONG	               BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;


typedef enum _MEMORY_INFORMATION_CLASS
{
   MemoryBasicInformation,
   MemoryWorkingSetList,
   MemorySectionName
}MEMORY_INFORMATION_CLASS;


NTSTATUS ZwQueryVirtualMemory(
  _In_      HANDLE                   ProcessHandle,
  _In_opt_  PVOID                    BaseAddress,
  _In_      MEMORY_INFORMATION_CLASS MemoryInformationClass,
  _Out_     PVOID                    MemoryInformation,
  _In_      SIZE_T                   MemoryInformationLength,
  _Out_opt_ PSIZE_T                  ReturnLength
);


typedef
NTSTATUS
(WINAPI *ZWQUERYVIRTUALMEMORY) (
                         IN HANDLE ProcessHandle,
                         IN PVOID BaseAddress,
                         IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
                         OUT PVOID MemoryInformation,
                         IN SIZE_T MemoryInformationLength,
						 OUT PSIZE_T ReturnLength OPTIONAL
                         );

#endif

typedef long (__stdcall *ZWOPENPROCESS)(
  OUT PHANDLE             ProcessHandle,
  IN ACCESS_MASK          AccessMask,
  IN POBJECT_ATTRIBUTES   ObjectAttributes,
  IN PCLIENT_ID           ClientId );
ZWOPENPROCESS ZwOpenProcess;


typedef long (__stdcall *ZWOPENTHREAD)(
	OUT PHANDLE             ThreadHandle,
	IN ACCESS_MASK          AccessMask,
	IN POBJECT_ATTRIBUTES   ObjectAttributes,
	IN PCLIENT_ID           ClientId );
ZWOPENTHREAD ZwOpenThread;

typedef long (__stdcall *ZWQUERYSYSTEMINFORMATION)(
    IN ULONG SystemInformationClass,
    IN OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    IN PULONG ReturnLength OPTIONAL );
ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation;

typedef long (__stdcall *ZWQUERYINFORMATIONPROCESS)(
    IN HANDLE ProcessHandle,
    IN ULONG ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL);
ZWQUERYINFORMATIONPROCESS ZwQueryInformationProcess;

typedef long (__stdcall *ZWQUERYINFORMATIONTHREAD)(
	__in       HANDLE ThreadHandle,
	__in       ULONG ThreadInformationClass,
	__inout    PVOID ThreadInformation,
	__in       ULONG ThreadInformationLength,
	__out_opt  PULONG ReturnLength);
ZWQUERYINFORMATIONTHREAD ZwQueryInformationThread;

typedef long (__stdcall *RTLINITUNICODESTRING)(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString);
RTLINITUNICODESTRING RtlInitUnicodeString;

typedef long (*RTLADJUSTPRIVILEGE)(ULONG,ULONG,ULONG,PVOID);
RTLADJUSTPRIVILEGE RtlAdjustPrivilege;

typedef BOOL (__stdcall *ENUMPROCESSMODULES)(
  __in   HANDLE hProcess,
  __out  HMODULE *lphModule,
  __in   DWORD cb,
  __out  LPDWORD lpcbNeeded);
ENUMPROCESSMODULES EnumProcessModules;

typedef DWORD (__stdcall *GETMODULEFILENAMEEXA)(
  __in      HANDLE hProcess,
  __in_opt  HMODULE hModule,
  __out     LPCSTR lpFilename,
  __in      DWORD nSize);
GETMODULEFILENAMEEXA GetModuleFileNameExA;

typedef DWORD (__stdcall *GETMODULEFILENAMEEXW)(
  __in      HANDLE hProcess,
  __in_opt  HMODULE hModule,
  __out     LPWSTR lpFilename,
  __in      DWORD nSize);
GETMODULEFILENAMEEXW GetModuleFileNameExW;

typedef long (__stdcall *ZWTERMINATEPROCESS)(
  __in_opt  HANDLE ProcessHandle,
  __in      NTSTATUS ExitStatus);
ZWTERMINATEPROCESS ZwTerminateProcess;

typedef long (__stdcall *ZWTERMINATETHREAD)(
	__in_opt  HANDLE ProcessHandle,
	__in      NTSTATUS ExitStatus);
ZWTERMINATETHREAD ZwTerminateThread;

typedef long (__stdcall *ZWSUSPENDPROCESS)(
  __in_opt  HANDLE ProcessHandle);
ZWSUSPENDPROCESS ZwSuspendProcess;

typedef long (__stdcall *ZWRESUMEPROCESS)(
  __in_opt  HANDLE ProcessHandle);
ZWRESUMEPROCESS ZwResumeProcess;

typedef long (__stdcall *ZWSUSPENDTHREAD)(
	__in_opt	HANDLE ProcessHandle,
	__out_opt	PULONG PreviousSuspendCount);
ZWSUSPENDTHREAD ZwSuspendThread;

typedef long (__stdcall *ZWRESUMETHREAD)(
	__in_opt	HANDLE ProcessHandle,
	__out_opt	PULONG SuspendCount);
ZWRESUMETHREAD ZwResumeThread;

typedef long (*ZWSHUTDOWNSYSTEM)(LONG ShutdownAction);
ZWSHUTDOWNSYSTEM ZwShutdownSystem;

typedef BOOL (WINAPI *ENUMPROCESSMODULESEX)(
	__in   HANDLE hProcess,
	__out  HMODULE *lphModule,
	__in   DWORD cb,
	__out  LPDWORD lpcbNeeded,
	__in   DWORD dwFilterFlag);
ENUMPROCESSMODULESEX EnumProcessModulesEx;