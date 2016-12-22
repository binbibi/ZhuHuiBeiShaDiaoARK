// DlgAuthor.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgAuthor.h"
#include "afxdialogex.h"
#include "ZhuHuiBeiShaDiaoARKDlg.h"
#include "DLGPROCESS.h"

// CDlgAuthor 对话框

IMPLEMENT_DYNAMIC(CDlgAuthor, CPropertyPage)

CDlgAuthor::CDlgAuthor()
	: CPropertyPage(CDlgAuthor::IDD)
	, m_InputPid(0)
{

}

CDlgAuthor::~CDlgAuthor()
{
}

void CDlgAuthor::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PID, m_InputPid);
}


BEGIN_MESSAGE_MAP(CDlgAuthor, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_IODRVIER, &CDlgAuthor::OnBnClickedButtonIodrvier)
	ON_BN_CLICKED(IDC_BUTTON_STRINGTEST, &CDlgAuthor::OnBnClickedButtonStringtest)
	ON_BN_CLICKED(IDC_BUTTON_STRINGTEST2, &CDlgAuthor::OnBnClickedButtonStringtest2)
	ON_BN_CLICKED(IDC_BUTTON_CHAR, &CDlgAuthor::OnBnClickedButtonChar)
	ON_BN_CLICKED(IDC_BUTTON_CHAR2, &CDlgAuthor::OnBnClickedButtonChar2)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgAuthor::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_GETEPROCESS_TEST, &CDlgAuthor::OnBnClickedGeteprocessTest)
END_MESSAGE_MAP()


// CDlgAuthor 消息处理程序

void CDlgAuthor::OnBnClickedButtonIodrvier()
{
	BOOL		IoSuccess = FALSE;
	ULONG		InBuffer[1];
	CString		OutBuff;
	DWORD		RetutnSize;
	PVOID       pBuff = malloc(260);
	InBuffer[0] = 4096;
	IoSuccess = DeviceIoControl(g_device,IOCTL_TEST, &InBuffer, 4, pBuff, 260,&RetutnSize,NULL);
	if(IoSuccess==FALSE)
	{
		AfxMessageBox(L"失败");
	}
	OutBuff.Format(L"%s",(wchar_t*)pBuff);
	AfxMessageBox(OutBuff);
	free(pBuff);
}


BOOL CDlgAuthor::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	return TRUE; 
}


void CDlgAuthor::OnBnClickedButtonStringtest()
{
	//传入字符串
	BOOL		IoSuccess = FALSE;	
	WCHAR		*string = (WCHAR*)malloc(260);
	DWORD		RetutnSize;
	PVOID       pBuff = malloc(260);
	StrCpyW(string,L"111111111111");
	IoSuccess = DeviceIoControl(g_device,IOCTL_TEST2, string, 260, pBuff, 260,&RetutnSize,NULL);
	if(IoSuccess==FALSE)
	{
		AfxMessageBox(L"失败");
	}
	free(pBuff);
	free(string);
}


void CDlgAuthor::OnBnClickedButtonStringtest2()
{
	//传入字符串
	BOOL		IoSuccess = FALSE;	
	WCHAR		*string = (WCHAR*)malloc(260);
	DWORD		RetutnSize;
	PVOID       pBuff = malloc(260);

	StrCpyW(string,L"111111222222");
	IoSuccess = DeviceIoControl(g_device,IOCTL_TEST2, string, 260, pBuff, 260,&RetutnSize,NULL);

	if(IoSuccess==FALSE)
	{
		AfxMessageBox(L"失败");
	}

	free(pBuff);
	free(string);
}


void CDlgAuthor::OnBnClickedButtonChar()
{
	BOOL		IoSuccess = FALSE;
	CHAR		*chStr = (CHAR*)malloc(260); 
	DWORD		RetueSize;
	PVOID		pOutBuff = malloc(260);

	memset(chStr,'\0',260);
	//strcpy(chStr,"char test!!!");
	StrCpyA(chStr,"char test!!!");
	IoSuccess = DeviceIoControl(g_device,IOCTL_TEST3,chStr,260,pOutBuff,260,&RetueSize,NULL);

	if(IoSuccess == FALSE)
	{
		AfxMessageBox(L"失败");
	}

	free(pOutBuff);
	free(chStr);
}


void CDlgAuthor::OnBnClickedButtonChar2()
{
	BOOL		IoSuccess = FALSE;
	CHAR		*chStr = (CHAR*)malloc(260); 
	DWORD		RetueSize;
	PVOID		pOutBuff = malloc(260);

	memset(chStr,'\0',260);
	StrCpyA(chStr,"Input Char !!!!!!!");
	IoSuccess = DeviceIoControl(g_device,IOCTL_TEST3,chStr,260,pOutBuff,260,&RetueSize,NULL);

	if(IoSuccess == FALSE)
	{
		AfxMessageBox(L"失败");
	}

	free(pOutBuff);
	free(chStr);
}

//需要管理员权限
void CDlgAuthor::OnBnClickedButton2()
{
	CString str;
	str = L"D:\\360data\\重要数据\\桌面\\新建文本文档.txt";
	
	MoveFileExW(str,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
	str = L"C:\\1.txt";
	
	MoveFileExW(str,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
}


void CDlgAuthor::OnBnClickedGeteprocessTest()
{
	CDLGPROCESS dlg;
	CString str;
	UpdateData();
	str = dlg.GetEprocess(m_InputPid);
	AfxMessageBox(str);
}
