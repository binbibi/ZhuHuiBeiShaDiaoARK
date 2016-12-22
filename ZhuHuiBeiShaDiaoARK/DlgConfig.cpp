// DlgConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgConfig.h"
#include "afxdialogex.h"


// CDlgConfig 对话框

IMPLEMENT_DYNAMIC(CDlgConfig, CPropertyPage)

CDlgConfig::CDlgConfig()
	: CPropertyPage(CDlgConfig::IDD)
{

}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CPropertyPage)
END_MESSAGE_MAP()


// CDlgConfig 消息处理程序
