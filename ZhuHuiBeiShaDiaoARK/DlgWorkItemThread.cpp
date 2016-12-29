// DlgWorkItemThread.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgWorkItemThread.h"
#include "afxdialogex.h"


typedef struct _WORK_THRAD_INFO
{
	ULONG	Count;
	ULONG	ThreadId;
	ULONG	WorkType;
	ULONG64	eThread;
	ULONG64	ThreadRoutine;
	CHAR	ImgPath[MAX_PATH];
}WORK_THREAD_INFO, *PWORK_THREAD_INFO;

static char *WORK_TYPE[4] =
{
	"CriticalWorkQueue",//13
	"DelayedWorkQueue",//14
	"HyperCriticalWorkQueue"//15
};

// CDlgWorkItemThread 对话框

IMPLEMENT_DYNAMIC(CDlgWorkItemThread, CPropertyPage)

CDlgWorkItemThread::CDlgWorkItemThread()
	: CPropertyPage(IDD_DLG_WORKITEMTHREAD)
{

}

CDlgWorkItemThread::~CDlgWorkItemThread()
{
}

void CDlgWorkItemThread::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WORK_ITEM_THREAD, m_list_workitem);
}


BEGIN_MESSAGE_MAP(CDlgWorkItemThread, CPropertyPage)
END_MESSAGE_MAP()


// CDlgWorkItemThread 消息处理程序


BOOL CDlgWorkItemThread::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	m_list_workitem.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_workitem.InsertColumn(0, L"ETHREAD", 0, 130);
	m_list_workitem.InsertColumn(1, L"线程ID", 0, 80);
	m_list_workitem.InsertColumn(2, L"优先级", LVCFMT_CENTER, 150);
	m_list_workitem.InsertColumn(3, L"线程入口", 0, 130);
	m_list_workitem.InsertColumn(4, L"当前地址所在模块", 0, 300);
	EnumWorkItemThread();
	return TRUE;
}


void CDlgWorkItemThread::EnumWorkItemThread()
{
	CString szTrim = L"\\??\\";
	CString csSysroot;
	CString str;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	//WCHAR	*szImgPath = NULL;
	PWORK_THREAD_INFO pWorkInfo = NULL;


	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);

	pWorkInfo = (PWORK_THREAD_INFO)malloc(sizeof(WORK_THREAD_INFO)*300);

	if (pWorkInfo == NULL)
		return;

	if (!ARKDeviceIoControl(IOCTL_ENUMWORKITEMTHREAD, NULL, 0, pWorkInfo, sizeof(WORK_THREAD_INFO) * 300))
	{
		free(pWorkInfo);
		pWorkInfo = NULL;
		return;
	}

	ULONG Count = 0;
	ULONG item = 0;
	Count = pWorkInfo[0].Count;

	for (size_t i = 0; i < Count; i++)
	{
		item = m_list_workitem.GetItemCount();

		str.Format(L"0x%p", pWorkInfo[i].eThread);
		m_list_workitem.InsertItem(item, str);

		str.Format(L"%d", pWorkInfo[i].ThreadId);
		m_list_workitem.SetItemText(item, 1, str);

		str.Format(L"%S", WORK_TYPE[pWorkInfo[i].WorkType]);
		m_list_workitem.SetItemText(item, 2, str);

		if (pWorkInfo[i].ThreadRoutine != 0)
		{
			str.Format(L"0x%p", pWorkInfo[i].ThreadRoutine);
			m_list_workitem.SetItemText(item, 3, str);

			//szImgPath = charToWchar(pWorkInfo[i].ImgPath);
			str.Format(L"%S", pWorkInfo[i].ImgPath);
			str = str.TrimLeft(szTrim);
			str.Replace(L"SystemRoot", csSysroot);
			m_list_workitem.SetItemText(item, 4, str);

			//free(szImgPath);
			//szImgPath = NULL;
		}

	}

	free(pWorkInfo);
	pWorkInfo = NULL;
	str.Format(L"当前系统共有 %d 个工作线程", Count);
	AfxGetMainWnd()->SetWindowTextW(str);
}
