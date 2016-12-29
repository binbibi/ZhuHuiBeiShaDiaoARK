#pragma once
#include "afxcmn.h"


// CDlgIOTimer 对话框

class CDlgIOTimer : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgIOTimer)

public:
	CDlgIOTimer();
	virtual ~CDlgIOTimer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_IOTIMER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_iotimer;
	virtual BOOL OnInitDialog();
	void EnumIOTimer();
};
