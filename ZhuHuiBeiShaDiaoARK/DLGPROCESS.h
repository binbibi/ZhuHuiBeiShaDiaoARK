#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyListCtrl.h"


class CDLGPROCESS : public CPropertyPage
{
	DECLARE_DYNAMIC(CDLGPROCESS)

public:
	CDLGPROCESS();
	virtual ~CDLGPROCESS();

// 对话框数据
	enum { IDD = IDD_DLG_PRO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_ListPro;
	virtual BOOL OnInitDialog();
	void EnumProByOpenPro(int sign);
	void InitAPI(void);
	HANDLE NtdllOpenProcess(ACCESS_MASK AccessMask, BOOL bInheritHandle, DWORD dwProcessId);
	BOOL GetFileInfo(WCHAR *pszFileFullPath, PWCHAR pszFileCorporation);
	BOOL IsWow64(HANDLE hProc);
	CImageList m_Ico;
	
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcess32771();
	afx_msg void OnlyShow64bitProc();
	afx_msg void OnlyShow32bitProc();
	afx_msg void OpenPathName();
	afx_msg void CopyImageName();
	afx_msg void OnLookModule();
	afx_msg void OnZwKillProcess();
	//ULONG m_CurSelectPid;
	CString GetItemInfo(int subItem);
	int nItemSelct;
	BOOL m_IsCheckFileState;//用于记录上一次是不是在检验全部文件签名 如果是就删除列表控件最后一项;
	DWORD m_AllProcessNum;
	afx_msg void OnProcess32813();
	afx_msg void OnZwTerByKernel();
	afx_msg void OnKillProcessAndDelByR3();
	afx_msg void OnKillPrcessAndDelByR0();
	afx_msg void OnRootDelFile();
	int MyKillProcessAndDelFile(DWORD pid, bool IsDelFile);
	afx_msg void OnSuspendProcess();
	afx_msg void OnResumeProcess();
	afx_msg void OnCheckFileTrust();
	afx_msg void OnCheckFileAll();
	BOOL CheckFileTrust(WCHAR* FileName,BOOL IsCheckAll,int subItem,BOOL IsShowNoSing);
	afx_msg void OnLookThread();
	afx_msg void OnLookHandle();
	afx_msg void OnTimer();
	afx_msg void OnLookMemory();
	afx_msg void OnLookWindows();
	afx_msg void OnLookHanldeByR0();
	afx_msg void OnCopyPathName();
	afx_msg void OnLookFileAttribute();
	afx_msg void OnProcessWeiZhuang();
	afx_msg void OnProcessHide();
	afx_msg void OnForceKillProcess();
	afx_msg void OnDownShowModule();
	CString GetEprocess(DWORD pid);
	afx_msg void OnShowNoSingPro();
};
