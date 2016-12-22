#pragma once
#include "afxdlgs.h"
#include "DlgSSDT.h"
#include "DlgSSSDT.h"


// CDlgKernelHook 对话框

class CDlgKernelHook : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgKernelHook)

public:
	CDlgKernelHook();
	virtual ~CDlgKernelHook();

// 对话框数据
	enum { IDD = IDD_DLG_KERNEL_HOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CPropertySheet		m_Sheet_KernelHook;
	CDlgSSDT			m_DlgSSDT;
	CDlgSSSDT			m_DlgSSSDT;
	virtual BOOL OnInitDialog();
	afx_msg void UloadDelFile();
};
