

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


//获取pe结构中数据目录表结构
NTSYSAPI PVOID  RtlImageDirectoryEntryToData(
	PVOID Base, //模块基地址
	BOOLEAN MappedAsImage,//是否映射为映象。
	USHORT Directory, //数据目录项的索引
	 OUT PULONG Size); //对应数据目录项的大小






static ULONG_PTR MyZwQuerySystemInformation(IN ULONG Number);
static ULONG_PTR MyModuleQuery(char* MudolePath);
static ULONG_PTR MyZwQuerySystemInformation(IN ULONG Number);
static ULONG_PTR QueryFunction(PWCHAR name);
static ULONGLONG MyGetKeServiceDescriptorTable64();
static PULONG MyPeLoad(IN char* cStr, IN WCHAR *wcSrt);
static ULONG  MyRelocationTable(PVOID  NewImage, PVOID RawImage);
static ULONG MySSDTRelocation(IN ULONG_PTR NewAddress, IN ULONG_PTR RawAddress);
static KIRQL WPOFFx64(); //关闭页面保护
static void WPONx64(KIRQL irql);//恢复页面保护
/*
获取SSDT正确偏移,成功返回真;
*/
static BOOLEAN Relocation(); //获取SSDT偏移信息
static ULONG FuncOffset(ULONG64 FuncAdd); //函数偏移

#define  MyDbgPrint  DbgPrint 
#define _max(a,b)  a>b?a:b //计算ab值那个大用哪个
ULONG gu_SSDT[0x300] = { NULL };
ULONG64 gu64_Raw_SSDT[0x200] = { NULL };
ULONG COUNT=0;
BOOLEAN FLAG = FALSE;
ULONG64 off = NULL;


KIRQL WPOFFx64()
{
	/*
	提升IRQL等级到Dispatch_Level
	关闭页面保护,CR0寄存器的值16位清0  0~16
	硬件中断标志位清0 ,不响应可屏蔽中断
	实现了,多线程安全性   _disable为硬件中断不响应
	Dispatch_Level为线程不可切换软中断
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
	恢复页面保护. CRO寄存器值16位值1  0~16
	IF中断标志至1  可以响应可屏蔽中断
	降低软中断级别
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
//	char KiSystemServiceStart_pattern[13] = "\x8B\xF8\xC1\xEF\x07\x83\xE7\x20\x25\xFF\x0F\x00\x00";	//鬼佬的特征码
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
	一键恢复SSDT的所有HOOK
*/
BOOLEAN UnhookSSDT()
{

	PSYSTEM_SERVICE_TABLE ssdt = NULL;
	ULONG i = 0;
	ULONG judge;//判断
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
		MyDbgPrint("UnhookSSDT()->全局数组变量无效");
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
			//MyDbgPrint("被HOOK的SSDT序列号=%d,正确的函数地址%llx:\n", i, GetSSDTFunctionAddress64_2(i));
		}

	}
	
	MyDbgPrint("UnhookSSDT()->SSDT恢复完毕");
	return TRUE;

}

/*
	获取SSDT正确偏移,成功返回真;
*/
BOOLEAN Relocation()
{
	ULONG a = 0;
	char module_path[260] = { NULL }; //内核路径
	ULONG_PTR base=NULL;//内核地址
	PULONG  new_module_pointer; //新内核
	base=MyModuleQuery(module_path);
	if (base==NULL)
	{
		MyDbgPrint("Relocation()->获取模块地址失败\n");
		return FALSE;
	}
	new_module_pointer = MyPeLoad(module_path, NULL);
	if (new_module_pointer==0)
	{
		MyDbgPrint("Relocation()->new_module_pointer==0)\n");
		return FALSE;
	}
	MyDbgPrint("新地址%p\n", new_module_pointer);
	

	a = MyRelocationTable(new_module_pointer, (PVOID)base);
	 if (a==0)
	 {
		 MyDbgPrint("Relocation()->基地址重定位数据失败\n");
		 ExFreePool(new_module_pointer);
		 return FALSE;
	 }
	a= MySSDTRelocation((ULONG_PTR)new_module_pointer, base);
	if (a == 0)
	{
		MyDbgPrint("Relocation()->SSDT重定位失败\n");
		ExFreePool(new_module_pointer);
		return FALSE;
	}
	if (gu_SSDT[0]==NULL)
	{
		MyDbgPrint("Relocation()->获取SSDT偏移失败\n");
		ExFreePool(new_module_pointer);
		return FALSE;
	}
	if (gu64_Raw_SSDT[0] == NULL)
	{
		MyDbgPrint("Relocation()->获取SSDT真实地址失败\n");
		ExFreePool(new_module_pointer);
		return FALSE;
	}
	ExFreePool(new_module_pointer);
	return TRUE;
}

/*
   获取内核模块 路径跟地址
*/
ULONG_PTR MyModuleQuery(OUT char* ModulePath )
{
	PSYSTEM_MODULE_INFORMATION  module = NULL;
	ULONG_PTR Base;
	module = (PSYSTEM_MODULE_INFORMATION)MyZwQuerySystemInformation(11);
	if (module==NULL)
	{
		MyDbgPrint("MyModuleQuery(),获取模块错误\n\r");
		return 0;
	}

	MyDbgPrint("模块路径:%s\n", module->Module->ImageName);
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
	//或许实际需要的缓冲区大小传入buffer1中
	status = ZwQuerySystemInformation(Number, Buffer, buffer1, &buffer1);
	ExFreePool(Buffer);
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		//分配一个无分页缓冲区NonPagedPool
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
			MyDbgPrint("错误状态=%d\r\n", RtlNtStatusToDosError(status));
			//分配缓冲区后如果失败就必须把分配的内存释放掉
			ExFreePool(Buffer);
			MyDbgPrint("GetSystemIniformation(),if (!NT_SUCCESS(status)),Error\r\n");
			return 0;
		}
		return (ULONG_PTR)Buffer;
	}
	MyDbgPrint("GetSystemIniformation(),错误状态=%d\r\n", RtlNtStatusToDosError(status));
	return 0;
}

/*
	输入导出函数名字 L""
	获取函数地址
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
	加载PE到内存
	传入ascii 字符格式文件路径,或者是unicode 字符格式文件路径
	只能二选一,其中一个为0;
*/
PULONG MyPeLoad(IN char* cStr, IN WCHAR *wcSrt)
{
	/*
	读取pe文件到内存的流程,   (是先从硬盘读取文件,然后加载到内存)
	1,先通过一个路径打开一个文件,
	2,然后开始读取PE文件的各个信息
	3,然后从获得的信息中获取文件需要的大小后 分配同样的大小的缓冲区
	4,然后在依次按照规定,在把文件从DOS头 到模块依次读取到刚分配的缓冲区内存中
	*/

	HANDLE    hfile;     //接受句柄
	NTSTATUS  status;    //状态
	PVOID sizeof_image=NULL;  //载入内存后的地址指针
	IO_STATUS_BLOCK      io_status_block;    //接受状态结构
	OBJECT_ATTRIBUTES    object_attributes;  //句柄属性
	UNICODE_STRING       path_name;       
	ANSI_STRING  ansi_path;
	ULONG flag = 0;//标志
	ULONG Characteristics = 0;

	IMAGE_DOS_HEADER	image_dos_header;//dos头结构
	LARGE_INTEGER       large_integer;//记录偏移
	

	IMAGE_NT_HEADERS64   image_nt_header;//NT头

	IMAGE_SECTION_HEADER * p_image_section_header;//指向多个区块结构

	ULONG sizeof_raw_data;

	ULONG i = 0;
	/*
		判断传入的参数后初始化字符串
	*/
	if (cStr != NULL&&wcSrt != NULL)
	{
		MyDbgPrint("请输入一个路径,MyPeLoad()");
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
			MyDbgPrint("MyLoadModule(),(cStr != NULL&&wcSrt == NULL)错误=%d\n", RtlNtStatusToDosError(status));
			return 0;
		}
		flag = 1;
	}
	else
	{
		MyDbgPrint("输入参数无效,MyLoadModule,else\n");
		return 0;
	}




	/*
	    初始化对象属性
	    ZwCreateFile函数呢不直接接受字符串只接受一个OBJECT_ATTRIBUTES的一个结构
	    所以我们要用InitializeObjectAttributes函数来初始化这个结构 
		然后在当成参数传给ZwCreateFile
	*/
	InitializeObjectAttributes(&object_attributes, //对象属性变量 POBJECT_ATTRIBUTES OUT  
		&path_name,								   //文件名   PUNICODE_STRING
		OBJ_CASE_INSENSITIVE,                      //表示不区分大小写
		NULL,                                      //NULL
		NULL);                                     //NULL
	MyDbgPrint("OBJECT_ATTRIBUTES后验证路径:%wZ\n", object_attributes.ObjectName);
	/*
		打开文件后,开始读取到内存
	*/
	status = ZwCreateFile(
		&hfile,                  //返回的句柄  OUT PHANDLE
		FILE_ALL_ACCESS,         //访问权限->所有权限
		&object_attributes,      //POBJECT_ATTRIBUTES 该结构包含要打开的文件名
		&io_status_block,        //PIO_STATUS_BLOCK 返回结果状态 OUT
		0,                       //初始分配大小,0是动态分配
		FILE_ATTRIBUTE_NORMAL,   //文件属性 一般为<-或者0;
		FILE_SHARE_READ,         //指定共享方式一般<- 或者0;
		FILE_OPEN,               //这个参数指定要对文件干嘛
		FILE_NON_DIRECTORY_FILE, //指定控制打开操作和句柄使用的附加标志位
		NULL,                    //指向可选的扩展属性区
		0);                      //扩展属性区的长度
	if (!NT_SUCCESS(status))
	{
		MyDbgPrint("MyLoadModule(),status = ZwCreateFile错误=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	/*
	InitializeObjectAttributes初始化对象属性,用的全局路径名字还是个指针
	如果释放字符串缓冲区太早的话就会因路径参数为空而打开文件失败	
	*/
	if (flag == 1) //表示如果传入的是ansi码需要把转换时分配的缓冲区释放掉
	{
		RtlFreeUnicodeString(&path_name);
	}

	//读取DOS头******
	
	large_integer.QuadPart = 0;
	status = ZwReadFile(hfile,       //ZwCreateFile成功后得到的句柄  
		NULL,                        //一个事件  NULL
		NULL,                        //回调例程。NULL
		NULL,                        //NULL
		&io_status_block,            //PIO_STATUS_BLOCK 返回结果状态 OUT ,同上
		&image_dos_header,           //存放读取数据的缓冲区 OUT PVOID  
		sizeof(IMAGE_DOS_HEADER),    //试图读取文件的长度
		&large_integer,              //要读取数据相对文件的偏移量PLARGE_INTEGER
		0);                          //NULL
	if (!NT_SUCCESS(status))
	{
		ZwClose(hfile);
		MyDbgPrint("MyLoadModule(),status = ZwCreateFile错误=%d\n", RtlNtStatusToDosError(status));
		return 0;
		
	}
	//读取NT头*******
	
	large_integer.QuadPart = image_dos_header.e_lfanew; //PE头偏移
	status = ZwReadFile(hfile,       //ZwCreateFile成功后得到的句柄  
		NULL,                        //一个事件  NULL
		NULL,                        //回调例程。NULL
		NULL,                        //NULL
		&io_status_block,            //PIO_STATUS_BLOCK 返回结果状态 OUT ,同上
		&image_nt_header,           //存放读取数据的缓冲区 OUT PVOID  
		sizeof(IMAGE_NT_HEADERS64),    //试图读取文件的长度
		&large_integer,              //要读取数据相对文件的偏移量PLARGE_INTEGER
		0);                          //NULL
	if (!NT_SUCCESS(status))
	{
		ZwClose(hfile);
		MyDbgPrint("MyLoadModule(),status = ZwCreateFile错误=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	//读取区块*****
	
	//分配所有模块总大小
	p_image_section_header = (IMAGE_SECTION_HEADER*)ExAllocatePool(NonPagedPool,        //NonPagedPool  从非分页内存池中分配内存 
		sizeof(IMAGE_SECTION_HEADER)*image_nt_header.FileHeader.NumberOfSections);
	memset(p_image_section_header, 0, sizeof(p_image_section_header));
	//读
	large_integer.QuadPart += sizeof(IMAGE_NT_HEADERS64); //区块偏移
	status = ZwReadFile(hfile,       //ZwCreateFile成功后得到的句柄  
		NULL,                        //一个事件  NULL
		NULL,                        //回调例程。NULL
		NULL,                        //NULL
		&io_status_block,            //PIO_STATUS_BLOCK 返回结果状态 OUT ,同上
		p_image_section_header,           //存放读取数据的缓冲区 OUT PVOID  
		sizeof(IMAGE_SECTION_HEADER)*image_nt_header.FileHeader.NumberOfSections,    //试图读取文件的长度
		&large_integer,              //要读取数据相对文件的偏移量PLARGE_INTEGER
		0);                          //NULL
	if (!NT_SUCCESS(status))
	{
		ExFreePool(p_image_section_header);
		ZwClose(hfile);
		MyDbgPrint("MyLoadModule(),large_integer.QuadPart +=错误=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	sizeof_image = ExAllocatePool(NonPagedPool, image_nt_header.OptionalHeader.SizeOfImage);//NonPagedPool  从非分页内存池中分配内存 
	if (sizeof_image == 0)
	{
		ZwClose(hfile);
		KdPrint(("sizeof_image ExAllocatePool Failed!"));
		ExFreePool(p_image_section_header);  //释放内存
		MyDbgPrint("MyLoadModule(),sizeof_image ExAllocatePool=错误=%d\n", RtlNtStatusToDosError(status));
		return 0;
	}
	//初始化下内存
	memset(sizeof_image, 0, image_nt_header.OptionalHeader.SizeOfImage);
	RtlCopyMemory(sizeof_image, &image_dos_header, sizeof(IMAGE_DOS_HEADER));        //dos头
	RtlCopyMemory((PVOID)((ULONG_PTR)sizeof_image + image_dos_header.e_lfanew),
		&image_nt_header, sizeof(IMAGE_NT_HEADERS));                                 //nt头
	RtlCopyMemory((PVOID)((ULONG_PTR)sizeof_image + image_dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS)),       //区块
		p_image_section_header, sizeof(IMAGE_SECTION_HEADER)*image_nt_header.FileHeader.NumberOfSections);//计算区块总大小
	//读取各个数据段的实际地址
	
	for (i = 0; i < image_nt_header.FileHeader.NumberOfSections; i++)
	{   //磁盘占用的大小选择最大的
		sizeof_raw_data = _max(p_image_section_header[i].Misc.VirtualSize, p_image_section_header[i].SizeOfRawData);
		large_integer.QuadPart = p_image_section_header[i].PointerToRawData;   //各个磁盘的偏移地址

		// 代码段 可执行 可读
		Characteristics = p_image_section_header[i].Characteristics;
		if ((Characteristics & 0x60000020) == 0x60000020)
		{
			// 这里用于保存要扫描钩子的节
			DbgPrint("[CodeSection]%s\n", p_image_section_header[i].Name);
		}
		//读
		status = ZwReadFile(hfile,       //ZwCreateFile成功后得到的句柄  
			NULL,                        //一个事件  NULL
			NULL,                        //回调例程。NULL
			NULL,                        //NULL
			&io_status_block,            //PIO_STATUS_BLOCK 返回结果状态 OUT ,同上
			(PVOID)((ULONG_PTR)sizeof_image + p_image_section_header[i].VirtualAddress), //区块装入内存后的相对文件头偏移量          
			sizeof_raw_data,             //试图读取文件的长度
			&large_integer,              //要读取数据相对文件的偏移量PLARGE_INTEGER
			0);                          //NULL
		if (!NT_SUCCESS(status))
		{
			MyDbgPrint("循环区块出错[%s]%x\n",
				p_image_section_header[i].Name,
				(ULONG_PTR)sizeof_image + p_image_section_header[i].VirtualAddress);

			ExFreePool(sizeof_image);
			ExFreePool(p_image_section_header);  //释放内存
			ZwClose(hfile);
			MyDbgPrint("MyLoadModule(),错误=%d\n", RtlNtStatusToDosError(status));
			return 0;
		}
	}
	ExFreePool(p_image_section_header);  //释放内存
	ZwClose(hfile);
	return (PULONG)sizeof_image;
}

/*
	模块全局变量地址重定位表
*/
ULONG  MyRelocationTable(PVOID  NewImage,PVOID RawImage)
{

	//_IMAGE_OPTIONAL_HEADER64
	ULONG					i;                       //for循环变量
	ULONG					uRelocTableSize = 0;     //存放数据块中的数据总个数
	ULONG					Type;                    //16位数据高4位
	PVOID 					uRelocAddress = 0;       //指向需要修改内容的地址
	PIMAGE_BASE_RELOCATION	pImageBaseRelocation = 0;//重定位表
	ULONG_PTR size;
	ULONG_PTR  offset;//偏移
	/*
		RtlImageNtHeader可以直接获取模块的NT头
	*/
	PIMAGE_NT_HEADERS64 nt_header=NULL;
	nt_header = RtlImageNtHeader(RawImage);
	if (!nt_header)
	{
		MyDbgPrint("MyRelocationTable()->if(!nt_header)错误\n");
		return 0;
	}
	//MyDbgPrint("重载模块ImageBase地址%p", nt_header->OptionalHeader.ImageBase);
	MyDbgPrint("旧模块ImageBase地址%p\n", nt_header->OptionalHeader.ImageBase);
	/*
		这里我们用的原始内核的偏移,目的就是定位到原始内核里的数据
	*/
	offset = (ULONG_PTR)RawImage - nt_header->OptionalHeader.ImageBase;
	/*
	     RtlImageDirectoryEntryToData函数
		 可以获取数据目录表中的任意结构
		 这里IMAGE_DIRECTORY_ENTRY_BASERELOC ==5 重定位表
	*/
	pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)RtlImageDirectoryEntryToData(NewImage, TRUE, IMAGE_DIRECTORY_ENTRY_BASERELOC, (PULONG)&size);
	if (pImageBaseRelocation == NULL)
	{
		MyDbgPrint("MyRelocationTable(),if (pImageBaseRelocation == NULL\n");
		return 0;
	}

	/*
		开始修改数据 重新定位

		等pImageBaseRelocation->VirtualAddress的值为空的时候就结束循环
	*/
	while (pImageBaseRelocation->VirtualAddress)
	{   //得到需要更改数据的个数

		uRelocTableSize = (pImageBaseRelocation->SizeOfBlock - 8) / 2;
		
		//循环遍历
		WORD * pRelativeInfo = (WORD*)(pImageBaseRelocation + 1);
		for (i = 0; i < uRelocTableSize; i++)
		{//判断高4位是否等于3
			
			Type = (*pRelativeInfo) >> 12;
			if (Type == IMAGE_REL_BASED_DIR64)
			{
				//让指针指向要重定位的数据
				uRelocAddress = (PVOID)(((*pRelativeInfo) & 0xfff) + pImageBaseRelocation->VirtualAddress + (ULONG_PTR)NewImage);

				/*
					偏移里边的数据才是要重新定位的全局变量数据
					计算出  uRelocAddress是需要重定位数据的地址
					而要替换数据的地址(里的内容)需要加上的是   新模块基址-映像基址的偏移
				*/
				*(ULONG_PTR*)uRelocAddress +=  offset;
				//DbgPrint("重定位后的地址%llx\r\n", *(ULONG_PTR*)uRelocAddress);

			}
			pRelativeInfo++;
		}
		//把指针移到下一个快,如果->SizeOfBlock为空了,表示没有块了退出循环
		pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((ULONG_PTR)pImageBaseRelocation + pImageBaseRelocation->SizeOfBlock);
	}

	MyDbgPrint("基地址重定位成功\n");
	return 1;
}

/*
	SSDT重定位
*/
ULONG MySSDTRelocation(IN ULONG_PTR NewAddress, IN ULONG_PTR RawAddress)
{             
	/*
	      实验结果
		新的内核地址是从磁盘读取到内存的,SSDT中的值是空的
	    PSYSTEM_SERVICE_TABLE中的成员变量值都是空的
		所以要进行一些关键数值复制
	*/
	ULONG_PTR offset; //函数偏移
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
	ULONG  z; //用于过渡转换


	offset = NewAddress - RawAddress;
	
	raw_ssdt = (PSYSTEM_SERVICE_TABLE)MyGetKeServiceDescriptorTable64();
	if (raw_ssdt ==NULL)
	{
		MyDbgPrint("MySSDTRelocation()->获取SSDT结构失败\n");
		return 0;
	}
	new_ssdt = (PSYSTEM_SERVICE_TABLE)((ULONG_PTR)raw_ssdt + offset);
	/*
		关键数值替换
	*/
	new_ssdt->NumberOfServices = raw_ssdt->NumberOfServices;
	new_ssdt->ServiceTableBase = (PVOID)((ULONG_PTR)raw_ssdt->ServiceTableBase + offset);


	/*
		SSDT的偏移非常有意思.内核初始化的时候ServiceTableBase数组中的值
		都是每8个字节存放一个函数的真实地址,然后再经过一些算法
		把ServiceTableBase数组中每4个字节存放一个函数偏移的值

		(以ServiceTableBase * 8 * 0x191)\2 ==后的地址 可以用windbg查看
		dq 后 内存中存放的是函数的绝对地址 函数的名称则是从 
		SSDT总序列号0x191\2 的函数开始,有兴趣的可以查看下;

		然后说下算法,它是以ServiceTableBase地址为例,然后让每个
		ULONG A=(绝对函数地址[i] - ServiceTableBase ==的偏移)+ServiceTableBase的低4字节
		存放在一个4字节的变量里. 然后  按照下面的算法后才是正确的地址
		(ServiceTableBase * i *4)= (A - ServiceTableBase的低4字节)<<4;
		
		如果ULONG64 B > ULONG64 C , C - B =的偏移  L ;
		如果  L 直接+B ==的值 将是一个错误的值.
		因为B是 8个字节, 计算出的偏移是4个字节  低4节相加将会溢出到8字节的高4字节
		所以地址就会错误.果断的让2个4字节的变量计算出正确的偏移后再写入内存
	*/
	//记录SSDT真实的函数地址
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

//需要在DriverEntry初始化 得到SSDT的基地址;
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

//在WINDBG里查看：
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
//	qwTemp = W32pServiceTable + 4 * (Index - 0x1000);	//这里是获得偏移地址的位置，要HOOK的话修改这里即可
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
	pWin32k = (PSYSTEM_SERVICE_TABLE)((ULONG64)KeServiceDescriptorTableShadow + 4 * 8);// 跳过第一个服务表
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


