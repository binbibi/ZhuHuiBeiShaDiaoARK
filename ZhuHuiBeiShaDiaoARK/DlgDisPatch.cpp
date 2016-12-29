// DlgDisPatch.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgDisPatch.h"
#include "afxdialogex.h"



typedef struct  _IRP_DISPATCH_INFO
{
	ULONG index;
	ULONG_PTR CurrentAddr;
	ULONG_PTR OrgAddr;
	WCHAR	  szModuleName[MAX_PATH];
}IRP_DISPATCH_INFO, *PIRP_DISPATCH_INFO;

PIRP_DISPATCH_INFO pIrpDisPatchInfo = NULL;
// CDlgDisPatch 对话框

IMPLEMENT_DYNAMIC(CDlgDisPatch, CDialogEx)

CDlgDisPatch::CDlgDisPatch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDisPatch::IDD, pParent)
{

}

CDlgDisPatch::~CDlgDisPatch()
{
}

void CDlgDisPatch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DISPATCH, m_list_DisPatch);
}


BEGIN_MESSAGE_MAP(CDlgDisPatch, CDialogEx)
END_MESSAGE_MAP()


// CDlgDisPatch 消息处理程序


BOOL CDlgDisPatch::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_list_DisPatch.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	//m_list_dirver.InsertColumn(0,L"index",0,50);
	m_list_DisPatch.InsertColumn(0,L"Id",0,30);
	m_list_DisPatch.InsertColumn(1,L"Irp",0,120);
	m_list_DisPatch.InsertColumn(2,L"当前地址",0,130);
	m_list_DisPatch.InsertColumn(3,L"原始地址",0,130);
	m_list_DisPatch.InsertColumn(4,L"文件厂商",0,120);
	m_list_DisPatch.InsertColumn(5,L"路径",0,320);
	if(!EnumIrpDisPatch(g_DriverName.GetBuffer()))
		return FALSE;
		
	return TRUE;
}


BOOL CDlgDisPatch::EnumIrpDisPatch(PWCHAR DriverName)
{
	CString str;
	CString szTrim = L"\\??\\";
	CString csSysroot;
	WCHAR	szSysRootBuff[MAX_PATH] = {0};
	WCHAR	szFileInfo[_MAX_PATH] = {0};
	WCHAR *szDriverName = (WCHAR*)malloc(MAX_PATH);
	

	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s",szSysRootBuff);

	if(szDriverName == NULL)
		return FALSE;
	pIrpDisPatchInfo = (PIRP_DISPATCH_INFO)malloc(sizeof(IRP_DISPATCH_INFO) * 100);
	if(pIrpDisPatchInfo == NULL)
		return FALSE;

	memset(szDriverName, 0, MAX_PATH);
	memset(pIrpDisPatchInfo, 0, sizeof(IRP_DISPATCH_INFO) * 100);
	RtlCopyMemory(szDriverName, g_DriverName.GetBuffer(), g_DriverName.GetLength()*2 - 8);

	if(!ARKDeviceIoControl(IOCTL_ENUMDIRPISPATCH,szDriverName, MAX_PATH,pIrpDisPatchInfo,sizeof(IRP_DISPATCH_INFO) * 100))
	{
		
		//AfxMessageBox(L"无法查看此驱动的分发例程");
		free(szDriverName);
		szDriverName = NULL;
		free(pIrpDisPatchInfo);
		pIrpDisPatchInfo = NULL;

		str.Format(L"无法获取 [%s] 的分发函数",g_DriverName);

		this->SetWindowTextW(str);
		return FALSE;
	}
	
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; i++)
	{
		UINT nItem = m_list_DisPatch.GetItemCount();
		str.Format(L"%d", pIrpDisPatchInfo[i].index);
		m_list_DisPatch.InsertItem(nItem,str);

		str.Format(L"%S",MajorFun[i]);
		m_list_DisPatch.SetItemText(nItem, 1, str);

		str.Format(L"0x%p", pIrpDisPatchInfo[i].CurrentAddr);
		m_list_DisPatch.SetItemText(nItem, 2, str);

		str.Format(L"-");
		m_list_DisPatch.SetItemText(nItem, 3, str);

		str.Format(L"%s", pIrpDisPatchInfo[i].szModuleName);
		str = str.TrimLeft(szTrim);
		str.Replace(L"SystemRoot", csSysroot);
		m_list_DisPatch.SetItemText(nItem, 5, str);

		GetFileInfo(str.GetBuffer(), szFileInfo);
		str.Format(L"%s", szFileInfo);
		m_list_DisPatch.SetItemText(nItem, 4, str);

		memset(szFileInfo, 0, MAX_PATH);

	}


	free(szDriverName);
	szDriverName = NULL;
	free(pIrpDisPatchInfo);
	pIrpDisPatchInfo = NULL;

	str.Format(L"[%s] 的分发函数",g_DriverName);

	this->SetWindowTextW(str);

	return TRUE;
}
