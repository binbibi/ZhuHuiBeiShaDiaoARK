#pragma once
#include "afxcmn.h"


// CDlgHandleLook 对话框

class CDlgHandleLook : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHandleLook)

public:
	CDlgHandleLook(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHandleLook();

// 对话框数据
	enum { IDD = IDD_DLG_HANDLELOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_List_HandleLook;
	BOOL ThreadEnumHandleByZwQuerySystemInformation(DWORD pid);
};
