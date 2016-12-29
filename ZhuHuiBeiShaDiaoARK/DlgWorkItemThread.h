#pragma once
#include "afxcmn.h"


// CDlgWorkItemThread 对话框

class CDlgWorkItemThread : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgWorkItemThread)

public:
	CDlgWorkItemThread();
	virtual ~CDlgWorkItemThread();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_WORKITEMTHREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_workitem;
	virtual BOOL OnInitDialog();
	void EnumWorkItemThread();
};
