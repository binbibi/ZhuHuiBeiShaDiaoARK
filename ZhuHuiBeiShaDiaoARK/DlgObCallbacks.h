#pragma once
#include "afxcmn.h"


// CDlgObCallbacks 对话框

class CDlgObCallbacks : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgObCallbacks)

public:
	CDlgObCallbacks();
	virtual ~CDlgObCallbacks();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_OBCALLBACKS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_obCallbacks;
	virtual BOOL OnInitDialog();
	void EnumObCallbacks();
};
