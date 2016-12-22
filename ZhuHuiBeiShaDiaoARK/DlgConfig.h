#pragma once


// CDlgConfig 对话框

class CDlgConfig : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig();
	virtual ~CDlgConfig();

// 对话框数据
	enum { IDD = IDD_DLG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
