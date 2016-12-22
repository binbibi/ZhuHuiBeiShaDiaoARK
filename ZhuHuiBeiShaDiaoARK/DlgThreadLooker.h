#pragma once
#include "afxcmn.h"


// CDlgThreadLooker 对话框

class CDlgThreadLooker : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgThreadLooker)

public:
	CDlgThreadLooker(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgThreadLooker();

// 对话框数据
	enum { IDD = IDD_DLG_THREAD_LOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_Looker_Thread;
	virtual BOOL OnInitDialog();
	void EnumThreadByPid(DWORD pid);
};
