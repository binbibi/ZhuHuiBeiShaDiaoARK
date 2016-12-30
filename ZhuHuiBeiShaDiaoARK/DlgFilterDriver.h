#pragma once
#include "afxcmn.h"


// CDlgFilterDriver 对话框

class CDlgFilterDriver : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgFilterDriver)

public:
	CDlgFilterDriver();
	virtual ~CDlgFilterDriver();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FILTER_DRIVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_filterDrv;
	virtual BOOL OnInitDialog();
	void EnumFilterDriver();
};
