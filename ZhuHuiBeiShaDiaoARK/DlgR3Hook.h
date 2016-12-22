#pragma once


// CDlgR3Hook 对话框

class CDlgR3Hook : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgR3Hook)

public:
	CDlgR3Hook();
	virtual ~CDlgR3Hook();

// 对话框数据
	enum { IDD = IDD_DLG_RING3_HOOK1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
