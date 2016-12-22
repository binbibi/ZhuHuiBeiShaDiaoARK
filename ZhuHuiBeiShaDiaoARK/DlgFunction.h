#pragma once


// CDlgFunction 对话框

class CDlgFunction : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgFunction)

public:
	CDlgFunction();
	virtual ~CDlgFunction();

// 对话框数据
	enum { IDD = IDD_DLG_FUNCTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
