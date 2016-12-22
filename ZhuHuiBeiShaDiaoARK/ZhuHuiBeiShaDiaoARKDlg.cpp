
// ZhuHuiBeiShaDiaoARKDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "ZhuHuiBeiShaDiaoARKDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnProcess32790();
	//afx_msg void UnloadDelFile();
	afx_msg void OnLookWindows();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//ON_COMMAND(ID_PROCESS_32790, &CAboutDlg::OnProcess32790)
	//ON_COMMAND(ID_32800, &CAboutDlg::UnloadDelFile)
END_MESSAGE_MAP()


// CZhuHuiBeiShaDiaoARKDlg 对话框



CZhuHuiBeiShaDiaoARKDlg::CZhuHuiBeiShaDiaoARKDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZhuHuiBeiShaDiaoARKDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZhuHuiBeiShaDiaoARKDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CZhuHuiBeiShaDiaoARKDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CZhuHuiBeiShaDiaoARKDlg 消息处理程序

BOOL CZhuHuiBeiShaDiaoARKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//查看驱动服务是否存在
	g_device = CreateFile(L"\\\\.\\MyDriver",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(g_device == INVALID_HANDLE_VALUE)
	{
		MessageBox(L"CreateFile is null ,May be driver load failed !",L"Error!");
		DestroyWindow();
	}

	this->m_sheet.AddPage(&m_DlgPro);
	this->m_sheet.AddPage(&m_DlgDriver);
	this->m_sheet.AddPage(&m_DlgKernel);
	this->m_sheet.AddPage(&m_DlgKerHook);
	this->m_sheet.AddPage(&m_DlgR3Hook);
	this->m_sheet.AddPage(&m_DlgNetWork);
	this->m_sheet.AddPage(&m_DlgFile);
	this->m_sheet.AddPage(&m_DlgSystemStart);
	this->m_sheet.AddPage(&m_DlgFunction);
	this->m_sheet.AddPage(&m_DlgRootFunc);
	this->m_sheet.AddPage(&m_DlgConfig);
	this->m_sheet.AddPage(&m_DlgAuthor);

	m_sheet.Create(this, WS_CHILD | WS_VISIBLE, WS_EX_CONTROLPARENT);

	/*RECT rect;
	m_sheet.GetWindowRect(&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;*/

	m_sheet.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CZhuHuiBeiShaDiaoARKDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CZhuHuiBeiShaDiaoARKDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CZhuHuiBeiShaDiaoARKDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






BOOL CZhuHuiBeiShaDiaoARKDlg::DestroyWindow()
{
	if(g_device != INVALID_HANDLE_VALUE)
		CloseHandle(g_device);
	return CDialogEx::DestroyWindow();
}
