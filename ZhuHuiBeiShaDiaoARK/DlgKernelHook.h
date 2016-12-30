#pragma once
#include "afxdlgs.h"
#include "DlgSSDT.h"
#include "DlgSSSDT.h"
#include "DlgObjectHook.h"
#include "DlgSystemCall.h"
#include "DlgObCallbacks.h"
#include "DlgWorkItemThread.h"
#include "DlgMinifilter.h"
#include "DlgDpcTimer.h"
#include "DlgIOTimer.h"
#include "DlgFilterDriver.h"


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
	CDlgObjectHook		m_DlgObjectHook;
	CDlgSystemCall		m_DlgSystemCall;
	CDlgObCallbacks		m_DlgObCallbacks;
	CDlgWorkItemThread	m_DlgWorkItem;
	CDlgMinifilter		m_DlgMinifilter;
	CDlgDpcTimer		m_DlgDpcTimer;
	CDlgIOTimer			m_DlgIoTimer;
	CDlgFilterDriver	m_DlgFilterDrv;
	virtual BOOL OnInitDialog();
	afx_msg void UloadDelFile();
};
