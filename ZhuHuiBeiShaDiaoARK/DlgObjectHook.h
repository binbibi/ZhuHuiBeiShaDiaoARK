#pragma once
#include "afxcmn.h"


// CDlgObjectHook 对话框

class CDlgObjectHook : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgObjectHook)

public:
	CDlgObjectHook();
	virtual ~CDlgObjectHook();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_OBJECTLOOK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_list_object;
	void EnumObjectType();
	afx_msg void OnObject32835();
	CString GetItemInfo(int subItem);
	afx_msg void OnNMRClickListObjecthook(NMHDR *pNMHDR, LRESULT *pResult);
};
