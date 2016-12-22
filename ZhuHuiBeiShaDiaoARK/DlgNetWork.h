#pragma once
#include "afxcmn.h"


// CDlgNetWork 对话框

class CDlgNetWork : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgNetWork)

public:
	CDlgNetWork();
	virtual ~CDlgNetWork();

// 对话框数据
	enum { IDD = IDD_DLG_NETWORK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List_NetWorkLook;
	virtual BOOL OnInitDialog();
	void EnumTcp(void);
	void EnumUdp(void);
	void GetTcpState(ULONG dwState, char* str);
	ULONG m_tcp_count;
	ULONG m_udp_count;
	WCHAR* GetProcessPath(DWORD pid,WCHAR* szPath);
};
