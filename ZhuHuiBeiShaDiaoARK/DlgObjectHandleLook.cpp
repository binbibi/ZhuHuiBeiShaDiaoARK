// DlgObjectHandleLook.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgObjectHandleLook.h"
#include "afxdialogex.h"

typedef struct _OBJECT_PROCEDURE_INFO
{
	ULONG64	DumpProcedrure;
	ULONG64	OpenProcedure;
	ULONG64	CloseProcedrure;
	ULONG64	DeleteProcedure;
	ULONG64	ParseProcedure;
	ULONG64	SecurityProcedure;
	ULONG64	QueryNameProcedure;
	ULONG64	OkayToCloseProcedure;
	CHAR	DumpImgPath[MAX_PATH];
	CHAR	OpenImgPath[MAX_PATH];
	CHAR	CloseImgPath[MAX_PATH];
	CHAR	DeleteImgPath[MAX_PATH];
	CHAR	ParseImgPath[MAX_PATH];
	CHAR	SecurityImgPath[MAX_PATH];
	CHAR	QueryNameImgPath[MAX_PATH];
	CHAR	OkayToCloseImgPath[MAX_PATH];
}OBJECT_PROCEDURE_INFO, *POBJECT_PROCEDURE_INFO;

OBJECT_PROCEDURE_INFO pObjHandleInfo;

// CDlgObjectHandleLook 对话框

IMPLEMENT_DYNAMIC(CDlgObjectHandleLook, CDialogEx)

CDlgObjectHandleLook::CDlgObjectHandleLook(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_OBJECT_PRODU_LOOK, pParent)
{

}

CDlgObjectHandleLook::~CDlgObjectHandleLook()
{
}

void CDlgObjectHandleLook::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OBJECT_HANDLE, m_list_objectHandle);
}


BEGIN_MESSAGE_MAP(CDlgObjectHandleLook, CDialogEx)
END_MESSAGE_MAP()


// CDlgObjectHandleLook 消息处理程序


BOOL CDlgObjectHandleLook::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list_objectHandle.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_objectHandle.InsertColumn(0, L"函数名", 0, 150);
	m_list_objectHandle.InsertColumn(1, L"函数地址", 0, 130);
	m_list_objectHandle.InsertColumn(2, L"所属模块", 0, 260);

	GetObjectHandle(g_index);

	return TRUE;
}

void CDlgObjectHandleLook::InsertObjeceHandleInfo(CString HandleName, ULONG64 addr, CHAR* ImgPath)
{
	CString str;
	CString csSysroot;
	CString szTrim = L"\\??\\";
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);
	ULONG nItem = m_list_objectHandle.GetItemCount();
	str = HandleName;
	m_list_objectHandle.InsertItem(nItem, str);
	//AfxMessageBox(L"111");

	str.Format(L"");
	if (addr != NULL)
		str.Format(L"0x%p", addr);
	m_list_objectHandle.SetItemText(nItem, 1, str);


	str.Format(L"%S", ImgPath);
	str = str.TrimLeft(szTrim);
	str.Replace(L"SystemRoot", csSysroot);
	m_list_objectHandle.SetItemText(nItem, 2, str);
}

void CDlgObjectHandleLook::GetObjectHandle(UCHAR Index)
{
	CString str;
	UCHAR i = Index;

	if (!ARKDeviceIoControl(IOCTL_GetObjectProcedureByIndex, &i, sizeof(UCHAR), &pObjHandleInfo, sizeof(OBJECT_PROCEDURE_INFO)))
	{
		AfxMessageBox(L"错误");
		return;
	}

	str.Format(L"DumpProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.DumpProcedrure, pObjHandleInfo.DumpImgPath);

	str.Format(L"OpenProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.OpenProcedure, pObjHandleInfo.OpenImgPath);
	
	str.Format(L"CloseProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.CloseProcedrure, pObjHandleInfo.CloseImgPath);

	str.Format(L"DeleteProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.DeleteProcedure, pObjHandleInfo.DeleteImgPath);

	str.Format(L"ParseProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.ParseProcedure, pObjHandleInfo.ParseImgPath);

	str.Format(L"SecurityProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.SecurityProcedure, pObjHandleInfo.SecurityImgPath);

	str.Format(L"QueryNameProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.QueryNameProcedure, pObjHandleInfo.QueryNameImgPath);

	str.Format(L"OkayToCloseProcedure");
	InsertObjeceHandleInfo(str, pObjHandleInfo.OkayToCloseProcedure, pObjHandleInfo.OkayToCloseImgPath);

	str.Format(L"[%s]的Object函数", g_ObjectName);
	this->SetWindowTextW(str);
}
