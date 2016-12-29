// DlgDpcTimer.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgDpcTimer.h"
#include "afxdialogex.h"

typedef struct _DPC_TIMER_
{
	ULONG Count;
	ULONG Period;// 周期
	ULONG_PTR TimerObject;
	ULONG_PTR TimeRoutine;
	ULONG_PTR Dpc;
	CHAR	  ImgPath[MAX_PATH];
}DPC_TIMER, *PDPC_TIMER;


// CDlgDpcTimer 对话框

IMPLEMENT_DYNAMIC(CDlgDpcTimer, CPropertyPage)

CDlgDpcTimer::CDlgDpcTimer()
	: CPropertyPage(IDD_DLG_DPCTIMER)
{

}

CDlgDpcTimer::~CDlgDpcTimer()
{
}

void CDlgDpcTimer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DPC_TIMER, m_list_DpcTimer);
}


BEGIN_MESSAGE_MAP(CDlgDpcTimer, CPropertyPage)
END_MESSAGE_MAP()


// CDlgDpcTimer 消息处理程序


BOOL CDlgDpcTimer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_list_DpcTimer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_DpcTimer.InsertColumn(0, L"KDPC", 0, 130);
	m_list_DpcTimer.InsertColumn(1, L"定时器对象", 0, 130);
	m_list_DpcTimer.InsertColumn(2, L"周期(ms)", 0, 75);
	m_list_DpcTimer.InsertColumn(3, L"入口地址", 0, 130);
	m_list_DpcTimer.InsertColumn(4, L"所在模块", 0, 350);

	EnumDpcTimer();
	return TRUE;
}


void CDlgDpcTimer::EnumDpcTimer()
{
	PDPC_TIMER pDpc = NULL;
	CString szTrim = L"\\??\\";
	CString csSysroot;
	CString str;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	WCHAR	szFileInfo[_MAX_PATH] = { 0 };

	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);

	pDpc = (PDPC_TIMER) malloc(sizeof(DPC_TIMER) * 500);
	if (pDpc == NULL)
		return;

	if (!ARKDeviceIoControl(IOCTL_ENUMMDPCTIMER, NULL, 0, pDpc, sizeof(DPC_TIMER) * 500))
	{
		free(pDpc);
		pDpc = NULL;
		return;
	}

	ULONG Count = 0;
	ULONG item = 0;

	Count = pDpc[0].Count;

	for (size_t i = 0; i < Count; i++)
	{
		item = m_list_DpcTimer.GetItemCount();

		str.Format(L"0x%p", pDpc[i].Dpc);
		m_list_DpcTimer.InsertItem(item, str);

		str.Format(L"0x%p", pDpc[i].TimerObject);
		m_list_DpcTimer.SetItemText(item, 1, str);

		str.Format(L"%d", pDpc[i].Period);
		m_list_DpcTimer.SetItemText(item, 2, str);

		str.Format(L"0x%p", pDpc[i].TimeRoutine);
		m_list_DpcTimer.SetItemText(item, 3, str);

		str.Format(L"%S", pDpc[i].ImgPath);
		str = str.TrimLeft(szTrim);
		str.Replace(L"SystemRoot", csSysroot);
		m_list_DpcTimer.SetItemText(item, 4, str);
	}

	str.Format(L"[DPC定时器]当前系统共有 %d 个Dpc定时器", Count);
	AfxGetMainWnd()->SetWindowTextW(str);

	free(pDpc);
	pDpc = NULL;
}
