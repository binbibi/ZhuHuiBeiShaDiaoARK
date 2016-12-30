// DlgKernelHook.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgKernelHook.h"
#include "afxdialogex.h"



// CDlgKernelHook 对话框

IMPLEMENT_DYNAMIC(CDlgKernelHook, CPropertyPage)

CDlgKernelHook::CDlgKernelHook()
	: CPropertyPage(CDlgKernelHook::IDD)
{

}

CDlgKernelHook::~CDlgKernelHook()
{
}

void CDlgKernelHook::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgKernelHook, CPropertyPage)
	//ON_COMMAND(ID_32800, &CDlgKernelHook::UloadDelFile)
END_MESSAGE_MAP()


// CDlgKernelHook 消息处理程序


BOOL CDlgKernelHook::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_Sheet_KernelHook.AddPage(&m_DlgSSDT);
	m_Sheet_KernelHook.AddPage(&m_DlgSSSDT);
	m_Sheet_KernelHook.AddPage(&m_DlgObjectHook);
	m_Sheet_KernelHook.AddPage(&m_DlgSystemCall);
	m_Sheet_KernelHook.AddPage(&m_DlgObCallbacks);
	m_Sheet_KernelHook.AddPage(&m_DlgWorkItem);
	m_Sheet_KernelHook.AddPage(&m_DlgMinifilter);
	m_Sheet_KernelHook.AddPage(&m_DlgDpcTimer);
	m_Sheet_KernelHook.AddPage(&m_DlgIoTimer);
	m_Sheet_KernelHook.AddPage(&m_DlgFilterDrv);
	
	m_Sheet_KernelHook.Create(this, WS_CHILD | WS_VISIBLE, WS_EX_CONTROLPARENT);
	m_Sheet_KernelHook.SetWindowPos(NULL,-6, -6, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	return TRUE;
}

