// DlgR3Hook.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgR3Hook.h"
#include "afxdialogex.h"


// CDlgR3Hook 对话框

IMPLEMENT_DYNAMIC(CDlgR3Hook, CPropertyPage)

CDlgR3Hook::CDlgR3Hook()
	: CPropertyPage(CDlgR3Hook::IDD)
{

}

CDlgR3Hook::~CDlgR3Hook()
{
}

void CDlgR3Hook::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgR3Hook, CPropertyPage)
END_MESSAGE_MAP()


// CDlgR3Hook 消息处理程序
