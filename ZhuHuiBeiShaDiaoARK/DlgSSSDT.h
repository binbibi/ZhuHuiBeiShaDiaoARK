#pragma once


// CDlgSSSDT 对话框

class CDlgSSSDT : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgSSSDT)

public:
	CDlgSSSDT();
	virtual ~CDlgSSSDT();

// 对话框数据
	enum { IDD = IDD_DIALOG_SSSDT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
