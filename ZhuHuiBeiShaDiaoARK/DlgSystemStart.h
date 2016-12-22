#pragma once


// CDlgSystemStart 对话框

class CDlgSystemStart : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgSystemStart)

public:
	CDlgSystemStart();
	virtual ~CDlgSystemStart();

// 对话框数据
	enum { IDD = IDD_DLG_START };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
