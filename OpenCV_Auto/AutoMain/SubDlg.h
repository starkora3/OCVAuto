#pragma once
#include "afxwin.h"
#include "DlgResize.h"
#include "ComFunc.h"
#include "MainDlg.h"


using namespace NHCOM;

// CMainDlg ダイアログ
class CSubDlg : public CDialog
{
	// コンストラクション
public:
	CSubDlg(CMainDlg* pParent);	// 標準コンストラクター

									// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUB_DIALOG };
#endif

	enum {
		MSG_SUBDLG = WM_USER+1,

		EVENT_CLOSE=1,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

	virtual BOOL OnInitDialog();


	afx_msg void OnClose();
	afx_msg void OnBnClickedWndSearchButton();
	afx_msg void OnBnClickedSwitchButton();
	afx_msg void OnBnClickedOpenSearchButton();
	afx_msg void OnBnClickedOpenSrcButton();
	afx_msg void OnBnClickedWndSearchButton2();
	afx_msg void OnBnClickedWndSetsizeButton();
	afx_msg void OnBnClickedGetimageButton();
	afx_msg void OnImageSearch();
	afx_msg void OnDropFiles(HDROP hDropInfo);


	DECLARE_MESSAGE_MAP()

	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lp);
	static BOOL CALLBACK EnumWindowsProc2(HWND hwnd, LPARAM lp);

	void InitLoadSetting();


	CWnd* GetFindWnd();
	CWnd* GetFindChildWnd();

	CEdit m_edtSrcFile;
	CEdit m_edtSearchFile;

	CEdit m_edtThreshold;
	CEdit m_edtWndSearch;
	CEdit m_edtWndChildSearch;
	CString m_strWndSearch;
	CString m_strWndChildSearch;
	CEdit m_edtFindWnd;
	CEdit m_edtFindChildWnd;
	CEdit m_edtWidth;
	CEdit m_edtHeight;

	CMainDlg* m_pMain;

	BOOL IsSearchWnd(LPCTSTR str) {
		CString title(str);
		title.MakeLower();
		return (title.Find(m_strWndSearch) >= 0) ? TRUE : FALSE;
	}
	BOOL IsSearchWndChild(LPCTSTR str) {
		CString title(str);
		title.MakeLower();
		return (title.Find(m_strWndChildSearch) >= 0) ? TRUE : FALSE;
	}
	//! 画像検索
	int DoSearch(IMAGE_FIND& ret,
		LPCTSTR src,
		LPCTSTR target,
		double dThreshold = 0.5,
		double dOverlap = 0.5) {
		return m_pMain->DoSearch(ret,src,target,dThreshold,dOverlap);
	}

	//! 設定
	BOOL GetSetting(
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""))
	{
		return m_pMain->GetSetting(key,strValue, lpszDefault);
	}
	//! 設定
	BOOL SetSetting(
		LPCTSTR key,
		LPCTSTR strValue) {
		return m_pMain->SetSetting(key,strValue);
	}

	void SetFindWindow(HWND hWnd);
	void SetFindWindowChild(HWND hWnd);


	void AddLog(LPCTSTR str) {
		m_pMain->AddLog(str);
	}

	CButton m_checkADB;
	afx_msg void OnBnClickedOpenDir();
public:
	afx_msg void OnBnClickedAdbCheck();
};
