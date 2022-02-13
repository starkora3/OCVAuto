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

		// 実行ファイルを削除
		CFileOperation::DeleteFile ( strFile );
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;

}
