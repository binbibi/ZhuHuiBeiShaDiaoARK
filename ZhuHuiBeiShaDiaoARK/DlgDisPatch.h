#pragma once
#include "afxcmn.h"

// CDlgDisPatch 对话框

class CDlgDisPatch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDisPatch)

public:
	CDlgDisPatch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDisPatch();

// 对话框数据
	enum { IDD = IDD_DLG_DISPATCHLOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_DisPatch;
	virtual BOOL OnInitDialog();
	BOOL EnumIrpDisPatch(PWCHAR DriverName);
};
