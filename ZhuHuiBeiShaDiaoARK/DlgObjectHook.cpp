// DlgObjectHook.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgObjectHook.h"
#include "afxdialogex.h"
#include "DlgObjectHandleLook.h"

typedef struct _OBJECT_TYPE_INFO
{
	// 是否支持回掉
	BOOLEAN	bSupportsObjectCallbacks;
	// 第0个才用到 标记有多少个对象
	ULONG	Count;
	// index
	ULONG	index;
	// hehe
	ULONG	ValidAccessMask;
	ULONG64	ObjectAddr;
	/*ULONG64	DumpProcedrure;
	ULONG64	OpenProcedure;
	ULONG64	CloseProcedrure;
	ULONG64	DeleteProcedure;
	ULONG64	ParseProcedure;
	ULONG64	SecurityProcedure;
	ULONG64	QueryNameProcedure;
	ULONG64	OkayToCloseProcedure;*/
	CHAR	imgPath[MAX_PATH];
	WCHAR	ObjectName[MAX_PATH];
}OBJECT_TYPE_INFO, *POBJECT_TYPE_INFO;

// CDlgObjectHook 对话框

IMPLEMENT_DYNAMIC(CDlgObjectHook, CPropertyPage)

CDlgObjectHook::CDlgObjectHook()
	: CPropertyPage(IDD_DLG_OBJECTLOOK)
{

}

CDlgObjectHook::~CDlgObjectHook()
{
}

void CDlgObjectHook::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OBJECTHOOK, m_list_object);
}


BEGIN_MESSAGE_MAP(CDlgObjectHook, CPropertyPage)
	ON_COMMAND(ID_OBJECT_32835, &CDlgObjectHook::OnObject32835)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OBJECTHOOK, &CDlgObjectHook::OnNMRClickListObjecthook)
END_MESSAGE_MAP()


// CDlgObjectHook 消息处理程序


BOOL CDlgObjectHook::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 这里先查询这些 看回调函数右键菜单实现
	m_list_object.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_object.InsertColumn(0, L"index", 0, 50);
	m_list_object.InsertColumn(1, L"对象类型", 0, 100);
	m_list_object.InsertColumn(2, L"Object地址", LVCFMT_CENTER, 130);
	m_list_object.InsertColumn(3, L"回调支持", LVCFMT_CENTER, 100);
	m_list_object.InsertColumn(4, L"掩码", LVCFMT_CENTER, 70);
	m_list_object.InsertColumn(5, L"所在模块", 0, 200);
	
	EnumObjectType();
	/*
	// 是否支持回掉
	BOOLEAN	bSupportsObjectCallbacks;
	// 第0个才用到 标记有多少个对象
	ULONG	Count;
	// index
	ULONG	index;
	// hehe
	ULONG	ValidAccessMask;
	ULONG64	DumpProcedrure;
	ULONG64	OpenProcedure;
	ULONG64	CloseProcedrure;
	ULONG64	DeleteProcedure;
	ULONG64	ParseProcedure;
	ULONG64	SecurityProcedure;
	ULONG64	QueryNameProcedure;
	ULONG64	OkayToCloseProcedure;
	WCHAR	ObjectName[MAX_PATH];
	*/
	return TRUE;
}


void CDlgObjectHook::EnumObjectType()
{
	CString str;
	POBJECT_TYPE_INFO pObjectInfo = NULL;
	pObjectInfo = (POBJECT_TYPE_INFO)malloc(sizeof(OBJECT_TYPE_INFO) * 50);

	memset(pObjectInfo, 0, sizeof(OBJECT_TYPE_INFO) * 50);

	if (pObjectInfo == NULL)
		return;

	if (!ARKDeviceIoControl(IOCTL_GetObjectInfo, NULL, 0, pObjectInfo, sizeof(OBJECT_TYPE_INFO) * 50))
	{
		free(pObjectInfo);
		pObjectInfo = NULL;
		return;
	}


	ULONG count = pObjectInfo[0].Count;
	ULONG nItem = 0;

	for (ULONG i = 0; i < count; i++)
	{
		nItem = m_list_object.GetItemCount();

		str.Format(L"%d", pObjectInfo[i].index);
		m_list_object.InsertItem(nItem, str);

		str.Format(L"%s", pObjectInfo[i].ObjectName);
		m_list_object.SetItemText(nItem, 1, str);

		str.Format(L"0x%p", pObjectInfo[i].ObjectAddr);
		m_list_object.SetItemText(nItem, 2, str);

		if (pObjectInfo[i].bSupportsObjectCallbacks)
			m_list_object.SetItemText(nItem, 3, L"Support");
		else
			m_list_object.SetItemText(nItem, 3, L"No Support");

		str.Format(L"0x%X", pObjectInfo[i].ValidAccessMask);

		m_list_object.SetItemText(nItem, 4, str);

		str.Format(L"%S", pObjectInfo[i].imgPath);

		m_list_object.SetItemText(nItem, 5, str);
	}

	free(pObjectInfo);

}

CString CDlgObjectHook::GetItemInfo(int subItem)
{
	DWORD nItemSelct = m_list_object.GetNextItem(-1, LVIS_SELECTED);
	if (nItemSelct != -1)
		return m_list_object.GetItemText(nItemSelct, subItem);
	return NULL;
}

// 查看对象函数
void CDlgObjectHook::OnObject32835()
{
	CString str;
	str = GetItemInfo(0);
	g_index = _ttoi(str);
	g_ObjectName = GetItemInfo(1);
	CDlgObjectHandleLook Dlg;
	Dlg.DoModal();
}


void CDlgObjectHook::OnNMRClickListObjecthook(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	POINT point;
	HMENU hMenu, hSubMenu;
	GetCursorPos(&point); //鼠标位置
	hMenu = LoadMenu(NULL,
		MAKEINTRESOURCE(IDR_MENU3)); // 加载菜单
	hSubMenu = GetSubMenu(hMenu, 0);//得到子菜单(因为弹出式菜单是子菜单)

	SetMenuDefaultItem(hSubMenu, -1, FALSE);//设置缺省菜单项,-1为无缺省项
	SetForegroundWindow(); // 激活窗口并置前

	TrackPopupMenu(hSubMenu, 0,
		point.x, point.y, 0, m_hWnd, NULL);

	*pResult = 0;
}
