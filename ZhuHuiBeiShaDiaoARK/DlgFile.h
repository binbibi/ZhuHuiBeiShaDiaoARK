#pragma once


// CDlgFile 对话框

class CDlgFile : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgFile)

public:
	CDlgFile();
	virtual ~CDlgFile();

// 对话框数据
	enum { IDD = IDD_DLG_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
