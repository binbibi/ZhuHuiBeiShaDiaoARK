#pragma once
#include "afxcmn.h"
#include "MyListCtrl.h"


// CDlgSSDT 对话框

class CDlgSSDT : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgSSDT)

public:
	CDlgSSDT();
	virtual ~CDlgSSDT();

// 对话框数据
	enum { IDD = IDD_DIALOG_SSDT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//CMyListCtrl m_list_ssdt;
	CListCtrl m_list_ssdt;
	virtual BOOL OnInitDialog();
	static DWORD m_SortColum;  
	static BOOL m_bAs; 
	static int CALLBACK MyListCompar(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void EnumSSDT(void);
	afx_msg void MyListCompar(NMHDR *pNMHDR, LRESULT *pResult);
};
