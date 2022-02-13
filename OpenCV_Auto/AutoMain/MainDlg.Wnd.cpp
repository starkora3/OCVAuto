#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;


LPCTSTR STR_DEVICE = _T("{DEVICE}");
LPCTSTR STR_FILE = _T("{FILEPATH}");


/**
 *
 */
void CMainDlg::AddChildWindow(CWnd* pWnd, int nDepth)
{
	CString str,title;
	CRect rc;
	 // 子ウィンドウ取得
	CWnd *pChild = pWnd->GetWindow(GW_CHILD); // returns the first child
	int nCount = 1;
	while (pChild)
	{
		pChild->GetWindowText(title);
		pChild->GetWindowRect(rc);
		str.Format(_T("Child%d(%d):(%d,%d)-(%d,%d) %s"), nDepth,nCount, rc.left, rc.top, rc.Width(), rc.Height(), title);
		AddLog(str);

		AddChildWindow(pChild, nDepth + 1);

		pChild = pChild->GetWindow(GW_HWNDNEXT);
		nCount++;
	}
}


/**
* スナップショット取得
*/
BOOL CMainDlg::CreateSnapShot(CWnd* pTarget,
								CString& strFile, 
								CString& strErr,
								int nType)
{
	if (m_bADB) {
		return CreateSnapShotADB(strFile,strErr);
	}

	if (pTarget == NULL) {
		pTarget = GetDesktopWindow();
	}
	

	CRect rcTarget;
	pTarget->GetWindowRect(rcTarget);

	BOOL bRet = TRUE;
	CWnd* pDesktop = GetDesktopWindow();
	CDC* pDC = (pDesktop) ? pDesktop->GetWindowDC() : NULL;
	if (pDC == NULL) {
		strErr = _T("Desktop DC Failed!");
		return FALSE;
	}
	try
	{
		pTarget->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE| SWP_NOACTIVATE);

		strFile = CFileOperation::CreateTmpFileInDir(m_strTempDir, _T("img"));
		CFileOperation::DeleteFile(strFile);
		GUID guid = ImageFormatPNG;
		switch (nType) {
		case TYPE_PNG:
		default:
			strFile += _T(".png");
			break;
		case TYPE_JPEG:
			strFile += _T(".jpg");
			guid = ImageFormatJPEG;
			break;
		}


		CRect rc;
		pDesktop->GetWindowRect(rc);

		int nLeft = rcTarget.left;
		int nTop = rcTarget.top;
		int nWidth = rcTarget.Width();
		int nHeight = rcTarget.Height();

		// マルチウィンドウだとマイナスになる場合がある
/*		if (nLeft < 0)
		{
			nWidth += nLeft;
			nLeft = 0;
		}
		if (nTop < 0)
		{
			nHeight += nTop;
			nTop = 0;
		}*/

		CImage cimage;
		cimage.Create(nWidth, nHeight, 32);
		HDC imageHDC = cimage.GetDC();
		::BitBlt(imageHDC,
			0, 0,
			nWidth,
			nHeight,
			pDC->GetSafeHdc(),
			nLeft,
			nTop,
			SRCCOPY);
		cimage.Save(strFile, guid);
		cimage.ReleaseDC();

	}
	catch (...)
	{
		bRet = FALSE;
	}

	if (pDC)
	{
		pDesktop->ReleaseDC(pDC);
		pDC = NULL;
	}
	return bRet;
}

BOOL CMainDlg::CreateSnapShotDirect(CWnd* pTarget,
	CString& strFile,
	CString& strErr)
{
	if (pTarget == NULL)return FALSE;

	CRect rcTarget;
	pTarget->GetWindowRect(rcTarget);

	BOOL bRet = TRUE;
	CDC* pDC = pTarget->GetWindowDC();
	try
	{
		strFile = CFileOperation::CreateTmpFileInDir(m_strTempDir, _T("img"));
		CFileOperation::DeleteFile(strFile);
		strFile += _T(".png");

		CImage cimage;
		cimage.Create(rcTarget.Width(), rcTarget.Height(), 32);
		HDC imageHDC = cimage.GetDC();
		::BitBlt(imageHDC,
			0, 0,
			rcTarget.Width(),
			rcTarget.Height(),
			pDC->GetSafeHdc(),
			0,
			0,
			SRCCOPY);
		cimage.Save(strFile, ImageFormatPNG);
		cimage.ReleaseDC();

	}
	catch (...)
	{
		bRet = FALSE;
	}

	if (pDC)
	{
		pTarget->ReleaseDC(pDC);
		pDC = NULL;
	}
	return bRet;
}

//! ADB スナップショットの取得
BOOL CMainDlg::CreateSnapShotADB(CString& strFile, CString& strErr,LPCTSTR device) {
	LPCTSTR STR_CMD = _T("adb {DEVICE} exec-out screencap -p > {FILEPATH}");


	BOOL bRet = FALSE;
	try
	{
		strFile = CFileOperation::CreateTmpFileInDir(m_strTempDir, _T("img"));
		CFileOperation::DeleteFile(strFile);
		strFile += _T(".png");

		CString strCmd(STR_CMD);
		strCmd.Replace(STR_FILE, strFile);

		bRet = DoADBCommand(strCmd,device);

	}
	catch (...)
	{
	}
	return bRet;
}

/**
 *  ADB コマンド実行
 */
BOOL CMainDlg::DoADBCommand(CString cmd,LPCTSTR device) {
	CString strFile;
	try
	{
		CString strDevice = _T("");

		if (device != NULL) {
			strDevice += _T("-s ");
			strDevice += device;
		}
		else if (!m_strDevice.IsEmpty()) {
			strDevice = _T("-s ") + m_strDevice;
		}
		cmd.Replace(STR_DEVICE, strDevice);

		strFile = CFileOperation::CreateTmpFileInDir(m_strTempDir, _T("cmd"));
		CFileOperation::DeleteFile(strFile);
		strFile += _T(".bat");

		CFile cFile;
		if (cFile.Open(strFile, CFile::modeCreate | CFile::modeWrite) == FALSE) {
			return FALSE;
		}

		CString strCmd;
		if (m_bNox) {
			// Noxはカレントフォルダを変更する必要がある
			strCmd = _T("cd \"C:\\Program Files (x86)\\Nox\\bin\" \r\n");
		} 

		strCmd += cmd;
		CStringA str(strCmd);
		cFile.Write((LPCSTR)str, str.GetLength());
		cFile.Close();

		STARTUPINFO         si;
		PROCESS_INFORMATION tProcessInfomation = { 0 };

		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		::CreateProcess(
			strFile
			, _T("")
			, NULL
			, NULL
			, FALSE
			, 0
			, NULL
			, NULL
			, &si
			, &tProcessInfomation
		);
		/*
			プロセスの終了を待つ
		*/
		DWORD dwResult = ::WaitForSingleObject(
			tProcessInfomation.hProcess
			, 5000
		);

	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;

}
