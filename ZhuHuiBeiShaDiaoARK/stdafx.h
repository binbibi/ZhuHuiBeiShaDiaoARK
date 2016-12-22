
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include <afxdlgs.h>
#include <winioctl.h>
#include <WinTrust.h>
#include <Mscat.h>
#include <SoftPub.h>

#pragma comment(linker,"/DEFAULTLIB:WinTrust.lib")

//0x800b0101 已签名但过期
//0x800b0100 没有签名
#define FILETRUST_EXPIRE 0x800b0101 //文件签名过期
#define FILETRUST_NOSIGN 0x800b0100 //没有签名
#define FILETRUST_SUCCESS 0x88888888


#define IOCTL_TEST	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_TEST2	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_TEST3	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_GETEPROCESS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS) //获取EPROCESS
#define IOCTL_KILLPROCESS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS) //Kill Process by PspTemThread
#define IOVTL_GETSSDTFUCADDR	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS) //get ssdt func addr
#define IOCTL_ENUMHANDLE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS) //Enum Process Handle

#define IOCTL_GetProcessPath	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS) //Get Process Path by selocat

#define IOCTL_ENUMDRIVER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS) //Enum dirver

#define IOCTL_ENUMDIRPISPATCH		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS) //Enum irp DisPatch


extern  HANDLE g_device;
extern  DWORD g_CurSelectPid;
extern  CString g_CurSelectImage;
extern  BOOL	g_bFirsetRun;

#ifndef _DRIVERNAME_
#define _DRIVERNAME_
extern  CString g_DriverName;
#endif

typedef long (*ZWUNMAPVIEWOFSECTION)(HANDLE,PVOID);
extern  ZWUNMAPVIEWOFSECTION ZwUnmapViewOfSection;
extern void CopyToClipboard(HWND hWndOwner, LPCTSTR strText);
extern BOOL NtPathToDosPathW(WCHAR* FullNtPath, WCHAR* FullDosPath);
extern BOOL ARKDeviceIoControl(DWORD dwContryolCode,LPVOID inBuff, DWORD inBuffSize, LPVOID outBuff, DWORD outBuffSize);
extern BOOL GetFileInfo(WCHAR *pszFileFullPath, PWCHAR pszFileCorporation);
extern void LookFileAttrubute(WCHAR* FileFullName);
extern ULONG CheckFileTrust(WCHAR* FileName,BOOL IsCheckAll,int subItem,BOOL IsShowNoSing);

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
extern ZWOPENPROCESS ZwOpenProcess;


typedef long (__stdcall *ZWOPENTHREAD)(
	OUT PHANDLE             ThreadHandle,
	IN ACCESS_MASK          AccessMask,
	IN POBJECT_ATTRIBUTES   ObjectAttributes,
	IN PCLIENT_ID           ClientId );
extern ZWOPENTHREAD ZwOpenThread;

typedef long (__stdcall *ZWQUERYSYSTEMINFORMATION)(
    IN ULONG SystemInformationClass,
    IN OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    IN PULONG ReturnLength OPTIONAL );
extern ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation;

typedef long (__stdcall *ZWQUERYINFORMATIONPROCESS)(
    IN HANDLE ProcessHandle,
    IN ULONG ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL);
extern ZWQUERYINFORMATIONPROCESS ZwQueryInformationProcess;

typedef long (__stdcall *ZWQUERYINFORMATIONTHREAD)(
	__in       HANDLE ThreadHandle,
	__in       ULONG ThreadInformationClass,
	__inout    PVOID ThreadInformation,
	__in       ULONG ThreadInformationLength,
	__out_opt  PULONG ReturnLength);
extern ZWQUERYINFORMATIONTHREAD ZwQueryInformationThread;

typedef long (__stdcall *RTLINITUNICODESTRING)(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString);
extern RTLINITUNICODESTRING RtlInitUnicodeString;

typedef long (*RTLADJUSTPRIVILEGE)(ULONG,ULONG,ULONG,PVOID);
extern RTLADJUSTPRIVILEGE RtlAdjustPrivilege;

typedef BOOL (__stdcall *ENUMPROCESSMODULES)(
  __in   HANDLE hProcess,
  __out  HMODULE *lphModule,
  __in   DWORD cb,
  __out  LPDWORD lpcbNeeded);
extern ENUMPROCESSMODULES EnumProcessModules;

typedef DWORD (__stdcall *GETMODULEFILENAMEEXA)(
  __in      HANDLE hProcess,
  __in_opt  HMODULE hModule,
  __out     LPCSTR lpFilename,
  __in      DWORD nSize);
extern GETMODULEFILENAMEEXA GetModuleFileNameExA;

typedef DWORD (__stdcall *GETMODULEFILENAMEEXW)(
  __in      HANDLE hProcess,
  __in_opt  HMODULE hModule,
  __out     LPWSTR lpFilename,
  __in      DWORD nSize);
extern GETMODULEFILENAMEEXW GetModuleFileNameExW;

typedef long (__stdcall *ZWTERMINATEPROCESS)(
  __in_opt  HANDLE ProcessHandle,
  __in      NTSTATUS ExitStatus);
extern ZWTERMINATEPROCESS ZwTerminateProcess;

typedef long (__stdcall *ZWTERMINATETHREAD)(
	__in_opt  HANDLE ProcessHandle,
	__in      NTSTATUS ExitStatus);
extern ZWTERMINATETHREAD ZwTerminateThread;

typedef long (__stdcall *ZWSUSPENDPROCESS)(
  __in_opt  HANDLE ProcessHandle);
extern ZWSUSPENDPROCESS ZwSuspendProcess;

typedef long (__stdcall *ZWRESUMEPROCESS)(
  __in_opt  HANDLE ProcessHandle);
extern ZWRESUMEPROCESS ZwResumeProcess;

typedef long (__stdcall *ZWSUSPENDTHREAD)(
	__in_opt	HANDLE ProcessHandle,
	__out_opt	PULONG PreviousSuspendCount);

typedef long (__stdcall *ZWRESUMETHREAD)(
	__in_opt	HANDLE ProcessHandle,
	__out_opt	PULONG SuspendCount);

extern ZWSUSPENDTHREAD ZwSuspendThread;

extern ZWRESUMETHREAD ZwResumeThread;

typedef long (*ZWSHUTDOWNSYSTEM)(LONG ShutdownAction);

typedef BOOL (WINAPI *ENUMPROCESSMODULESEX)(
	__in   HANDLE hProcess,
	__out  HMODULE *lphModule,
	__in   DWORD cb,
	__out  LPDWORD lpcbNeeded,
	__in   DWORD dwFilterFlag);

extern ZWSHUTDOWNSYSTEM ZwShutdownSystem;

extern ENUMPROCESSMODULESEX EnumProcessModulesEx;


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



