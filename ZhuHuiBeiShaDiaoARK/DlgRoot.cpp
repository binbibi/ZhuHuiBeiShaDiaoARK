// DlgRoot.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgRoot.h"
#include "afxdialogex.h"


// CDlgRoot 对话框

IMPLEMENT_DYNAMIC(CDlgRoot, CPropertyPage)

CDlgRoot::CDlgRoot()
	: CPropertyPage(CDlgRoot::IDD)
{

}

CDlgRoot::~CDlgRoot()
{
}

void CDlgRoot::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRoot, CPropertyPage)
END_MESSAGE_MAP()


// CDlgRoot 消息处理程序
