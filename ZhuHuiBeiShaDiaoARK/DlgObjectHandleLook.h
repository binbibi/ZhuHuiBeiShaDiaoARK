#pragma once
#include "afxcmn.h"


// CDlgObjectHandleLook 对话框

class CDlgObjectHandleLook : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgObjectHandleLook)

public:
	CDlgObjectHandleLook(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgObjectHandleLook();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_OBJECT_PRODU_LOOK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_objectHandle;
	void CDlgObjectHandleLook::InsertObjeceHandleInfo(CString HandleName, ULONG64 addr, CHAR* ImgPath);
	virtual BOOL OnInitDialog();
	void GetObjectHandle(UCHAR Index);
};
