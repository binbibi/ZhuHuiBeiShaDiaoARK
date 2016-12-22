// DlgThreadLooker.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgThreadLooker.h"
#include "afxdialogex.h"
#include "DLGPROCESS.h"
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#include   <Tlhelp32.h>  

// CDlgThreadLooker 对话框

HANDLE NtdllOpenThread(ACCESS_MASK AccessMask, BOOL bInheritHandle, DWORD dwThreadId)
{
	OBJECT_ATTRIBUTES oa;
	CLIENT_ID cid;
	memset(&oa,0x0,sizeof(oa));
	memset(&cid,0x0,sizeof(cid));
	HANDLE hThread = 0;
	oa.Length = sizeof(oa);
	cid.UniqueThread = (HANDLE)dwThreadId;
	ZwOpenThread(&hThread,AccessMask,&oa,&cid);
	return hThread;
}

PVOID GetTEB(HANDLE hThread)
{
	ULONG retlen=0;
	THREAD_BASIC_INFORMATION tbi;
	memset(&tbi,0,sizeof(tbi));
	long ntret = ZwQueryInformationThread(hThread,0,&tbi,sizeof(tbi),&retlen);
	if (ntret == 0)
		return tbi.TebBaseAddress;
	return NULL;
}

LONG GetPriority(HANDLE hThread)
{
	ULONG retlen=0;
	THREAD_BASIC_INFORMATION tbi;
	memset(&tbi,0,sizeof(tbi));
	long ntret=ZwQueryInformationThread(hThread,0,&tbi,sizeof(tbi),&retlen);
	if (ntret==0)
		return tbi.Priority;
	return 0;
}

ULONG64 GetThreadStartAddress(HANDLE hThread)
{
	ULONG retlen=0;
	ULONG64 tsa=0;
	long ntret=ZwQueryInformationThread(hThread,9,&tsa,sizeof(tsa),&retlen);
	if (ntret==0)
		return tsa;
	return 0;
}

IMPLEMENT_DYNAMIC(CDlgThreadLooker, CDialogEx)

CDlgThreadLooker::CDlgThreadLooker(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgThreadLooker::IDD, pParent)
{

}

CDlgThreadLooker::~CDlgThreadLooker()
{
}

void CDlgThreadLooker::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_THREADH_LOOK, m_list_Looker_Thread);
}


BEGIN_MESSAGE_MAP(CDlgThreadLooker, CDialogEx)
END_MESSAGE_MAP()


// CDlgThreadLooker 消息处理程序


BOOL CDlgThreadLooker::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//线程ID  ETHREAD TEB 优先级 线程入口 模块 线程状态
	m_list_Looker_Thread.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	m_list_Looker_Thread.InsertColumn(0,L"线程ID",0,50);
	m_list_Looker_Thread.InsertColumn(1,L"ETHREAD",0,125);
	m_list_Looker_Thread.InsertColumn(2,L"TEB",0,125);
	m_list_Looker_Thread.InsertColumn(3,L"优先级",LVCFMT_CENTER,55);
	m_list_Looker_Thread.InsertColumn(4,L"线程入口",0,130);
	m_list_Looker_Thread.InsertColumn(5,L"模块",0,70);
	m_list_Looker_Thread.InsertColumn(6,L"线程状态",LVCFMT_CENTER,70);
	m_list_Looker_Thread.InsertColumn(7,L"挂起状态",LVCFMT_CENTER,70);
	EnumThreadByPid(g_CurSelectPid);
	return TRUE;
}


void CDlgThreadLooker::EnumThreadByPid(DWORD pid)
{
	DWORD i= 0,tmppid = 0;
	HANDLE tmpTid = 0;
	WCHAR FileName[100] = {0};
	DWORD dwStatu = 0;
	CString str;
	CDLGPROCESS tem;
	HANDLE hProcess = tem.NtdllOpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,pid);
	if(hProcess <= 0)
		return;

	for (i = 8; i < 262144; i += 4)
	{
		tmpTid = NtdllOpenThread(THREAD_QUERY_INFORMATION,0,i);
		if(tmpTid != 0)
		{
			tmppid = GetProcessIdOfThread(tmpTid);
			if (tmppid == pid){
				int item = m_list_Looker_Thread.GetItemCount();
				str.Format(L"%d",i);
				m_list_Looker_Thread.InsertItem(item,str);
				str.Format(L"0xFFFFFFFFFFFFFFFF");
				m_list_Looker_Thread.SetItemText(item,1,str);
				str.Format(L"0x%p",GetTEB(tmpTid));
				m_list_Looker_Thread.SetItemText(item,2,str);
				str.Format(L"%d",GetPriority(tmpTid));
				m_list_Looker_Thread.SetItemText(item,3,str);
				str.Format(L"0x%p",GetThreadStartAddress(tmpTid));
				m_list_Looker_Thread.SetItemText(item,4,str);
				GetMappedFileName(hProcess,(LPVOID)GetThreadStartAddress(tmpTid),FileName,0x100);
				str = FileName;
				str = str.Mid(str.ReverseFind('\\')+1);
				m_list_Looker_Thread.SetItemText(item,5,str);
				GetExitCodeThread(tmpTid,&dwStatu);
				if(dwStatu == STILL_ACTIVE)
					m_list_Looker_Thread.SetItemText(item,6,L"等待");
				else
					m_list_Looker_Thread.SetItemText(item,6,L"终止");

				//线程挂起状态
				
			}
			CloseHandle(tmpTid);
		}
	}
	str.Format(L"[%s]Pid:%d 线程",g_CurSelectImage,g_CurSelectPid);
	this->SetWindowTextW(str);
	CloseHandle(hProcess);
}
