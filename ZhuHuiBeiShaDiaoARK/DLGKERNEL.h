#pragma once


// CDLGKERNEL 对话框

class CDLGKERNEL : public CPropertyPage
{
	DECLARE_DYNAMIC(CDLGKERNEL)

public:
	CDLGKERNEL();
	virtual ~CDLGKERNEL();

// 对话框数据
	enum { IDD = IDD_DLG_KERNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
