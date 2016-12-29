#pragma once
#include "afxcmn.h"


// CDlgSystemCall 对话框

class CDlgSystemCall : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgSystemCall)

public:
	CDlgSystemCall();
	virtual ~CDlgSystemCall();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CALLBACKS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_syscallbacks;
	virtual BOOL OnInitDialog();
	void EnumSystemCallbacks();
};
