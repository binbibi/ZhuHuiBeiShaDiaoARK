// DlgSystemCall.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgSystemCall.h"
#include "afxdialogex.h"


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

"LoadImage",//0
"CreateProcess",//1
"CreateThread",//2
"CmpRegister",//3
"ShutDowdn",//4
// EnumType:5	回调类型不变
"BugCheck",//5
"BugCheckReason"//6
*/


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

// CDlgSystemCall 对话框

IMPLEMENT_DYNAMIC(CDlgSystemCall, CPropertyPage)

CDlgSystemCall::CDlgSystemCall()
	: CPropertyPage(IDD_DLG_CALLBACKS)
{

}

CDlgSystemCall::~CDlgSystemCall()
{
}

void CDlgSystemCall::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_syscallbacks);
}


BEGIN_MESSAGE_MAP(CDlgSystemCall, CPropertyPage)
END_MESSAGE_MAP()


// CDlgSystemCall 消息处理程序


BOOL CDlgSystemCall::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_list_syscallbacks.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_syscallbacks.InsertColumn(0, L"回调入口", LVCFMT_CENTER, 130);
	m_list_syscallbacks.InsertColumn(1, L"类型", LVCFMT_CENTER, 130);
	m_list_syscallbacks.InsertColumn(2, L"路径", 0, 250);
	m_list_syscallbacks.InsertColumn(3, L"文件厂商", LVCFMT_CENTER, 160);
	m_list_syscallbacks.InsertColumn(4, L"备注", LVCFMT_CENTER, 100);
	EnumSystemCallbacks();
	return TRUE;
}


void CDlgSystemCall::EnumSystemCallbacks()
{
	ULONG EnumType = 0;
	ULONG count = 0;
	ULONG item = 0;
	PNOTIFY_INFO pNotify = NULL;
	CString str;
	CString szTrim = L"\\??\\";
	CString csSysroot;
	CString szWindowsText;
	CString tm;
	size_t i = 0;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	WCHAR	szFileInfo[_MAX_PATH] = { 0 };

	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);

	pNotify = (PNOTIFY_INFO)malloc(sizeof(NOTIFY_INFO)* 100);

	if (pNotify == NULL)
		return;

	/*

	"LoadImage",//0
	"CreateProcess",//1
	"CreateThread",//2
	"CmpRegister",//3
	"ShutDowdn",//4
	// EnumType:5	回调类型不变
	"BugCheck",//5
	"BugCheckReason"//6
	*/

	for (EnumType = 0; EnumType  < 6; EnumType++)
	{
		if (ARKDeviceIoControl(IOCTL_ENUMCALLBACKS, &EnumType, 4, pNotify, sizeof(NOTIFY_INFO) * 100))
		{
			count = pNotify[0].Count;

			for (i = 0; i < count; i++)
			{
				item = m_list_syscallbacks.GetItemCount();

				str.Format(L"0x%p", pNotify[i].CallbacksAddr);
				m_list_syscallbacks.InsertItem(item, str);


				str.Format(L"%S", CallbacksType[pNotify[i].CallbackType]);
				m_list_syscallbacks.SetItemText(item, 1, str);

				str.Format(L"%S", pNotify[i].ImgPath);
				str = str.TrimLeft(szTrim);
				str.Replace(L"SystemRoot", csSysroot);
				m_list_syscallbacks.SetItemText(item, 2, str);

				GetFileInfo(str.GetBuffer(), szFileInfo);
				str.Format(L"%s", szFileInfo);

				m_list_syscallbacks.SetItemText(item, 3, str);

				if (EnumType == 3)
				{
					str.Format(L"Cookie:0x%p", pNotify[i].Cookie);
					m_list_syscallbacks.SetItemText(item, 4, str);
				}

				memset(szFileInfo, 0, MAX_PATH);
			}

			tm.Format(L"%S:%d  ", CallbacksType[EnumType], count);

			szWindowsText = szWindowsText + tm;
			
			memset(szFileInfo, 0, MAX_PATH);
			count = 0;
			i = 0;
			memset(pNotify, 0, sizeof(NOTIFY_INFO) * 100);
		}
	}

	tm.Format(L"[回调查看器]当前系统:" + szWindowsText);
	AfxGetMainWnd()->SetWindowTextW(tm);
	free(pNotify);
	pNotify = NULL;

}
