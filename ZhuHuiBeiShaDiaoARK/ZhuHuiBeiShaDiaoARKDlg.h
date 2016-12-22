
// ZhuHuiBeiShaDiaoARKDlg.h : 头文件
//
#include "DLGKERNEL.h"
#include "DLGPROCESS.h"
#include "DlgDrvier.h"
#include "DlgKernelHook.h"
#include "DlgR3Hook.h"
#include "DlgNetWork.h"
#include "DlgFile.h"
#include "DlgSystemStart.h"
#include "DlgFunction.h"
#include "DlgConfig.h"
#include "DlgRoot.h"
#include "DlgAuthor.h"

#pragma once


// CZhuHuiBeiShaDiaoARKDlg 对话框
class CZhuHuiBeiShaDiaoARKDlg : public CDialogEx
{
// 构造
public:
	CZhuHuiBeiShaDiaoARKDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ZHUHUIBEISHADIAOARK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CPropertySheet		m_sheet;
	CDLGPROCESS			m_DlgPro;
	CDLGKERNEL			m_DlgKernel;
	CDlgDrvier			m_DlgDriver;
	CDlgKernelHook		m_DlgKerHook;
	CDlgR3Hook			m_DlgR3Hook;
	CDlgNetWork			m_DlgNetWork;
	CDlgFile			m_DlgFile;
	CDlgSystemStart		m_DlgSystemStart;
	CDlgFunction		m_DlgFunction;
	CDlgConfig			m_DlgConfig;
	CDlgRoot			m_DlgRootFunc;
	CDlgAuthor			m_DlgAuthor;
	virtual BOOL DestroyWindow();
};
