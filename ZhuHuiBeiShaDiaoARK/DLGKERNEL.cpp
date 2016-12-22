// DLGKERNEL.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DLGKERNEL.h"
#include "afxdialogex.h"


// CDLGKERNEL 对话框

IMPLEMENT_DYNAMIC(CDLGKERNEL, CPropertyPage)

CDLGKERNEL::CDLGKERNEL()
	: CPropertyPage(CDLGKERNEL::IDD)
{

}

CDLGKERNEL::~CDLGKERNEL()
{
}

void CDLGKERNEL::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLGKERNEL, CPropertyPage)
END_MESSAGE_MAP()


// CDLGKERNEL 消息处理程序
