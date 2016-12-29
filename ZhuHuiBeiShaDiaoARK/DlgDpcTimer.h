#pragma once
#include "afxcmn.h"


// CDlgDpcTimer 对话框

class CDlgDpcTimer : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgDpcTimer)

public:
	CDlgDpcTimer();
	virtual ~CDlgDpcTimer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DPCTIMER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_DpcTimer;
	virtual BOOL OnInitDialog();
	void EnumDpcTimer();
};
