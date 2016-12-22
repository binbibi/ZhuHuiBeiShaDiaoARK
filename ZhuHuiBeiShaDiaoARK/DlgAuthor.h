#pragma once


// CDlgAuthor 对话框

class CDlgAuthor : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgAuthor)

public:
	CDlgAuthor();
	virtual ~CDlgAuthor();

// 对话框数据
	enum { IDD = IDD_DLG_AUTHOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonIodrvier();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStringtest();
	afx_msg void OnBnClickedButtonStringtest2();
	afx_msg void OnBnClickedButtonChar();
	afx_msg void OnBnClickedButtonChar2();
	afx_msg void OnBnClickedButton2();
	DWORD m_InputPid;
	afx_msg void OnBnClickedGeteprocessTest();
};
