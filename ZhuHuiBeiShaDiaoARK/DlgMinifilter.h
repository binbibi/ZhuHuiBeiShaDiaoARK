#pragma once
#include "afxcmn.h"


// CDlgMinifilter 对话框

class CDlgMinifilter : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgMinifilter)

public:
	CDlgMinifilter();
	virtual ~CDlgMinifilter();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MINIFILTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_minifilter;
	virtual BOOL OnInitDialog();
	void EnumMinifilter();
};
