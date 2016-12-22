// DlgFile.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgFile.h"
#include "afxdialogex.h"


// CDlgFile 对话框

IMPLEMENT_DYNAMIC(CDlgFile, CPropertyPage)

CDlgFile::CDlgFile()
	: CPropertyPage(CDlgFile::IDD)
{

}

CDlgFile::~CDlgFile()
{
}

void CDlgFile::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgFile, CPropertyPage)
END_MESSAGE_MAP()


// CDlgFile 消息处理程序
