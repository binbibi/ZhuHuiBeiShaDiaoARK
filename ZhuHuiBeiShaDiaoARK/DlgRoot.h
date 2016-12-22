#pragma once


// CDlgRoot 对话框

class CDlgRoot : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgRoot)

public:
	CDlgRoot();
	virtual ~CDlgRoot();

// 对话框数据
	enum { IDD = IDD_DLG_ROOTKIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
