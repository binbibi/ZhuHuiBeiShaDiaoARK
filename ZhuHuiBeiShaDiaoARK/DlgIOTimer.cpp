// DlgIOTimer.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgIOTimer.h"
#include "afxdialogex.h"

typedef struct _IO_TIMER_INFO
{
	ULONG	count;
	ULONG	status;
	ULONG64	IoTimer;
	ULONG64	DeviceObject;
	ULONG64	TimerRoutine;
	CHAR	ImgPath[MAX_PATH];
}IO_TIMER_INFO, *PIO_TIMER_INFO;
// CDlgIOTimer 对话框

IMPLEMENT_DYNAMIC(CDlgIOTimer, CPropertyPage)

CDlgIOTimer::CDlgIOTimer()
	: CPropertyPage(IDD_DLG_IOTIMER)
{

}

CDlgIOTimer::~CDlgIOTimer()
{
}

void CDlgIOTimer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IO_TIMER, m_list_iotimer);
}


BEGIN_MESSAGE_MAP(CDlgIOTimer, CPropertyPage)
END_MESSAGE_MAP()


// CDlgIOTimer 消息处理程序


BOOL CDlgIOTimer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_list_iotimer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_iotimer.InsertColumn(0, L"IO_TIMER", 0, 130);
	m_list_iotimer.InsertColumn(1, L"状态", LVCFMT_CENTER, 75);
	m_list_iotimer.InsertColumn(2, L"入口地址", 0, 130);
	m_list_iotimer.InsertColumn(3, L"设备对象", 0, 130);
	m_list_iotimer.InsertColumn(4, L"所在模块", 0, 250);

	EnumIOTimer();
	return TRUE;
}


void CDlgIOTimer::EnumIOTimer()
{
	CString szTrim = L"\\??\\";
	CString csSysroot;
	CString str;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	PIO_TIMER_INFO pIoTimerInfo = NULL;

	pIoTimerInfo = (PIO_TIMER_INFO)malloc(sizeof(IO_TIMER_INFO) * 100);
	if (pIoTimerInfo == NULL)
		return;

	RtlZeroMemory(pIoTimerInfo, sizeof(IO_TIMER_INFO) * 100);

	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);

	if (!ARKDeviceIoControl(IOCTL_ENUMIOTMER, NULL, 0, pIoTimerInfo, sizeof(IO_TIMER_INFO) * 100))
	{
		free(pIoTimerInfo);
		pIoTimerInfo = NULL;
		return;
	}

	ULONG count = 0;
	ULONG item = 0;

	count = pIoTimerInfo[0].count;
	
	for (size_t i = 0; i < count; i++)
	{
		item = m_list_iotimer.GetItemCount();

		str.Format(L"0x%p", pIoTimerInfo[i].IoTimer);
		m_list_iotimer.InsertItem(item, str);

		m_list_iotimer.SetItemText(item, 1, pIoTimerInfo[i].status ? L"启动": L"暂停");

		str.Format(L"0x%p", pIoTimerInfo[i].TimerRoutine);
		m_list_iotimer.SetItemText(item, 2, str);

		str.Format(L"0x%p", pIoTimerInfo[i].DeviceObject);
		m_list_iotimer.SetItemText(item, 3, str);

		str.Format(L"%S", pIoTimerInfo[i].ImgPath);
		str = str.TrimLeft(szTrim);
		str.Replace(L"SystemRoot", csSysroot);

		m_list_iotimer.SetItemText(item, 4, str);

	}

	free(pIoTimerInfo);
	pIoTimerInfo = NULL;

	str.Format(L"[IO定时器]当前系统共有 %d 个IO定时器", count);

	AfxGetMainWnd()->SetWindowTextW(str);


}
