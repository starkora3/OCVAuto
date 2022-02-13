#include "stdafx.h"
#include "App.h"
#include "SubDlg.h"
#include "FileOperation.h"
#include "DataDef.h"

#include <opencv2/core.hpp>    // coreモジュールのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiモジュールのヘッダーをインクルード
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv/highgui.h>

//#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;
using namespace SETTING;

LPCTSTR STR_KEY_CHILD_ADB = _T("WndChildADB");


CSubDlg::CSubDlg(CMainDlg* pParent ):CDialog(IDD_SUB_DIALOG,pParent)
{
	m_pMain = pParent;
}

void CSubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edtSrcFile);
	DDX_Control(pDX, IDC_EDIT2, m_edtSearchFile);
	DDX_Control(pDX, IDC_THRESHOLD_EDIT, m_edtThreshold);
	DDX_Control(pDX, IDC_WNDTITLE_EDIT, m_edtWndSearch);
	DDX_Control(pDX, IDC_WNDTITLE_EDIT2, m_edtWndChildSearch);
	DDX_Control(pDX, IDC_WNDSEARCH_EDIT, m_edtFindWnd);
	DDX_Control(pDX, IDC_WNDCHILDSEARCH_EDIT, m_edtFindChildWnd);
	DDX_Control(pDX, IDC_WNDW_EDIT, m_edtWidth);
	DDX_Control(pDX, IDC_WNDH_EDIT, m_edtHeight);
	DDX_Control(pDX, IDC_ADB_CHECK, m_checkADB);
}

BEGIN_MESSAGE_MAP(CSubDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_IMAGESEARCH_BUTTON, &CSubDlg::OnImageSearch)
	ON_BN_CLICKED(IDC_SWITCH_BUTTON, &CSubDlg::OnBnClickedSwitchButton)
	ON_BN_CLICKED(IDC_OPEN_SEARCH_BUTTON, &CSubDlg::OnBnClickedOpenSearchButton)
	ON_BN_CLICKED(IDC_OPEN_SRC_BUTTON, &CSubDlg::OnBnClickedOpenSrcButton)
	ON_BN_CLICKED(IDC_WND_SEARCH_BUTTON, &CSubDlg::OnBnClickedWndSearchButton)
	ON_BN_CLICKED(IDC_WND_GETSIZE_BUTTON, &CSubDlg::OnBnClickedWndSearchButton2)
	ON_BN_CLICKED(IDC_WND_SETSIZE_BUTTON, &CSubDlg::OnBnClickedWndSetsizeButton)
	ON_BN_CLICKED(IDC_GETIMAGE_BUTTON, &CSubDlg::OnBnClickedGetimageButton)
	ON_BN_CLICKED(IDC_OPEN_DIR, &CSubDlg::OnBnClickedOpenDir)
	ON_BN_CLICKED(IDC_ADB_CHECK, &CSubDlg::OnBnClickedAdbCheck)
END_MESSAGE_MAP()

void CSubDlg::OnClose()
{
	CWnd* p = GetParent();
	if (p && p->GetSafeHwnd())
	{
		p->PostMessage(MSG_SUBDLG, EVENT_CLOSE, 0);
	}
}


/**
* Init
**/
BOOL CSubDlg::OnInitDialog()
{
	__super::OnInitDialog();

	InitLoadSetting();

	DragAcceptFiles();

	OnBnClickedWndSearchButton();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}



void CSubDlg::OnBnClickedAdbCheck()
{
	try {
		int nCheck = m_checkADB.GetCheck();
		CString temp;
		temp.Format(_T("%d"), nCheck);
		m_pMain->SetSetting(STR_KEY_CHILD_ADB, temp);
	}
	catch (...) {

	}
}


/**
* 画像検索実行
*/
void CSubDlg::OnImageSearch()
{
	CString src, search, temp;
	m_edtSearchFile.GetWindowText(search);
	m_edtSrcFile.GetWindowTextW(src);

	if (src.IsEmpty() || search.IsEmpty())
	{
		::AfxMessageBox(_T("ファイルを指定してください。"));
	}

	m_edtThreshold.GetWindowText(temp);
	double dThreshold = _ttof(temp);

	SetSetting(_T("SrcFile"), src);
	SetSetting(_T("SearchFile"), search);
	SetSetting(_T("Threshold"), temp);

	IMAGE_FIND ret;
	int nFind = DoSearch(ret,
		src,
		search,
		dThreshold);
	if (nFind >= 0)
	{
		cv::imshow("reference", ret.mat);

		if (nFind > 0)
		{
			CString strFile = CFileOperation::CreateTmpFileInDir(m_pMain->GetTempDir(), _T("img"));
			CFileOperation::DeleteFile(strFile);
			strFile += _T(".jpg");

			cv::String path = CStringA(strFile);
			cv::imwrite(path, ret.mat);
		}
	}
	else {
		::AfxMessageBox(_T("ファイルが見つかりません"));
	}

}


/**
*
*/
void CSubDlg::OnBnClickedSwitchButton()
{
	CString str, str2;
	m_edtSearchFile.GetWindowText(str);
	m_edtSrcFile.GetWindowTextW(str2);

	m_edtSearchFile.SetWindowText(str2);
	m_edtSrcFile.SetWindowTextW(str);


}


/**
* 開く
*/
void CSubDlg::OnBnClickedOpenSrcButton()
{
	CString str;
	m_edtSrcFile.GetWindowText(str);
	str = CFileOperation::GetDirNameAtPath(str);
	CFileDialog		selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY);
	selDlg.m_ofn.lpstrInitialDir = (LPCTSTR)str;
	if (selDlg.DoModal() == IDOK)
	{
		m_edtSrcFile.SetWindowText(selDlg.GetPathName());
	}

}


/**
* 開く
*/
void CSubDlg::OnBnClickedOpenSearchButton()
{
	CString str;
	m_edtSearchFile.GetWindowText(str);
	str = CFileOperation::GetDirNameAtPath(str);
	CFileDialog		selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY);
	selDlg.m_ofn.lpstrInitialDir = (LPCTSTR)str;
	if (selDlg.DoModal() == IDOK)
	{
		m_edtSearchFile.SetWindowText(selDlg.GetPathName());
	}
}

/**
*
*/
void CSubDlg::OnBnClickedWndSearchButton2()
{
	CString str;
	m_edtFindWnd.GetWindowText(str);

	if (str.IsEmpty())
	{
		return;
	}
	HWND hWnd = (HWND)wcstol(str, NULL, 16);

	CWnd* pWnd = CWnd::FromHandle(hWnd);
	if (pWnd == NULL)return;

	CRect rc;
	pWnd->GetWindowRect(rc);
	//pWnd->GetClientRect(rc);

	str.Format(_T("%d"), rc.Width());
	m_edtWidth.SetWindowTextW(str);

	str.Format(_T("%d"), rc.Height());
	m_edtHeight.SetWindowTextW(str);

}


/**
* サイズ設定
*/
void CSubDlg::OnBnClickedWndSetsizeButton()
{
	CString str;
	m_edtFindWnd.GetWindowText(str);

	if (str.IsEmpty())
	{
		return;
	}
	HWND hWnd = (HWND)wcstol(str, NULL, 16);

	CWnd* pWnd = CWnd::FromHandle(hWnd);
	if (pWnd == NULL)return;

	m_edtWidth.GetWindowText(str);
	int nWidth = _ttol(str);
	m_edtHeight.GetWindowText(str);
	int nHeight = _ttol(str);

	::SetWindowPos(hWnd, NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE);
}


/**
*
*/
CWnd* CSubDlg::GetFindWnd()
{
	CString str;
	m_edtFindWnd.GetWindowText(str);

	if (str.IsEmpty())
	{
		return NULL;
	}

	HWND hWnd = (HWND)wcstol(str, NULL, 16);

	CWnd* pWnd = CWnd::FromHandle(hWnd);
	return pWnd;
}
/**
*
*/
CWnd* CSubDlg::GetFindChildWnd()
{
	CString str;
	m_edtFindChildWnd.GetWindowText(str);

	if (str.IsEmpty())
	{
		return NULL;
	}
	HWND hWnd = (HWND)wcstol(str, NULL, 16);

	CWnd* pWnd = CWnd::FromHandle(hWnd);
	return pWnd;
}

/**
* イメージ取得
*/
void CSubDlg::OnBnClickedGetimageButton()
{
	try
	{
		CRect rcTarget;

		CString strFile, strErr, strChild;
		BOOL bRet = FALSE;
		int nCheck = m_checkADB.GetCheck();
		if (nCheck>0) {// ADB で取得
			try {
				CString str, device;
				m_edtWndSearch.GetWindowText(str);
				if (!m_pMain->GetNoxDevice(str, device)) {
					m_pMain->GetADBDevice(str, device);
				}
				bRet = m_pMain->CreateSnapShotADB(strFile, strErr, device);
			}
			catch (CException* e) {
				CString strErr;
				e->GetErrorMessage(strErr.GetBuffer(1024), 1024);
				strErr.ReleaseBuffer();
				::AfxMessageBox(CString("Create Error!") + strErr);
			}
		}
		else {
			CWnd* pTarget = GetFindWnd();
			if (pTarget == NULL) {
				::AfxMessageBox(_T("Target Not Found!"));
				return;
			}
			pTarget->GetWindowRect(rcTarget);

			bRet = m_pMain->CreateSnapShot(pTarget,strFile, strErr);
		}
		if (bRet)
		{
			CString path = m_pMain->GetScreenShotPath();
			CString pre;
			{
				time_t timer;
				struct tm newtime;

				/* 現在時刻の取得 */
				time(&timer);

				/* 現在時刻を構造体に変換 */
				localtime_s(&newtime, &timer);

				pre.Format(_T("%04d%02d%02d-%02d%02d%02d-")
					, newtime.tm_year + 1900
					, newtime.tm_mon + 1
					, newtime.tm_mday
					, newtime.tm_hour
					, newtime.tm_min
					, newtime.tm_sec);
			}
			CString temp = path + pre + _T("ss.png");
			if (CFileOperation::MoveFile(strFile, temp))
			{
				strFile = temp;
			}

			if(!rcTarget.IsRectEmpty())
			{// Child
				CWnd* pTarget = GetFindChildWnd();
				if (pTarget && pTarget->IsWindowVisible())
				{
					pTarget->GetWindowRect(rcTarget);
					//					if (m_pMain->CreateSnapShotDirect(pTarget, strChild, strErr))
					if (m_pMain->CreateSnapShot(pTarget, strChild, strErr))
					{
						temp = path + pre + _T("ss-child.png");
						if (CFileOperation::MoveFile(strChild, temp))
						{
						}
					}
				}
			}


			//			CString dir = CFileOperation::GetDirNameAtPath(strFile);
			//			::ShellExecute(NULL, _T("open"), dir, NULL, NULL, 0);

			m_edtSrcFile.SetWindowText(strFile);

		}
		else
		{
			::AfxMessageBox(strErr);
		}
	}
	catch (...)
	{
	}
}

/**
* ファイルどろっぷ処理
*/
void CSubDlg::OnDropFiles(HDROP hDropInfo)
{
	//ドロップされたファイルの個数を取得

	CWnd* pWnd = GetFocus();
	HWND hFocus = (pWnd) ? pWnd->GetSafeHwnd() : NULL;

	UINT uCount = DragQueryFile(hDropInfo, ~0lu, NULL, 0);
	if (uCount == 0)return;
	if (hFocus != m_edtSearchFile.GetSafeHwnd())
	{
		int idx = 0;
		//ファイル名の長さを取得
		UINT uiLen = DragQueryFile(hDropInfo, idx, NULL, 0);

		//ファイル名を取得
		CString name;
		DragQueryFile(hDropInfo, idx, name.GetBuffer(uiLen + 1), uiLen + 1);
		name.ReleaseBuffer();

		m_edtSrcFile.SetWindowTextW(name);

		if (uCount > 1)
		{
			idx++;
			uiLen = DragQueryFile(hDropInfo, idx, NULL, 0);

			//ファイル名を取得
			DragQueryFile(hDropInfo, idx, name.GetBuffer(uiLen + 1), uiLen + 1);
			name.ReleaseBuffer();
			m_edtSearchFile.SetWindowTextW(name);
		}
	}
	else
	{
		int idx = 0;
		//ファイル名の長さを取得
		UINT uiLen = DragQueryFile(hDropInfo, idx, NULL, 0);

		//ファイル名を取得
		CString name;
		DragQueryFile(hDropInfo, idx, name.GetBuffer(uiLen + 1), uiLen + 1);
		name.ReleaseBuffer();

		m_edtSearchFile.SetWindowTextW(name);
	}
}

void CSubDlg::InitLoadSetting()
{
	CString temp;
	GetSetting(_T("SrcFile"), temp, _T(""));
	m_edtSrcFile.SetWindowText(temp);

	GetSetting(_T("SearchFile"), temp, _T(""));
	m_edtSearchFile.SetWindowText(temp);

	GetSetting(_T("Threshold"), temp, _T("0.5"));
	m_edtThreshold.SetWindowText(temp);

	GetSetting(_T("WndSearch"), temp, _T("Nox2"));
	m_edtWndSearch.SetWindowText(temp);

	GetSetting(_T("WndChildSearch"), temp, _T("会話"));
	m_edtWndChildSearch.SetWindowText(temp);

	GetSetting(STR_KEY_CHILD_ADB, temp, _T("0"));
	m_checkADB.SetCheck(_ttoi(temp));


//	GetSetting(STR_KEY_TASK_FILE, temp, _T(""));

}

/**
* 窓検索
*/
void CSubDlg::OnBnClickedWndSearchButton()
{
	CString str, str2;
	m_edtWndSearch.GetWindowText(str);
	if (str.IsEmpty())
	{
		::AfxMessageBox(L"WndSearch Empty!");
		return;
	}
	m_edtWndChildSearch.GetWindowText(str2);

	if (str.CompareNoCase(_T("desktop")) == 0)
	{
		SetFindWindow(::GetDesktopWindow());
		return;
	}

	SetSetting(STR_KEY_WNDSEARCH, str);
	SetSetting(STR_KEY_CHILDSEARCH, str2);

	m_strWndSearch = str;
	m_strWndSearch.MakeLower();

	m_strWndChildSearch = str2;
	m_strWndChildSearch.MakeLower();

	// 画面上のすべてのトップレベルウィンドウを列挙します。
	EnumWindows(EnumWindowsProc, (LPARAM)this);

}

/**
* ウィンドウ列挙
*/
BOOL CALLBACK CSubDlg::EnumWindowsProc(HWND hWnd, LPARAM lp)
{
	try
	{
		if (hWnd)
		{
			CSubDlg* pDlg = reinterpret_cast<CSubDlg*>(lp);
			if (pDlg)
			{
				CString str, title;
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				if (pWnd)
				{
					// ウィンドウタイトル
					pWnd->GetWindowText(title);
					if (pDlg->IsSearchWnd(title))
					{
						pDlg->SetFindWindow(hWnd);
					}
				}
			}
		}
	}
	catch (...)
	{
	}
	return TRUE;
}


/**
* ウィンドウ列挙２
*/
BOOL CALLBACK CSubDlg::EnumWindowsProc2(HWND hWnd, LPARAM lp)
{
	try
	{
		if (hWnd)
		{
			CSubDlg* pDlg = reinterpret_cast<CSubDlg*>(lp);
			if (pDlg)
			{
				CString str, title;
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				if (pWnd)
				{
					// ウィンドウタイトル
					pWnd->GetWindowText(title);
					if (pDlg->IsSearchWndChild(title))
					{
						pDlg->SetFindWindowChild(hWnd);
					}
				}
			}
		}
	}
	catch (...)
	{
	}
	return TRUE;
}


/**
* 見つかったウィンドウをセット
*/
void CSubDlg::SetFindWindow(HWND hWnd)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);

	CString str, title;
	CRect rc;
	pWnd->GetWindowText(title);
	pWnd->GetWindowRect(rc);
	str.Format(_T("Find! [%08x(%d,%d)-(%d,%d)] %s "), hWnd, rc.left, rc.top, rc.Width(), rc.Height(), title);
	AddLog(str);

	str.Format(_T("%x"), hWnd);
	m_edtFindWnd.SetWindowText(str);

	if (!m_strWndChildSearch.IsEmpty())
	{
		// 小窓検索開始
		EnumWindows(EnumWindowsProc2, (LPARAM)this);
	}

	{ // 子ウィンドウ取得
		CWnd *pChild = pWnd->GetWindow(GW_CHILD); // returns the first child
		int nCount = 1;
		while (pChild)
		{
			pChild->GetWindowText(title);
			pChild->GetWindowRect(rc);
			str.Format(_T("Child%d:(%d,%d)-(%d,%d) %s"), nCount, rc.left, rc.top, rc.Width(), rc.Height(), title);
			AddLog(str);

			//AddChildWindow(pChild, 1);


			pChild = pChild->GetWindow(GW_HWNDNEXT);
			nCount++;
		}
	}
}

/**
* 見つかったウィンドウをセット
*/
void CSubDlg::SetFindWindowChild(HWND hWnd)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);

	CString str, title;
	CRect rc;
	pWnd->GetWindowText(title);
	pWnd->GetWindowRect(rc);
	str.Format(_T("Find Child! [%08x(%d,%d)-(%d,%d)] %s "), hWnd, rc.left, rc.top, rc.Width(), rc.Height(), title);
	AddLog(str);

	m_edtFindWnd.GetWindowText(str);
	HWND hParent = (HWND)wcstol(str, NULL, 16);

	CWnd* pParent = pWnd->GetParent();
	if (hParent && pParent && pParent->GetSafeHwnd() == hParent)
	{
		str.Format(_T("%x"), hWnd);
		m_edtFindChildWnd.SetWindowText(str);
	}
}

void CSubDlg::OnBnClickedOpenDir()
{
	try {
		CString path = m_pMain->GetScreenShotPath();
		path.Replace(_T("\\"), _T("/"));
		::ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOW);
	}
	catch (...) {

	}
}
