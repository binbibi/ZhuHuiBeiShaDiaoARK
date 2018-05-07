// DlgSSDT.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgSSDT.h"
#include "afxdialogex.h"


DWORD CDlgSSDT::m_SortColum = 0;
BOOL CDlgSSDT::m_bAs = TRUE;

typedef struct _SSDT_INFO
{
	ULONG64 cut_addr;
	ULONG64 org_addr;
	CHAR	imgPath[MAX_PATH];
}SSDT_INFO, *PSSDT_INFO;
// CDlgSSDT 对话框

IMPLEMENT_DYNAMIC(CDlgSSDT, CPropertyPage)

CDlgSSDT::CDlgSSDT()
	: CPropertyPage(CDlgSSDT::IDD)
{

}

CDlgSSDT::~CDlgSSDT()
{
}

void CDlgSSDT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC__SSDT_LIST, m_list_ssdt);
}


BEGIN_MESSAGE_MAP(CDlgSSDT, CPropertyPage)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC__SSDT_LIST, &CDlgSSDT::MyListCompar)
END_MESSAGE_MAP()


// CDlgSSDT 消息处理程序


//原始地址与当前地址都在内核中完成

int   CALLBACK CDlgSSDT::MyListCompar(LPARAM   lParam1,   LPARAM   lParam2,   LPARAM   lParamSort) 
{ 
	//通过传递的参数来得到CSortList对象指针，从而得到排序方式 
	CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
	
	//通过ItemData来确定数据 

	//int   iCompRes; 
	CString    szComp1 = pListCtrl->GetItemText(lParam1,m_SortColum);
	CString    szComp2 = pListCtrl->GetItemText(lParam2,m_SortColum);
	
	//switch(m_SortColum) 
	//{ 
	////case(2): 
	////	//以第一列为根据排序   编号
 ////               //_ttol 
	////	iCompRes=_ttol(szComp1) <=_ttol(szComp2)?-1:1; 
	////	break; 
	////case(1):
	////	iCompRes = _ttoi(szComp1) <= _ttoi(szComp2)?-1:1;
	////	break;
	//
	//default: 
	//	iCompRes=szComp1.Compare(szComp2); /*a.Compare(b),如果a>b则返回1，如果a<b则返回-1*/
	//	break; 
	//} 
	//根据当前的排序方式进行调整
	
	if(m_bAs) 
		return wcscmp(szComp1, szComp2);
	else 
		return wcscmp(szComp2, szComp1);
} 


BOOL CDlgSSDT::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//index 函数名称 当前函数地址 hook 原始函数地址 当前地址所在模块
	m_list_ssdt.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	m_list_ssdt.InsertColumn(0,L"index",0,50);
	m_list_ssdt.InsertColumn(1,L"函数名称",0,150);
	m_list_ssdt.InsertColumn(2,L"当前函数地址",LVCFMT_CENTER,130);
	m_list_ssdt.InsertColumn(3,L"hook",LVCFMT_CENTER,70);
	m_list_ssdt.InsertColumn(4,L"原始函数地址",0,130);
	m_list_ssdt.InsertColumn(5,L"当前地址所在模块",LVCFMT_CENTER,200);
	EnumSSDT();
	return TRUE;
}

//DWORD FileLen(char *filename)
//{
//	WIN32_FIND_DATAA fileInfo={0};
//	DWORD fileSize=0;
//	HANDLE hFind;
//	hFind = FindFirstFileA(filename ,&fileInfo);
//	if(hFind != INVALID_HANDLE_VALUE)
//	{
//		fileSize = fileInfo.nFileSizeLow;
//		FindClose(hFind);
//	}
//	return fileSize;
//}
//
//CHAR *LoadDllContext(char *filename)
//{
//	DWORD dwReadWrite, LenOfFile=FileLen(filename);
//	HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
//	if (hFile != INVALID_HANDLE_VALUE)
//	{
//		PCHAR buffer=(PCHAR)malloc(LenOfFile);
//		SetFilePointer(hFile, 0, 0, FILE_BEGIN);
//		ReadFile(hFile, buffer, LenOfFile, &dwReadWrite, 0);
//		CloseHandle(hFile);
//		return buffer;
//	}
//	return NULL;
//}

DWORD GetSSDTFunctionIndex(char *FunctionName)
{
	return *(DWORD*)((PUCHAR)GetProcAddress(LoadLibraryW(L"ntdll.dll"),FunctionName)+4); //极限精简
	/*
	PUCHAR fptr=(PUCHAR)GetProcAddress(LoadLibraryW(L"ntdll.dll"),FunctionName);
	DWORD index=*(DWORD*)(fptr+4);
	return index;
	*/
	/*
	所有的函数都像下面一样：
	00000000`775a0110 4c8bd1          mov     r10,rcx
	00000000`775a0113 b8??000000      mov     eax,??h
	00000000`775a0118 0f05            syscall
	00000000`775a011a c3              ret
	*/
}

//负责传入Index和获取函数名
void CDlgSSDT::EnumSSDT(void)
{
	CString csSysroot;
	CString str;
	DWORD i = 0;
	DWORD fs_pos = 0;
	DWORD fe_pos = 0;
	DWORD ssdt_fun_cnt = 0;
	SSDT_INFO ssdt_addr_info = { 0 };

	DWORD dwRet = 0;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };

	if (!CopyFileW(L"c:\\windows\\system32\\ntdll.dll", L"c:\\ntdll.txt", 0))
		goto __end;

	DWORD fl = FileLen("c:\\ntdll.txt");
	
	//CString csSysroot;
	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);
	char func_name[MAX_PATH] = {0};

	char func_start[]="ZwAcceptConnectPort", func_end[]="ZwYieldExecution"; //每个函数名之间隔着\0
	char *funs=(char *)malloc(strlen(func_start)); memcpy(funs,func_start,strlen(func_start));
	char *fune=(char *)malloc(strlen(func_end)); memcpy(fune,func_end,strlen(func_end));
	char *ntdlltxt=LoadDllContext("c:\\ntdll.txt");

	for(i=0;i<fl;i++)
	{
		if(memcmp(ntdlltxt+i,funs,strlen(func_start))==0)
			fs_pos=i;
		if(memcmp(ntdlltxt+i,fune,strlen(func_end))==0)
		{	
			fe_pos=i;
			break;
		}
	}
	

	ntdlltxt=ntdlltxt+fs_pos;

	while(strcmp(ntdlltxt,func_end)!=0) 
	{
		DWORD fn_index=GetSSDTFunctionIndex(ntdlltxt);
		memset(func_name,'\0',MAX_PATH);
		strcpy(func_name,ntdlltxt);
		func_name[0]='N';
		func_name[1]='t';

		if(fn_index<1000)
		{

			//ssdt_func_ori_addr=GetFunctionOriginalAddress(fn_index);
			//IoControl(hSSDTDrv ,CTL_CODE_GEN(0x802), &fn_index, 4, &ssdt_func_addr, 8);
			DeviceIoControl(g_device,IOVTL_GETSSDTFUCADDR, &fn_index, sizeof(DWORD), &ssdt_addr_info, sizeof(SSDT_INFO),&dwRet,NULL);
			ntdlltxt[0]='N';ntdlltxt[1]='t';				//寻找的是Zw***，但是应该显示Nt***
			//if(ssdt_addr_info.cut_addr != ssdt_addr_info.org_addr)
			str.Format(L"%03d",fn_index);
			int nIntemNum = m_list_ssdt.GetItemCount();
			m_list_ssdt.InsertItem(nIntemNum,str);
			str.Format(L"%S",func_name);
			m_list_ssdt.SetItemText(nIntemNum,1,str);
			str.Format(L"0x%p",ssdt_addr_info.cut_addr);
			m_list_ssdt.SetItemText(nIntemNum,2,str);
			m_list_ssdt.SetItemText(nIntemNum,3,L"-");
			str.Format(L"0x%p",ssdt_addr_info.org_addr);
			m_list_ssdt.SetItemText(nIntemNum,4,str);

			str.Format(L"%S",ssdt_addr_info.imgPath);
			str.Replace(L"\\SystemRoot", csSysroot);
			m_list_ssdt.SetItemText(nIntemNum,5,str);
			
			if (ssdt_addr_info.cut_addr != ssdt_addr_info.org_addr)
			{

				//m_list_ssdt.SetItemTextColor(nIntemNum,RGB(186,12,4),FALSE);
			}
				

			/*m_list_ssdt.InsertColumn(1,L"函数名称",0,150);
			m_list_ssdt.InsertColumn(2,L"当前函数地址",LVCFMT_CENTER,130);
			m_list_ssdt.InsertColumn(3,L"hook",LVCFMT_CENTER,70);
			m_list_ssdt.InsertColumn(4,L"原始函数地址",0,130);
			m_list_ssdt.InsertColumn(5,L"当前地址所在模块",LVCFMT_CENTER,200);*/
				
		}
		else
		{
			fn_index=0x57;	//不同的系统，不同的编号，由于目前只有WIN7 X64，所以这里直接硬编码了
			//ssdt_func_ori_addr=GetFunctionOriginalAddress(fn_index);
			//IoControl(hSSDTDrv ,CTL_CODE_GEN(0x802), &fn_index, 4, &ssdt_func_addr, 8);
			DeviceIoControl(g_device,IOVTL_GETSSDTFUCADDR, &fn_index, sizeof(DWORD), &ssdt_addr_info, sizeof(SSDT_INFO),&dwRet,NULL);
			ntdlltxt[0]='N';ntdlltxt[1]='t';

			str.Format(L"%03d",fn_index);
			int nIntemNum = m_list_ssdt.GetItemCount();
			m_list_ssdt.InsertItem(nIntemNum,str);
			str.Format(L"%S",func_name);
			m_list_ssdt.SetItemText(nIntemNum,1,str);
			str.Format(L"0x%p",ssdt_addr_info.cut_addr);
			m_list_ssdt.SetItemText(nIntemNum,2,str);
			m_list_ssdt.SetItemText(nIntemNum,3,L"-");
			str.Format(L"0x%p",ssdt_addr_info.org_addr);
			m_list_ssdt.SetItemText(nIntemNum,4,str);

			str.Format(L"%S", ssdt_addr_info.imgPath);
			str.Replace(L"\\SystemRoot", csSysroot);
			m_list_ssdt.SetItemText(nIntemNum,5,str);

			if (ssdt_addr_info.cut_addr != ssdt_addr_info.org_addr)
			{
				m_list_ssdt.SetItemText(nIntemNum, 3, L"!!!!!!!!!!!!!");
				//m_list_ssdt.SetItemTextColor(nIntemNum, RGB(186, 12, 4), FALSE);
			}
				
			/*if(ssdt_func_ori_addr!=ssdt_func_addr)
				printf("0x%-0.3X!\t%llx  %llx  %s\n",fn_index,ssdt_func_addr,ssdt_func_ori_addr,ntdlltxt);
			else
				printf("0x%-0.3X\t%llx  %llx  %s\n",fn_index,ssdt_func_addr,ssdt_func_ori_addr,ntdlltxt);*/
			/*
			0:000> u ZwQuerySystemTime
			ntdll!ZwQuerySystemTime:
			00000000`77b20450 e91b62fdff      jmp     ntdll!RtlQuerySystemTime (00000000`77af6670)
			00000000`77b20455 6666660f1f840000000000 nop word ptr [rax+rax]
			*/
			/*
			nt!ZwQuerySystemTime:
			fffff800`01673fa0 488bc4          mov     rax,rsp
			fffff800`01673fa3 fa              cli
			fffff800`01673fa4 4883ec10        sub     rsp,10h
			fffff800`01673fa8 50              push    rax
			fffff800`01673fa9 9c              pushfq
			fffff800`01673faa 6a10            push    10h
			fffff800`01673fac 488d053d270000  lea     rax,[nt!KiServiceLinkage (fffff800`016766f0)]
			fffff800`01673fb3 50              push    rax
			fffff800`01673fb4 b857000000      mov     eax,57h
			fffff800`01673fb9 e9825e0000      jmp     nt!KiServiceInternal (fffff800`01679e40)
			fffff800`01673fbe 6690            xchg    ax,ax
			*/
		}
		ntdlltxt=ntdlltxt+strlen(ntdlltxt)+1;
		ssdt_fun_cnt++;
	}

	//显示完最后一个函数
	DWORD fn_index=GetSSDTFunctionIndex(ntdlltxt); 
	strcpy(func_name,ntdlltxt);
	func_name[0]='N';
	func_name[1]='t';
	//ssdt_func_ori_addr=GetFunctionOriginalAddress(fn_index);
	//IoControl(hSSDTDrv ,CTL_CODE_GEN(0x802), &fn_index, 4, &ssdt_func_addr, 8);
	DeviceIoControl(g_device,IOVTL_GETSSDTFUCADDR, &fn_index, sizeof(DWORD), &ssdt_addr_info, sizeof(SSDT_INFO),&dwRet,NULL);
	//ntdlltxt[0]='N';ntdlltxt[1]='t';

	str.Format(L"%03d",fn_index);
	int nIntemNum = m_list_ssdt.GetItemCount();
	m_list_ssdt.InsertItem(nIntemNum,str);
	str.Format(L"%S",func_name);
	m_list_ssdt.SetItemText(nIntemNum,1,str);
	str.Format(L"0x%p",ssdt_addr_info.cut_addr);
	m_list_ssdt.SetItemText(nIntemNum,2,str);
	m_list_ssdt.SetItemText(nIntemNum,3,L"-");
	str.Format(L"0x%p",ssdt_addr_info.org_addr);
	m_list_ssdt.SetItemText(nIntemNum,4,str);

	str.Format(L"%S", ssdt_addr_info.imgPath);
	str.Replace(L"\\SystemRoot", csSysroot);
	m_list_ssdt.SetItemText(nIntemNum, 5, str);


	if (ssdt_addr_info.cut_addr != ssdt_addr_info.org_addr)
	{
		m_list_ssdt.SetItemText(nIntemNum, 3, L"!!!!!!!!!!!!!");
		//m_list_ssdt.SetItemTextColor(nIntemNum, RGB(186, 12, 4), FALSE);
	}
	//printf("0x%-0.3X\t%llx  %llx  %s\n",fn_index,ssdt_func_addr,ssdt_func_ori_addr,ntdlltxt);
	ssdt_fun_cnt++;
__end:
	//显示ssdt表上的函数数
	//printf("\nTotal of SSDT function: %ld\n",ssdt_fun_cnt);
	str.Format(L"猪会被杀掉:[SSDT管理器]共有 %d 个SSDT函数",ssdt_fun_cnt);
	AfxGetMainWnd()->SetWindowTextW(str);
	DeleteFileA("c:\\ntdll.txt");
}


void CDlgSSDT::MyListCompar(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	 NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
      m_SortColum = pNMListView->iSubItem;
	  m_bAs=!m_bAs;//升序还是降序
	  

      int count = m_list_ssdt.GetItemCount();   //行数
      for (int i=0; i<count;  i++)  
	  {  
		 m_list_ssdt.SetItemData(i,i);  
	  }
	  m_list_ssdt.SortItems(MyListCompar, (LPARAM) &m_list_ssdt);
      *pResult = 0;
}
