

#include <ntddk.h>
#include <intrin.h>

#ifndef _NTIAMGE_
#define _NTIAMGE_
#include <ntimage.h>
#endif // !_NTIAMGE_

#include <ntimage.h>
#include "GlobalFunc.h"

typedef struct _SYSTEM_SERVICE_TABLE{
	PVOID  		ServiceTableBase; 
	PVOID  		ServiceCounterTableBase; 
	ULONGLONG  	NumberOfServices; 
	PVOID  		ParamTableBase; 
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;

PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;

PSYSTEM_SERVICE_TABLE KeServiceDescriptorTableShadow = 0;
ULONG64	ul64W32pServiceTable = 0;
PEPROCESS g_guiProcess = NULL;

typedef struct _SSDT_INFO
{
	ULONG64 cut_addr;
	ULONG64 org_addr;
	CHAR	imgPath[MAX_PATH];
}SSDT_INFO,*PSSDT_INFO;

typedef struct _SSSDT_INFO
{
	ULONG64 Address;
	CHAR	ImgPath[MAX_PATH];
}SSSDT_INFO, *PSSSDT_INFO;

#pragma intrinsic(__readmsr)
ULONGLONG MyGetKeServiceDescriptorTable64();


//NTSYSAPI NTSTATUS   ZwQuerySystemInformation
//(
//IN ULONG	SystemInformationClass,
//OUT PVOID	SystemInformation,
//IN ULONG	Length,
//OUT PULONG	ReturnLength
//);



NTSYSAPI PIMAGE_NT_HEADERS   RtlImageNtHeader( PVOID Base);


//��ȡpe�ṹ������Ŀ¼��ṹ
NTSYSAPI PVOID  RtlImageDirectoryEntryToData(
	PVOID Base, //ģ�����ַ
	BOOLEAN MappedAsImage,//�Ƿ�ӳ��Ϊӳ��
	USHORT Directory, //����Ŀ¼�������
	 OUT PULONG Size); //��Ӧ����Ŀ¼��Ĵ�С






static ULONG_PTR MyZwQuerySystemInformation(IN ULONG Number);
static ULONG_PTR MyModuleQuery(char* MudolePath);
static ULONG_PTR MyZwQuerySystemInformation(IN ULONG Number);
static ULONG_PTR QueryFunction(PWCHAR name);
static ULONGLONG MyGetKeServiceDescriptorTable64();
static PULONG MyPeLoad(IN char* cStr, IN WCHAR *wcSrt);
static ULONG  MyRelocationTable(PVOID  NewImage, PVOID RawImage);
static ULONG MySSDTRelocation(IN ULONG_PTR NewAddress, IN ULONG_PTR RawAddress);
static KIRQL WPOFFx64(); //�ر�ҳ�汣��
static void WPONx64(KIRQL irql);//�ָ�ҳ�汣��
/*
��ȡSSDT��ȷƫ��,�ɹ�������;
*/
static BOOLEAN Relocation(); //��ȡSSDTƫ����Ϣ
static ULONG FuncOffset(ULONG64 FuncAdd); //����ƫ��

#define  MyDbgPrint  DbgPrint 
#define _max(a,b)  a>b?a:b //����abֵ�Ǹ������ĸ�
ULONG gu_SSDT[0x300] = { NULL };
ULONG64 gu64_Raw_SSDT[0x200] = { NULL };
ULONG COUNT=0;
BOOLEAN FLAG = FALSE;
ULONG64 off = NULL;


KIRQL WPOFFx64()
{
	/*
	����IRQL�ȼ���Dispatch_Level
	�ر�ҳ�汣��,CR0�Ĵ�����ֵ16λ��0  0~16
	Ӳ���жϱ�־λ��0 ,����Ӧ�������ж�
	ʵ����,���̰߳�ȫ��   _disableΪӲ���жϲ���Ӧ
	Dispatch_LevelΪ�̲߳����л����ж�
	*/
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	UINT64 cr0 = __readcr0();
	_disable();
	cr0 &= 0xfffffffffffeffff;
	__writecr0(cr0);
	return irql;
}

void WPONx64(KIRQL irql)
{
	/*
	�ָ�ҳ�汣��. CRO�Ĵ���ֵ16λֵ1  0~16
	IF�жϱ�־��1  ������Ӧ�������ж�
	�������жϼ���
	*/
	UINT64 cr0 = __readcr0();
	cr0 |= 0x10000;
	__writecr0(cr0);
	_enable();
	KeLowerIrql(irql);
}


int get_ssdt_info_init()
{
	KeServiceDescriptorTable = (PSYSTEM_SERVICE_TABLE)MyGetKeServiceDescriptorTable64();
	if(KeServiceDescriptorTable != NULL && Relocation())
	{
		KdPrint(("ssdtbase:0x%llx",(ULONG64)KeServiceDescriptorTable));		
		return 1;
	}else
	{
		KdPrint(("ssdt base is null!\n"));
		return 0;
	}

	/*if(Relocation())
		return 1;
	else
		return 0;*/
}

//ULONGLONG GetSSDTBaseX64()
//{
//	char KiSystemServiceStart_pattern[13] = "\x8B\xF8\xC1\xEF\x07\x83\xE7\x20\x25\xFF\x0F\x00\x00";	//���е�������
//	ULONGLONG CodeScanStart = (ULONGLONG)&_strnicmp;
//	ULONGLONG CodeScanEnd = (ULONGLONG)&KdDebuggerNotPresent;
//	ULONGLONG i, tbl_address, b;
//	for (i = 0; i < CodeScanEnd - CodeScanStart; i++)
//	{
//		if (!memcmp((char*)(ULONGLONG)CodeScanStart +i, (char*)KiSystemServiceStart_pattern,13))
//		{ 
//			for (b = 0; b < 50; b++)
//			{
//				tbl_address = ((ULONGLONG)CodeScanStart+i+b);
//				if (*(USHORT*) ((ULONGLONG)tbl_address ) == (USHORT)0x8d4c)
//					return ((LONGLONG)tbl_address +7) + *(LONG*)(tbl_address +3);
//			}
//		}
//	}
//	return 0;
//}

ULONGLONG MyGetKeServiceDescriptorTable64() 
{
	PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
    PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
	PUCHAR i = NULL;
	UCHAR b1=0,b2=0,b3=0;
	ULONG templong=0;
	ULONGLONG addr=0;
	for(i=StartSearchAddress;i<EndSearchAddress;i++)
	{
		if( MmIsAddressValid(i) && MmIsAddressValid(i+1) && MmIsAddressValid(i+2) )
		{
			b1=*i;
			b2=*(i+1);
			b3=*(i+2);
			if( b1==0x4c && b2==0x8d && b3==0x15 ) //4c8d15
			{
				memcpy(&templong,i+3,4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}


/*
	һ���ָ�SSDT������HOOK
*/
BOOLEAN UnhookSSDT()
{

	PSYSTEM_SERVICE_TABLE ssdt = NULL;
	ULONG i = 0;
	ULONG judge;//�ж�
	KIRQL irql;

	if (FLAG == FALSE)
	{
		BOOLEAN  Boo;
		Boo = Relocation();
		if (!Boo)
		{
			MyDbgPrint("UnhookSSDT()->if (!Boo),");
			return FALSE;
		}
		FLAG = TRUE;
	}
	
	if (gu_SSDT[0]==NULL)
	{
		MyDbgPrint("UnhookSSDT()->ȫ�����������Ч");
		return FALSE;
	}
	ssdt = (PSYSTEM_SERVICE_TABLE)MyGetKeServiceDescriptorTable64();
	if (ssdt==NULL)
	{
		MyDbgPrint("UnhookSSDT()->if (ssdt==NULL)");
		return FALSE;
	}
	
	for (i = 0; i < ssdt->NumberOfServices;i++)
	{
		judge = *(ULONG*)((ULONG64)ssdt->ServiceTableBase + i * 4) >> 4;
		if (judge!= gu_SSDT[i]>>4)
		{
			irql=WPOFFx64();
			*(ULONG*)((ULONG64)ssdt->ServiceTableBase + i * 4) = gu_SSDT[i];
			WPONx64(irql);
			//MyDbgPrint("��HOOK��SSDT���к�=%d,��ȷ�ĺ�����ַ%llx:\n", i, GetSSDTFunctionAddress64_2(i));
		}

	}
	
	MyDbgPrint("UnhookSSDT()->SSDT�ָ����");
	return TRUE;

}

/*
	��ȡSSDT��ȷƫ��,�ɹ�������;
*/
BOOLEAN Relocation()
{
	ULONG a = 0;
	char module_path[260] = { NULL }; //�ں�·��
	ULONG_PTR base=NULL;//�ں˵�ַ
	PULONG  new_module_pointer; //���ں�
	base=MyModuleQuery(module_path);
	if (base==NULL)
	{
		MyDbgPrint("Relocation()->��ȡģ���ַʧ��\n");
		return FALSE;
	}
	new_module_pointer = MyPeLoad(module_path, NULL);
	if (new_module_pointer==0)
	{
		MyDbgPrint("Relocation()->new_module_pointer==0)\n");
		return FALSE;
	}
	MyDbgPrint("�µ�ַ%p\n", new_module_pointer);
	

	a = MyRelocationTable(new_module_pointer, (PVOID)base);
	 if (a==0)
	 {
		 MyDbgPrint("Relocation()->����ַ�ض�λ����ʧ��\n");
		 ExFreePool(new_module_pointer);
		 return FALSE;
	 }
	a= MySSDTRelocation((ULONG_PTR)new_module_pointer, base);
	if (a == 0)
	{
		MyDbgPrint("Relocation()->SSDT�ض�λʧ��\n");
		ExFreePool(new_module_pointer);
		return FALSE;
	}
	if (gu_SSDT[0]==NULL)
	{
		MyDbgPrint("Relocation()->��ȡSSDTƫ��ʧ��\n");
		ExFreePool(new_module_pointer);
		return FALSE;
	}
	if (gu64_Raw_SSDT[0] == NULL)
	{
		MyDbgPrint("Relocation()->��ȡSSDT��ʵ��ַʧ��\n");
		ExFreePool(new_module_pointer);
		return FALSE;
	}
	ExFreePool(new_module_pointer);
	return TRUE;
}

/*
   ��ȡ�ں�ģ�� ·������ַ
*/
ULONG_PTR MyModuleQuery(OUT char* ModulePath )
{
	PSYSTEM_MODULE_INFORMATION  module = NULL;
	ULONG_PTR Base;
	module = (PSYSTEM_MODULE_INFORMATION)MyZwQuerySystemInformation(11);
	if (module==NULL)
	{
		MyDbgPrint("MyModuleQuery(),��ȡģ�����\n\r");
		return 0;
	}

	MyDbgPrint("ģ��·��:%s\n", module->Module->ImageName);
	MyDbgPrint("%s:%p\n", module->Module->ImageName + module->Module->ModuleNameOffset, module->Module->Base);

	memcpy(ModulePath, module->Module->ImageName, strlen(module->Module->ImageName));
	Base = (ULONG_PTR)module->Module->Base;
	ExFreePool(module);
	return Base;
}

ULONG_PTR MyZwQuerySystemInformation(IN ULONG Number)
{
	VOID * Buffer;
	NTSTATUS status;
	ULONG   buffer1 = 20;

	Buffer = ExAllocatePool(NonPagedPool, buffer1);
	//����ʵ����Ҫ�Ļ�������С����buffer1��
	status = ZwQuerySystemInformation(Number, Buffer, buffer1, &buffer1);
	ExFreePool(Buffer);
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		//����һ���޷�ҳ������NonPagedPool
		Buffer = ExAllocatePool(NonPagedPool, buffer1);
		if (!MmIsAddressValid(Buffer))
		{
			MyDbgPrint("GetSystemIniformation(),if (!MmIsAddressValid(Buffer)),Error\r\n");
			return 0;
		}
		memset(Buffer, 0, sizeof(Buffer));
		status = ZwQuerySystemInformation(Number, Buffer, buffer1, &buffer1);
		if (!NT_SUCCESS(status))
		{
			MyDbgPrint("����״̬=%d\r\n", RtlNtStatusToDosError(status));
			//���仺���������ʧ�ܾͱ���ѷ�����ڴ��ͷŵ�
			ExFreePool(Buffer);
			MyDbgPrint("GetSystemIniformation(),if (!NT_SUCCESS(status)),Error\r\n");
			return 0;
		}
		return (ULONG_PTR)Buffer;
	}
	MyDbgPrint("GetSystemIniformation(),����״̬=%d\r\n", RtlNtStatusToDosError(status));
	return 0;
}

/*
	���뵼���������� L""
	��ȡ������ַ
*/
ULONG_PTR QueryFunction(PWCHAR name)
{
	UNICODE_STRING   na;
	ULONG_PTR add;
	RtlInitUnicodeString(&na, name);
	add = (ULONG_PTR)MmGetSystemRoutineAddress(&na);
	return add;
}

/*
	����PE���ڴ�
	����ascii �ַ���ʽ�ļ�·��,������unicode �ַ���ʽ�ļ�·��
	ֻ�ܶ�ѡһ,����һ��Ϊ0;
*/
PULONG MyPeLoad(IN char* cStr, IN WCHAR *wcSrt)
{
	/*
	��ȡpe�ļ����ڴ������,   (���ȴ�Ӳ�̶�ȡ�ļ�,Ȼ����ص��ڴ�)
	1,��ͨ��һ��·����һ���ļ�,
	2,Ȼ��ʼ��ȡPE�ļ��ĸ�����Ϣ
	3,Ȼ��ӻ�õ���Ϣ�л�ȡ�ļ���Ҫ�Ĵ�С�� ����ͬ���Ĵ�С�Ļ�����
	4,Ȼ�������ΰ��չ涨,�ڰ��ļ���DOSͷ ��ģ�����ζ�ȡ���շ���Ļ������ڴ���
	*/

	HANDLE    hfile;     //���ܾ��
	NTSTATUS  status;    //״̬
	PVOID sizeof_image=NULL;  //�����ڴ��ĵ�ַָ��
	IO_STATUS_BLOCK      io_status_block;    //����״̬�ṹ
	OBJECT_ATTRIBUTES    object_attributes;  //�������
	UNICODE_STRING       path_name;       
	ANSI_STRING  ansi_path;
	ULONG flag = 0;//��־
	ULONG Characteristics = 0;

	IMAGE_DOS_HEADER	image_dos_header;//dosͷ�ṹ
	LARGE_INTEGER       large_integer;//��¼ƫ��
	

	IMAGE_NT_HEADERS64   image_nt_header;//NTͷ

	IMAGE_SECTION_HEADER * p_image_section_header;//ָ��������ṹ

	ULONG sizeof_raw_data;

	ULONG i = 0;
	/*
		�жϴ���Ĳ������ʼ���ַ���
	*/
	if (cStr != NULL&&wcSrt != NULL)
	{
		MyDbgPrint("������һ��·��,MyPeLoad()");
		return 0;
	}
	if (cStr == NULL&&wcSrt != NULL)
	{
		RtlInitUnicodeString(&path_name, wcSrt);
	}
	else if (cStr != NULL&&wcSrt == NULL)
	{
		RtlInitAnsiString(&ansi_path, cStr);
		status = RtlAnsiStringToUnicodeString(&path_name, &ansi_path, TRUE);
		if (!NT_SUCCESS(status))
		{
			MyDbgPrint("MyLoadModule(),(cStr != NULL&&wcSrt == NULL)����=%d\n", RtlNtStatusToDosError(status));
			return 0;
		}
		flag = 1;
	}
	else
	{
		MyDbgPrint("���������Ч,MyLoadModule,else\n");
		return 0;
	}




	/*
	    ��ʼ����������
	    ZwCreateFile�����ز�ֱ�ӽ����ַ���ֻ����һ��OBJECT_ATTRIBUTES��һ���ṹ
	    ��������Ҫ��InitializeObjectAttributes��������ʼ������ṹ 
		Ȼ���ڵ��ɲ�������ZwCreateFile
	*/
	InitializeObjectAttributes(&object_attributes, //�������Ա��� POBJECT_ATTRIBUTES OUT  
		&path_name,								   //�ļ���   PUNICODE_STRING
		OBJ_CASE_INSENSITIVE,                      //��ʾ�����ִ�Сд
		NULL,                                      //NULL
		NULL);                                     //NULL
	MyDbgPrint("OBJECT_ATTRIBUTES����֤·��:%wZ\n", object_attributes.ObjectName);
	/*
		���ļ���,��ʼ��ȡ���ڴ�
	*/
	status = ZwCreateFile(
		&hfile,                  //���صľ��  OUT PHANDLE
		FILE_ALL_ACCESS,         //����Ȩ��->����Ȩ��
		&object_attributes,      //POBJECT_ATTRIBUTES �ýṹ����Ҫ�򿪵��ļ���
		&io_status_block,        //PIO_STATUS_BLOCK ���ؽ��״̬ OUT
		0,                       //��ʼ�����С,0�Ƕ�̬����
		FILE_ATTRIBUTE_NORMAL,   //�ļ����� һ��Ϊ<-����0;
		FILE_SHARE_READ,         //ָ������ʽһ��<- ����0;
		FILE_OPEN,               //�������ָ��Ҫ���ļ�����
		FILE_NON_DIRECTORY_FILE, //ָ�����ƴ򿪲����;��ʹ�õĸ��ӱ�־λ
		NULL,                    //ָ���ѡ����չ������
		0);                      //��չ�������ĳ���
	if (!NT_SUCCESS(status))
	{
		MyDbgPrint("MyLoadModule(),status = ZwCreateFile����=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	/*
	InitializeObjectAttributes��ʼ����������,�õ�ȫ��·�����ֻ��Ǹ�ָ��
	����ͷ��ַ���������̫��Ļ��ͻ���·������Ϊ�ն����ļ�ʧ��	
	*/
	if (flag == 1) //��ʾ����������ansi����Ҫ��ת��ʱ����Ļ������ͷŵ�
	{
		RtlFreeUnicodeString(&path_name);
	}

	//��ȡDOSͷ******
	
	large_integer.QuadPart = 0;
	status = ZwReadFile(hfile,       //ZwCreateFile�ɹ���õ��ľ��  
		NULL,                        //һ���¼�  NULL
		NULL,                        //�ص����̡�NULL
		NULL,                        //NULL
		&io_status_block,            //PIO_STATUS_BLOCK ���ؽ��״̬ OUT ,ͬ��
		&image_dos_header,           //��Ŷ�ȡ���ݵĻ����� OUT PVOID  
		sizeof(IMAGE_DOS_HEADER),    //��ͼ��ȡ�ļ��ĳ���
		&large_integer,              //Ҫ��ȡ��������ļ���ƫ����PLARGE_INTEGER
		0);                          //NULL
	if (!NT_SUCCESS(status))
	{
		ZwClose(hfile);
		MyDbgPrint("MyLoadModule(),status = ZwCreateFile����=%d\n", RtlNtStatusToDosError(status));
		return 0;
		
	}
	//��ȡNTͷ*******
	
	large_integer.QuadPart = image_dos_header.e_lfanew; //PEͷƫ��
	status = ZwReadFile(hfile,       //ZwCreateFile�ɹ���õ��ľ��  
		NULL,                        //һ���¼�  NULL
		NULL,                        //�ص����̡�NULL
		NULL,                        //NULL
		&io_status_block,            //PIO_STATUS_BLOCK ���ؽ��״̬ OUT ,ͬ��
		&image_nt_header,           //��Ŷ�ȡ���ݵĻ����� OUT PVOID  
		sizeof(IMAGE_NT_HEADERS64),    //��ͼ��ȡ�ļ��ĳ���
		&large_integer,              //Ҫ��ȡ��������ļ���ƫ����PLARGE_INTEGER
		0);                          //NULL
	if (!NT_SUCCESS(status))
	{
		ZwClose(hfile);
		MyDbgPrint("MyLoadModule(),status = ZwCreateFile����=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	//��ȡ����*****
	
	//��������ģ���ܴ�С
	p_image_section_header = (IMAGE_SECTION_HEADER*)ExAllocatePool(NonPagedPool,        //NonPagedPool  �ӷǷ�ҳ�ڴ���з����ڴ� 
		sizeof(IMAGE_SECTION_HEADER)*image_nt_header.FileHeader.NumberOfSections);
	memset(p_image_section_header, 0, sizeof(p_image_section_header));
	//��
	large_integer.QuadPart += sizeof(IMAGE_NT_HEADERS64); //����ƫ��
	status = ZwReadFile(hfile,       //ZwCreateFile�ɹ���õ��ľ��  
		NULL,                        //һ���¼�  NULL
		NULL,                        //�ص����̡�NULL
		NULL,                        //NULL
		&io_status_block,            //PIO_STATUS_BLOCK ���ؽ��״̬ OUT ,ͬ��
		p_image_section_header,           //��Ŷ�ȡ���ݵĻ����� OUT PVOID  
		sizeof(IMAGE_SECTION_HEADER)*image_nt_header.FileHeader.NumberOfSections,    //��ͼ��ȡ�ļ��ĳ���
		&large_integer,              //Ҫ��ȡ��������ļ���ƫ����PLARGE_INTEGER
		0);                          //NULL
	if (!NT_SUCCESS(status))
	{
		ExFreePool(p_image_section_header);
		ZwClose(hfile);
		MyDbgPrint("MyLoadModule(),large_integer.QuadPart +=����=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	sizeof_image = ExAllocatePool(NonPagedPool, image_nt_header.OptionalHeader.SizeOfImage);//NonPagedPool  �ӷǷ�ҳ�ڴ���з����ڴ� 
	if (sizeof_image == 0)
	{
		ZwClose(hfile);
		KdPrint(("sizeof_image ExAllocatePool Failed!"));
		ExFreePool(p_image_section_header);  //�ͷ��ڴ�
		MyDbgPrint("MyLoadModule(),sizeof_image ExAllocatePool=����=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	//��ʼ�����ڴ�
	memset(sizeof_image, 0, image_nt_header.OptionalHeader.SizeOfImage);
	RtlCopyMemory(sizeof_image, &image_dos_header, sizeof(IMAGE_DOS_HEADER));        //dosͷ
	RtlCopyMemory((PVOID)((ULONG_PTR)sizeof_image + image_dos_header.e_lfanew),
		&image_nt_header, sizeof(IMAGE_NT_HEADERS));                                 //ntͷ
	RtlCopyMemory((PVOID)((ULONG_PTR)sizeof_image + image_dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS)),       //����
		p_image_section_header, sizeof(IMAGE_SECTION_HEADER)*image_nt_header.FileHeader.NumberOfSections);//���������ܴ�С
	//��ȡ�������ݶε�ʵ�ʵ�ַ
	
	for (i = 0; i < image_nt_header.FileHeader.NumberOfSections; i++)
	{   //����ռ�õĴ�Сѡ������
		sizeof_raw_data = _max(p_image_section_header[i].Misc.VirtualSize, p_image_section_header[i].SizeOfRawData);
		large_integer.QuadPart = p_image_section_header[i].PointerToRawData;   //�������̵�ƫ�Ƶ�ַ

		// ����� ��ִ�� �ɶ�
		Characteristics = p_image_section_header[i].Characteristics;
		if ((Characteristics & 0x60000020) == 0x60000020)
		{
			// �������ڱ���Ҫɨ�蹳�ӵĽ�
			DbgPrint("[CodeSection]%s\n", p_image_section_header[i].Name);
		}
		//��
		status = ZwReadFile(hfile,       //ZwCreateFile�ɹ���õ��ľ��  
			NULL,                        //һ���¼�  NULL
			NULL,                        //�ص����̡�NULL
			NULL,                        //NULL
			&io_status_block,            //PIO_STATUS_BLOCK ���ؽ��״̬ OUT ,ͬ��
			(PVOID)((ULONG_PTR)sizeof_image + p_image_section_header[i].VirtualAddress), //����װ���ڴ�������ļ�ͷƫ����          
			sizeof_raw_data,             //��ͼ��ȡ�ļ��ĳ���
			&large_integer,              //Ҫ��ȡ��������ļ���ƫ����PLARGE_INTEGER
			0);                          //NULL
		if (!NT_SUCCESS(status))
		{
			MyDbgPrint("ѭ���������[%s]%x\n",
				p_image_section_header[i].Name,
				(ULONG_PTR)sizeof_image + p_image_section_header[i].VirtualAddress);

			ExFreePool(sizeof_image);
			ExFreePool(p_image_section_header);  //�ͷ��ڴ�
			ZwClose(hfile);
			MyDbgPrint("MyLoadModule(),����=%d\n", RtlNtStatusToDosError(status));
			return 0;
		}
	}
	ExFreePool(p_image_section_header);  //�ͷ��ڴ�
	ZwClose(hfile);
	return (PULONG)sizeof_image;
}

/*
	ģ��ȫ�ֱ�����ַ�ض�λ��
*/
ULONG  MyRelocationTable(PVOID  NewImage,PVOID RawImage)
{

	//_IMAGE_OPTIONAL_HEADER64
	ULONG					i;                       //forѭ������
	ULONG					uRelocTableSize = 0;     //������ݿ��е������ܸ���
	ULONG					Type;                    //16λ���ݸ�4λ
	PVOID 					uRelocAddress = 0;       //ָ����Ҫ�޸����ݵĵ�ַ
	PIMAGE_BASE_RELOCATION	pImageBaseRelocation = 0;//�ض�λ��
	ULONG_PTR size;
	ULONG_PTR  offset;//ƫ��
	/*
		RtlImageNtHeader����ֱ�ӻ�ȡģ���NTͷ
	*/
	PIMAGE_NT_HEADERS64 nt_header=NULL;
	nt_header = RtlImageNtHeader(RawImage);
	if (!nt_header)
	{
		MyDbgPrint("MyRelocationTable()->if(!nt_header)����\n");
		return 0;
	}
	//MyDbgPrint("����ģ��ImageBase��ַ%p", nt_header->OptionalHeader.ImageBase);
	MyDbgPrint("��ģ��ImageBase��ַ%p\n", nt_header->OptionalHeader.ImageBase);
	/*
		���������õ�ԭʼ�ں˵�ƫ��,Ŀ�ľ��Ƕ�λ��ԭʼ�ں��������
	*/
	offset = (ULONG_PTR)RawImage - nt_header->OptionalHeader.ImageBase;
	/*
	     RtlImageDirectoryEntryToData����
		 ���Ի�ȡ����Ŀ¼���е�����ṹ
		 ����IMAGE_DIRECTORY_ENTRY_BASERELOC ==5 �ض�λ��
	*/
	pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)RtlImageDirectoryEntryToData(NewImage, TRUE, IMAGE_DIRECTORY_ENTRY_BASERELOC, (PULONG)&size);
	if (pImageBaseRelocation == NULL)
	{
		MyDbgPrint("MyRelocationTable(),if (pImageBaseRelocation == NULL\n");
		return 0;
	}

	/*
		��ʼ�޸����� ���¶�λ

		��pImageBaseRelocation->VirtualAddress��ֵΪ�յ�ʱ��ͽ���ѭ��
	*/
	while (pImageBaseRelocation->VirtualAddress)
	{   //�õ���Ҫ�������ݵĸ���

		uRelocTableSize = (pImageBaseRelocation->SizeOfBlock - 8) / 2;
		
		//ѭ������
		WORD * pRelativeInfo = (WORD*)(pImageBaseRelocation + 1);
		for (i = 0; i < uRelocTableSize; i++)
		{//�жϸ�4λ�Ƿ����3
			
			Type = (*pRelativeInfo) >> 12;
			if (Type == IMAGE_REL_BASED_DIR64)
			{
				//��ָ��ָ��Ҫ�ض�λ������
				uRelocAddress = (PVOID)(((*pRelativeInfo) & 0xfff) + pImageBaseRelocation->VirtualAddress + (ULONG_PTR)NewImage);

				/*
					ƫ����ߵ����ݲ���Ҫ���¶�λ��ȫ�ֱ�������
					�����  uRelocAddress����Ҫ�ض�λ���ݵĵ�ַ
					��Ҫ�滻���ݵĵ�ַ(�������)��Ҫ���ϵ���   ��ģ���ַ-ӳ���ַ��ƫ��
				*/
				*(ULONG_PTR*)uRelocAddress +=  offset;
				//DbgPrint("�ض�λ��ĵ�ַ%llx\r\n", *(ULONG_PTR*)uRelocAddress);

			}
			pRelativeInfo++;
		}
		//��ָ���Ƶ���һ����,���->SizeOfBlockΪ����,��ʾû�п����˳�ѭ��
		pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((ULONG_PTR)pImageBaseRelocation + pImageBaseRelocation->SizeOfBlock);
	}

	MyDbgPrint("����ַ�ض�λ�ɹ�\n");
	return 1;
}

/*
	SSDT�ض�λ
*/
ULONG MySSDTRelocation(IN ULONG_PTR NewAddress, IN ULONG_PTR RawAddress)
{             
	/*
	      ʵ����
		�µ��ں˵�ַ�ǴӴ��̶�ȡ���ڴ��,SSDT�е�ֵ�ǿյ�
	    PSYSTEM_SERVICE_TABLE�еĳ�Ա����ֵ���ǿյ�
		����Ҫ����һЩ�ؼ���ֵ����
	*/
	ULONG_PTR offset; //����ƫ��
	PSYSTEM_SERVICE_TABLE  raw_ssdt=NULL,new_ssdt=NULL;

	typedef	struct ssdt_recover
	{	  
		ULONG64 add;
	}shuzu,*pshuzu;
	typedef union  asdf
	{
		ULONG  a[2];
		shuzu  s;
	}myunion, *pmyunion;
	myunion   add;
	ULONG i;
	ULONG  z; //���ڹ���ת��


	offset = NewAddress - RawAddress;
	
	raw_ssdt = (PSYSTEM_SERVICE_TABLE)MyGetKeServiceDescriptorTable64();
	if (raw_ssdt ==NULL)
	{
		MyDbgPrint("MySSDTRelocation()->��ȡSSDT�ṹʧ��\n");
		return 0;
	}
	new_ssdt = (PSYSTEM_SERVICE_TABLE)((ULONG_PTR)raw_ssdt + offset);
	/*
		�ؼ���ֵ�滻
	*/
	new_ssdt->NumberOfServices = raw_ssdt->NumberOfServices;
	new_ssdt->ServiceTableBase = (PVOID)((ULONG_PTR)raw_ssdt->ServiceTableBase + offset);


	/*
		SSDT��ƫ�Ʒǳ�����˼.�ں˳�ʼ����ʱ��ServiceTableBase�����е�ֵ
		����ÿ8���ֽڴ��һ����������ʵ��ַ,Ȼ���پ���һЩ�㷨
		��ServiceTableBase������ÿ4���ֽڴ��һ������ƫ�Ƶ�ֵ

		(��ServiceTableBase * 8 * 0x191)\2 ==��ĵ�ַ ������windbg�鿴
		dq �� �ڴ��д�ŵ��Ǻ����ľ��Ե�ַ �������������Ǵ� 
		SSDT�����к�0x191\2 �ĺ�����ʼ,����Ȥ�Ŀ��Բ鿴��;

		Ȼ��˵���㷨,������ServiceTableBase��ַΪ��,Ȼ����ÿ��
		ULONG A=(���Ժ�����ַ[i] - ServiceTableBase ==��ƫ��)+ServiceTableBase�ĵ�4�ֽ�
		�����һ��4�ֽڵı�����. Ȼ��  ����������㷨�������ȷ�ĵ�ַ
		(ServiceTableBase * i *4)= (A - ServiceTableBase�ĵ�4�ֽ�)<<4;
		
		���ULONG64 B > ULONG64 C , C - B =��ƫ��  L ;
		���  L ֱ��+B ==��ֵ ����һ�������ֵ.
		��ΪB�� 8���ֽ�, �������ƫ����4���ֽ�  ��4����ӽ��������8�ֽڵĸ�4�ֽ�
		���Ե�ַ�ͻ����.���ϵ���2��4�ֽڵı����������ȷ��ƫ�ƺ���д���ڴ�
	*/
	//��¼SSDT��ʵ�ĺ�����ַ
	for (i = 0; i < new_ssdt->NumberOfServices; i++)
	{
		gu64_Raw_SSDT[i] = *(ULONG64 *)((ULONG_PTR)new_ssdt->ServiceTableBase + i * 8);
		//DbgPrint("%llx\n", gu64_Raw_SSDT[i]);
	}

	add.s.add = (ULONG64)raw_ssdt->ServiceTableBase;
	for (i = 0; i < raw_ssdt->NumberOfServices; i++)
	{
		z = *(ULONG64 *)((ULONG_PTR)new_ssdt->ServiceTableBase + i * 8) - add.s.add;
		z += add.a[0];
		*(ULONG *)((ULONG64)new_ssdt->ServiceTableBase + i * 4) = (z-add.a[0])<<4;

	}
 	MyDbgPrint("raw_ssdt%p\n", raw_ssdt->ServiceTableBase);
 	MyDbgPrint("new_ssdt%p\n", new_ssdt->ServiceTableBase);
	
	for (i = 0; i < new_ssdt->NumberOfServices; i++)
	{
		gu_SSDT[i] = *(ULONG *)((ULONG64)new_ssdt->ServiceTableBase + i * 4);
		//DbgPrint("0x%08x\n", gu_SSDT[i]);
	}

	return 1;
}

ULONG64 get_ssdt_org_addr(ULONG index)
{
	if(gu64_Raw_SSDT[0] == NULL)
	{
		MyDbgPrint("gu64_raw_ssdt is null\n");
		return 0;
	}
	return gu64_Raw_SSDT[index];
}

//��Ҫ��DriverEntry��ʼ�� �õ�SSDT�Ļ���ַ;
ULONG64 get_ssdt_func_addr(ULONG index)
{
	LONG dwtmp=0;
	ULONGLONG add = 0;
	PULONG ServiceTableBase=NULL;
	ServiceTableBase=(PULONG)KeServiceDescriptorTable->ServiceTableBase;
	dwtmp=ServiceTableBase[index];
	dwtmp=dwtmp>>4;
	add=((LONGLONG)dwtmp + (ULONGLONG)ServiceTableBase);//&0xFFFFFFF0;
	return add;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//		SSSDT

//��WINDBG��鿴��
//ln win32k!W32pServiceTable+((poi(win32k!W32pServiceTable+4*(<syscall nbr>-1000))&0x00000000`ffffffff)>>4)-10000000
//u win32k!W32pServiceTable+((poi(win32k!W32pServiceTable+4*(Index-0x1000))&0x00000000`ffffffff)>>4)-0x10000000
//u poi(win32k!W32pServiceTable+4*(1-0x1000))
//u poi(win32k!W32pServiceTable+4*(1-0x1000))&0x00000000`ffffffff
//u (poi(win32k!W32pServiceTable+4*(1-0x1000))&0x00000000`ffffffff)>>4
//u win32k!W32pServiceTable+((poi(win32k!W32pServiceTable+4*(1-0x1000))&0x00000000`ffffffff)>>4)-0x10000000
//ULONGLONG GetSSSDTFuncCurAddr64(ULONG64 Index)
//{
//	ULONGLONG				W32pServiceTable = 0, qwTemp = 0;
//	LONG 					dwTemp = 0;
//	PSYSTEM_SERVICE_TABLE	pWin32k;
//	pWin32k = (PSYSTEM_SERVICE_TABLE)((ULONG64)KeServiceDescriptorTableShadow + sizeof(SYSTEM_SERVICE_TABLE));	//sizeof(SYSTEM_SERVICE_TABLE)
//	W32pServiceTable = (ULONGLONG)(pWin32k->ServiceTableBase);
//	ul64W32pServiceTable = W32pServiceTable;
//	//DbgPrint("W32pServiceTable: %llx",W32pServiceTable);
//	//DbgPrint("Service Count: %lld",pWin32k->NumberOfServices);
//	qwTemp = W32pServiceTable + 4 * (Index - 0x1000);	//�����ǻ��ƫ�Ƶ�ַ��λ�ã�ҪHOOK�Ļ��޸����Ｔ��
//	dwTemp = *(PLONG)qwTemp;
//	dwTemp = dwTemp >> 4;
//	qwTemp = W32pServiceTable + (LONG64)dwTemp;
//	//DbgPrint("0x%llx\n", qwTemp);
//	return qwTemp;
//}

ULONG64 GetSSSDTFuncCurAddr64(ULONG64 index)
{
	ULONGLONG				W32pServiceTable = 0, FunctionCount = 0,Index = 0;
	ULONGLONG				qwTemp = 0;
	LONG					dwTemp;
	PSYSTEM_SERVICE_TABLE	pWin32k;
	pWin32k = (PSYSTEM_SERVICE_TABLE)((ULONG64)KeServiceDescriptorTableShadow + 4 * 8);// ������һ�������
	W32pServiceTable = (ULONGLONG)(pWin32k->ServiceTableBase);
	FunctionCount = pWin32k->NumberOfServices;
	ul64W32pServiceTable = W32pServiceTable;
	//DbgPrint("%d\n",FunctionCount);
	
	
	Index = 0x1000 + index;
	qwTemp = W32pServiceTable + 4 * (Index - 0x1000);	//DbgPrint("qwTemp: %llx",qwTemp);
	dwTemp = *(PLONG)qwTemp;
	dwTemp = dwTemp >> 4;
	qwTemp = W32pServiceTable + (LONG64)dwTemp;
	/*DbgPrint(
		"id:%d\r\n"
		"Address: %llx\r\n", index, qwTemp);*/

		return qwTemp;
	
}

ULONGLONG GetKeServiceDescriptorTableShadow64()
{
	PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
	PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
	PUCHAR i = NULL;
	UCHAR b1 = 0, b2 = 0, b3 = 0;
	ULONG templong = 0;
	ULONGLONG addr = 0;
	for (i = StartSearchAddress; i<EndSearchAddress; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 2))
		{
			b1 = *i;
			b2 = *(i + 1);
			b3 = *(i + 2);
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x1d) //4c8d1d
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}


PEPROCESS GetGuiProcess(BOOLEAN bTry)
{
	ULONG i = 0;
	PEPROCESS eproc = NULL;
	for (i = 4; i<262144; i = i + 4)
	{
		eproc = LookupProcess((HANDLE)i);
		if (eproc != NULL)
		{
			ObDereferenceObject(eproc);
			CHAR *Name = (CHAR*)PsGetProcessImageFileName(eproc);
			/*if (!_strnicmp("explorer.exe", Name, strlen("explorer.exe")))
			{
				DbgPrint("EPROCESS=%p PID=%ld,Name=%s\n", eproc, PsGetProcessId(eproc), PsGetProcessImageFileName(eproc));
				return eproc;
			}
			else */
			if (bTry)
			{
				if (!_strnicmp("explorer.exe", Name, strlen("explorer.exe")))
				{
					DbgPrint("EPROCESS=%p PID=%ld,Name=%s\n", eproc, PsGetProcessId(eproc), PsGetProcessImageFileName(eproc));
					return eproc;
				}
			}
			else if (!_strnicmp("csrss.exe", Name, strlen("csrss.exe")))
			{
				DbgPrint("EPROCESS=%p PID=%ld,Name=%s\n", eproc, PsGetProcessId(eproc), PsGetProcessImageFileName(eproc));
				return eproc;
			}
		}
	}
	return NULL;
}


