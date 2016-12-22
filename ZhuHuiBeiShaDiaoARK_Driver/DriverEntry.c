#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <stdlib.h>
#include <ntimage.h>
#include "MyDriver.h"    
#include "Process.h"
#include "SSDT.h"
#include "Driver.h"

extern	PHANDLE_INFO pHandleInfo;

PDRIVER_OBJECT g_DriverObject = NULL;

UNICODE_STRING US;

VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{	
	UNICODE_STRING strLink;
	RtlInitUnicodeString(&strLink, LINK_NAME);
	IoDeleteSymbolicLink(&strLink);
	IoDeleteDevice(pDriverObj->DeviceObject);
	if(pHandleInfo)
	{
		ExFreePool(pHandleInfo);
		pHandleInfo = NULL;
	}
	KdPrint(("[ZhuHuiBeiShaDiAO]:DriverUnload"));
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack;
	ULONG uIoControlCode;
	PVOID pIoBuffer;
	ULONG uInSize;
	ULONG uOutSize;
	ULONGLONG Eprocess = 0;
	//ULONGLONG ssdt_func_addr = 0;
	wchar_t *outbuff = (wchar_t*)ExAllocatePool(NonPagedPool,260*2);//传出W字符
	SSDT_INFO ssdt_addr_info = {0};
	
	ULONG Inbuff;
	UNICODE_STRING UnString;
	WCHAR *Instrbuff = NULL;//(WCHAR*)ExAllocatePool(NonPagedPool,260);//传入W字符

	ANSI_STRING	   AnString;
	CHAR  *InChBuff = (CHAR*)ExAllocatePool(NonPagedPool,260);	

	UNREFERENCED_PARAMETER(pDevObj);
	//
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
	uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	//

	RtlZeroMemory(outbuff, 260*2);
	switch(uIoControlCode)
	{
		case IOCTL_TEST:
		{
			Inbuff = *(ULONG*)pIoBuffer;
			KdPrint(("Inulong:%d",Inbuff));
			RtlInitUnicodeString(&US,L"Driver String For MFC: 我爱北京天安门！");
			wcscpy(outbuff,US.Buffer);
			memcpy(pIoBuffer,outbuff,260);
			status = STATUS_SUCCESS;
			break;
		}
		case IOCTL_TEST2:
		{
			Instrbuff = (WCHAR*)pIoBuffer;
			RtlInitUnicodeString(&UnString,Instrbuff);
			KdPrint(("InUnicodeString:%wZ",&UnString));//%ws
			status = STATUS_SUCCESS;
			break;
		}
		case IOCTL_TEST3:
		{
			InChBuff = (CHAR*)pIoBuffer;
			RtlInitAnsiString(&AnString,InChBuff);
			KdPrint(("InAnsiString:%Z",&AnString));//%s
			status = STATUS_SUCCESS;
			break;
		}
		case IOCTL_GETEPROCESS:
		{
			__try
				{
					Inbuff = *(ULONG*)pIoBuffer;
					//KdPrint(("Input Pid:%d",Inbuff));
					Eprocess = (ULONGLONG)GetEProcessByPid((HANDLE)Inbuff);
					if(Eprocess != 0){
						ObDereferenceObject((PEPROCESS)Eprocess);
						RtlCopyMemory(
							pIrp->AssociatedIrp.SystemBuffer, 
							&Eprocess, 
							sizeof(ULONGLONG)
							);
					}else
					{
						RtlCopyMemory(
							pIrp->AssociatedIrp.SystemBuffer, 
							&Eprocess, 
							sizeof(ULONGLONG)
							);
					}
					status = STATUS_SUCCESS;
				}
			__except(EXCEPTION_EXECUTE_HANDLER)
				{
					KdPrint(("异常"));;
				}
			break;
		}
		case IOCTL_KILLPROCESS:
		{
			__try
				{
					Inbuff = *(ULONG*)pIoBuffer;
					KdPrint(("Kill Process:%d",Inbuff));
					Eprocess = (ULONGLONG)GetEProcessByPid((HANDLE)Inbuff);
					if(Eprocess != 0){
						MyPsTerminateSystemProcess((PEPROCESS)Eprocess);
						ObDereferenceObject((PEPROCESS)Eprocess);
						RtlCopyMemory(
							pIrp->AssociatedIrp.SystemBuffer, 
							&Eprocess, 
							sizeof(ULONGLONG)
							);
					}else
					{
						RtlCopyMemory(
							pIrp->AssociatedIrp.SystemBuffer, 
							&Eprocess, 
							sizeof(ULONGLONG)
							);
					}
					status = STATUS_SUCCESS;
				}
			__except(EXCEPTION_EXECUTE_HANDLER)
				{
					KdPrint(("结束进程出现异常!"));
				}
			break;
		}
		case IOVTL_GETSSDTFUCADDR:
			{
				Inbuff = *(ULONG*)pIoBuffer;
				//DbgPrint("input func index :%d\n",Inbuff);
				//ssdt_func_addr = get_ssdt_func_addr(Inbuff);
				ssdt_addr_info.cut_addr = get_ssdt_func_addr(Inbuff);
				ssdt_addr_info.org_addr = get_ssdt_org_addr(Inbuff);
				//DbgPrint("%p---%p\n",ssdt_addr_info.cut_addr,ssdt_addr_info.org_addr);
				RtlCopyMemory(
					pIrp->AssociatedIrp.SystemBuffer,
					&ssdt_addr_info,
					sizeof(SSDT_INFO))
					;
				status = STATUS_SUCCESS;
				break;
			}
			break;
		case IOCTL_ENUMHANDLE:
			{
				Inbuff = *(ULONG*)pIoBuffer; //pid
				if(EnumProcessHandleWin78((HANDLE)Inbuff) == 0x88888888)
				{
					status = STATUS_UNSUCCESSFUL;
					break;
				}

				if(uOutSize < pHandleInfo[0].CountNum * sizeof(HANDLE_INFO))
				{
					status = STATUS_UNSUCCESSFUL;
					break;
				}
				__try
				{
					KdPrint(("CountNum:%d\n",pHandleInfo[0].CountNum));
					RtlCopyMemory(
						pIrp->AssociatedIrp.SystemBuffer,
						pHandleInfo,
						sizeof(HANDLE_INFO)*1024*2)
						;
					status = STATUS_SUCCESS;
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					status = STATUS_UNSUCCESSFUL;
				}
				break;
			}
			break;
		case IOCTL_GetProcessPath:
			{
				Inbuff = *(ULONG*)pIoBuffer;

				if(uOutSize < 260*2)
				{
					status = STATUS_UNSUCCESSFUL;
					break;
				}

				if(NT_SUCCESS(GetProcessPathByPid((HANDLE)Inbuff,outbuff)))
				{
					memcpy(pIoBuffer,outbuff,260*2);
					status = STATUS_SUCCESS;
				}else
				{
					status = STATUS_UNSUCCESSFUL;
				}
				break;
			}
		case IOCTL_ENUMDRIVER:
			{
				if(uOutSize < sizeof(DRIVER_INFO)*500)
				{
					status = STATUS_BUFFER_TOO_SMALL;
					break;
				}
				EnumDriver(g_DriverObject);
				KdPrint(("IOCTL:%d\n",pDriverInfo[0].Count));
				__try{
					RtlCopyMemory(
						pIrp->AssociatedIrp.SystemBuffer, 
						pDriverInfo, 
						sizeof(DRIVER_INFO)*500);
					status = STATUS_SUCCESS;
					ExFreePool(pDriverInfo);
					pDriverInfo = NULL;
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					status = STATUS_INVALID_PARAMETER;
				}
				break;
			}
		case IOCTL_ENUMDIRPISPATCH:
			{
				if(uOutSize < sizeof(IRP_DISPATCH_INFO) * 100)
				{
					status = STATUS_BUFFER_TOO_SMALL;
					break;
				}

				if(uInSize > MAX_PATH)
				{
					status = STATUS_BUFFER_OVERFLOW;
					break;
				}

				__try{
					RtlCopyMemory(outbuff, pIoBuffer, uInSize);
					KdPrint(("%S\n", outbuff));
					status = EnumDriverMajorFunction(outbuff);
					if(!NT_SUCCESS(status))
					{
						if(status == STATUS_OBJECT_NAME_NOT_FOUND)
							break;
					}

					RtlCopyMemory(
						pIrp->AssociatedIrp.SystemBuffer, 
						pIrpDispatchInfo,
						sizeof(IRP_DISPATCH_INFO) * 100
						);

					status = STATUS_SUCCESS;
					ExFreePool(pIrpDispatchInfo);
					pIrpDispatchInfo = NULL;

				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					status = STATUS_UNSUCCESSFUL;
				}

				break;
			}
			break;
	}
	if(status == STATUS_SUCCESS)
		pIrp->IoStatus.Information = uOutSize;
	else
		pIrp->IoStatus.Information = 0;	
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	ExFreePool(outbuff);

	//ExFreePool(Instrbuff); //释放就蓝屏??? 原因是想释放systembuff
	return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ustrLinkName;
	UNICODE_STRING ustrDevName;  
	PDEVICE_OBJECT pDevObj;

	UNREFERENCED_PARAMETER(pRegistryString);
	g_DriverObject = pDriverObj;
	if(!get_ssdt_info_init())
	{
		DbgPrint("ssdt init faild!\n");
		return STATUS_UNSUCCESSFUL;
	}

	DbgPrint("ssdtbase:0x%llx",(ULONG64)KeServiceDescriptorTable);

	//
	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
	pDriverObj->DriverUnload = DriverUnload;
	
	//
	RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);
	status = IoCreateDevice(pDriverObj, 0, &ustrDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDevObj);
	if(!NT_SUCCESS(status))	return status;
	if(IoIsWdmVersionAvailable(1, 0x10))
		RtlInitUnicodeString(&ustrLinkName, LINK_GLOBAL_NAME);
	else
		RtlInitUnicodeString(&ustrLinkName, LINK_NAME);
	status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);  	

	

	if(!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj); 
		return status;
	}
	KdPrint(("[ZhuHuiBeiShaDiAO]:DriverEntry"));
	//EnumProcessHandleWin78((HANDLE)448);
	//EnumDriverMajorFunction(L"ntfs");
	return STATUS_SUCCESS;
}