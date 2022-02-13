#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"

#include <opencv2/core.hpp>    // coreモジュールのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiモジュールのヘッダーをインクルード
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <Psapi.h> 

#include <WinHttp.h>

#pragma comment( lib, "Winhttp.lib" )

//#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;

LPCSTR STR_MNU = "manual";

/**
* ステップ実行
*/
int CMainDlg::DoStepSub(const TASK_STEP& step, CString& strSnapShot,int& nState)
{
	switch (step.type)
	{
	case TYPE_END:
		return DoEnd();
	case TYPE_SNAPSHOT:
		{
			DoSnapShot(m_cStep.strWndSearch, step.strParam);
			return RET_OK;
		}		
	case TYPE_CLICK:
		return DoClick(step);
	case TYPE_DCLICK:
		return DoDClick(step);

	case TYPE_SHELL:
		return DoShell(step);
	case TYPE_ACC_GET:
		return DoAccGet(step);
	case TYPE_ACC_NEXT:
		return DoAccNext(step);
	case TYPE_ACC_INPUT:
		return DoAccInput(step);

	//! Nox
	case TYPE_NOX_CMD:
		return DoNoxShell(step);

		// ウィンドウ系
	case TYPE_SEARCH_BORDER_EXIST:
		return DoSearchBorderExist(step, strSnapShot);
	case TYPE_SEARCH_BORDER_NONE:
			return DoSearchBorderNone(step, strSnapShot);
	case TYPE_SEARCH_BORDER_CLICK_OK:
		return DoSearchBorderOverClickOK(step, strSnapShot);
	case TYPE_SEARCH_BORDER_CLICK_NG:
		return DoSearchBorderOverClickNG(step, strSnapShot);
	case TYPE_SEARCH_BORDER_CLICK2:
		return DoSearchBorderOverClick2(step, strSnapShot);

	case TYPE_DRUG_P2P:
		return DoDrugP2P(step, strSnapShot);


	// 子ウィンドウ
	case TYPE_CHILD_SEARCH_CLICK_V:
		return DoStepChildSearchClickV(step);
	case TYPE_CHILD_SEARCH_NONE:
		return DoStepChildSearchNone(step);
	case TYPE_CHILD_SEARCH_EXIST:
		return DoStepChildSearchExist(step);

	case TYPE_MOVE_L:
		return DoMoveL(step, strSnapShot, nState);
	case TYPE_MOVE_R:
		return DoMoveR(step, strSnapShot, nState);
	case TYPE_MOVE_U:
		return DoMoveU(step, strSnapShot, nState);
	case TYPE_MOVE_D:
		return DoMoveD(step, strSnapShot,nState);

	case TYPE_MOVE_LU:
		return DoMoveLU(step, strSnapShot, nState);
	case TYPE_MOVE_LD:
		return DoMoveLD(step, strSnapShot, nState);
	case TYPE_MOVE_RU:
		return DoMoveRU(step, strSnapShot, nState);
	case TYPE_MOVE_RD:
		return DoMoveRD(step, strSnapShot, nState);

	case TYPE_KEY_DOWNUP:
		return DoKeyDownUp(step);
	case TYPE_KEY_DOWN:
		return DoKeyDown(step);
	case TYPE_KEY_UP:
		return DoKeyUp(step);

	case TYPE_MEMORY_CHECK:
		return DoMemoryCheck(step);
	case TYPE_STATE_SEND:
		return DoDesktopPost(step);

	case TYPE_ADB_PS_START:
		return DoADBPSStart(step);
	case TYPE_ADB_PS_CHECK:
		return DoADBPSCheck(step);
	case TYPE_ADB_PS_KILL:
		return DoADBPSKill(step);

	case TYPE_TASK_KILL:
		return DoTaskKill(step);
	case TYPE_TASK_KILL_W:
		return DoTaskKillW(step);
	case TYPE_KEY_HOME:
		return DoKeyHome(step);
	case TYPE_KEY_PSW:
		return DoKeyPSW(step);
	case TYPE_KEY_INPUT:
		return DoKeyInput(step);

	case TYPE_PROCESS_KILL_SERVICE:
		return DoProcessKillService(step);
	case TYPE_PROCESS_KILL:
		return DoProcessKill();

	case TYPE_TASK_SERVICE_DISABLE:
		m_chkService.SetCheck(0);
		return RET_OK;
	case TYPE_TASK_SERVICE_ENABLE:
		m_chkService.SetCheck(1);
		return RET_OK;
	case TYPE_ERROR:
		return RET_ERR;
	default:
		return RET_UNKNOWN;
	}
	return RET_ERR;
}



/**
 * 比較関数を定義
 */
bool CMainDlg::ascTop(const FINDDATA& left, const FINDDATA& right) 
{
	return left.rc.top < right.rc.top ;
}

/**
 * 画像保存用 ID取得
 */
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0, size = 0;
	GetImageEncodersSize(&num, &size);
	if (!size)return -1;
	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)GlobalAlloc(GPTR, size);
	if (!pImageCodecInfo)return -1;
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j<num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			GlobalFree(pImageCodecInfo);
			return j;
		}
	}
	GlobalFree(pImageCodecInfo);
	return -1;
}

/**
* 1, 終了
*/
int CMainDlg::DoEnd()
{
	return RET_RESET;
}
LPCTSTR STR_PREFIX_ACCOUNT = _T("{account}");

//! 2, スナップショットを撮る
CString CMainDlg::DoSnapShot(LPCTSTR _prefix,LPCTSTR _name,HWND hTarget,int nType)
{
	CString prefix(_prefix);
	if (prefix.IsEmpty())
	{
		prefix = _T("ss");
	}

	CString name(_name);
	if (name.IsEmpty())
	{
		name = CString(STR_MNU);
	}

	int nIdx = name.Find(STR_PREFIX_ACCOUNT);
	if (nIdx >= 0) {
		name.Replace(STR_PREFIX_ACCOUNT, m_strAccount);
	}

	CString temp;
	CWnd* pTarget = NULL;
	
	if (hTarget == NULL) {
		pTarget = CWnd::FromHandle(GetTaskWindow());
		if (pTarget == NULL)return NULL;
	}
	else {
		pTarget = CWnd::FromHandle(hTarget);
		if (pTarget == NULL)return NULL;
	}

	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	CString strErr, strSnapShot;
	if (CreateSnapShot(pTarget, strSnapShot, strErr, nType) == FALSE)
	{// 取得失敗
		return NULL;
	}

	CString strDest;
	{// ファイルを残す
		CString dir = GetScreenShotPath();

		time_t timer;
		struct tm newtime;

		/* 現在時刻の取得 */
		time(&timer);

		/* 現在時刻を構造体に変換 */
		localtime_s(&newtime, &timer);

		CString strExt = _T("png");
		switch (nType) {
		case TYPE_JPEG:
			strExt = _T("jpg");
			break;
		}


		strDest.Format(_T("%s_%04d%02d%02d-%02d%02d%02d_%s.%s")
			, prefix
			, newtime.tm_year + 1900
			, newtime.tm_mon + 1
			, newtime.tm_mday
			, newtime.tm_hour
			, newtime.tm_min
			, newtime.tm_sec
			, name
			, strExt
			);

		// ¥　 / :　*? "　<　>　|
		if (strDest.FindOneOf(_T("\\/:*?\"<>|")) >= 0)
		{// 禁止文字あり
			strDest.Replace(_T("\\"), _T("￥"));
			strDest.Replace(_T("/"), _T("／"));
			strDest.Replace(_T(":"), _T("："));
			strDest.Replace(_T("*"), _T("＊"));
			strDest.Replace(_T("?"), _T("？"));
			strDest.Replace(_T("\""), _T("”"));
			strDest.Replace(_T("<"), _T("＜"));
			strDest.Replace(_T(">"), _T("＞"));
			strDest.Replace(_T("|"), _T("｜"));
		}
		strDest = dir + strDest;
	}
	CFileOperation::MoveFile(strSnapShot, strDest);

	return strDest;
}

//! 3, クリック
int CMainDlg::DoClick(const TASK_STEP& step)
{
	CWnd* pTarget = CWnd::FromHandle(GetTaskWindow());
	if (pTarget == NULL)return RET_ERR;


	CRect rcTarget(0, 0, 0, 0);
	if (!m_bADB) {
		pTarget->GetWindowRect(rcTarget);
	}
	CPoint pos = rcTarget.CenterPoint();

	if (step.GetValue("position"))
	{// 位置指定
		pos.x = rcTarget.left + step.GetValue("x");
		pos.y = rcTarget.top + step.GetValue("y");
	}
	else
	{// クリック位置をずらす
		pos.x += step.GetValue("add-x");
		pos.y += step.GetValue("add-y");
	}

	int nSleep = step.GetValue("sleep");
	BOOL bMsg = step.GetValue(STR_MNU)?FALSE:TRUE;
	BOOL bDbl = (step.GetValue("dclick") > 0) ? TRUE : FALSE;


	CString str;
	str.Format(_T("Click(%d,%d) - manual:%d"), pos.x, pos.y, bMsg);
	AddLog(str);

	if (bDbl == FALSE)
	{
		ActLButtonClick(GetTaskWindow(), pos, nSleep, bMsg);
	}
	else
	{
		ActLButtonDClick(GetTaskWindow(), pos, nSleep, bMsg);
	}

	return RET_OK;
}

//! 4, Dクリック
int CMainDlg::DoDClick(const TASK_STEP& step)
{
	CWnd* pTarget = CWnd::FromHandle(GetTaskWindow());
	if (pTarget == NULL)return RET_ERR;


	CRect rcTarget(0, 0, 0, 0);
	if (!m_bADB) {
		pTarget->GetWindowRect(rcTarget);
	}
	CPoint pos = rcTarget.CenterPoint();


	if (step.GetValue("position"))
	{// 位置指定
		pos.x += step.GetValue("x");
		pos.y += step.GetValue("y");
	}
	else
	{// クリック位置をずらす
		pos.x += step.GetValue("add-x");
		pos.y += step.GetValue("add-y");
	}

	int nSleep = step.GetValue("sleep");
	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;
	BOOL bDbl = (step.GetValue("dclick") > 0) ? TRUE : FALSE;

	if (bDbl == FALSE)
	{
		ActLButtonClick(GetTaskWindow(), pos, nSleep, bMsg);
	}
	else
	{
		ActLButtonDClick(GetTaskWindow(), pos, nSleep, bMsg);
	}

	return RET_OK;
}

LPCTSTR STR_FMT_SEARCH = _T("{target}");

//! 10, シェル実行
int CMainDlg::DoShell(const TASK_STEP& step)
{

	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	CString temp = step.strParam;
	if (temp.Find(STR_FMT_SEARCH) >= 0) {
		temp.Replace(STR_FMT_SEARCH,m_cStep.strWndSearch);
	}

	AddLog(_T("シェル実行: ") + strPath + temp);

	::ShellExecute(NULL, _T("open"), strPath + temp, NULL, NULL, 0);

	int nSleep = step.GetValue("sleep");
	Sleep(nSleep);

	return RET_OK;
}


LPCTSTR STR_ACC_END = _T("-- END --");
LPCTSTR STR_FMT_CUR = _T("%s%s_%s_current.txt");
LPCTSTR STR_FMT_COPY = _T("%s%s_%s_copy.txt");

//! アカウント日付取得
CString CMainDlg::GetAccDate() {
	CString strDate;
	time_t timer;
	struct tm newtime;

	/* 現在時刻の取得 */
	time(&timer);

	if (m_nAccTimeDiff != 0) {
		// 時差を取り込む
		timer -= 3600 * m_nAccTimeDiff;
	}

	/* 現在時刻を構造体に変換 */
	localtime_s(&newtime, &timer);

	strDate.Format(_T("%04d%02d%02d")
		, newtime.tm_year + 1900
		, newtime.tm_mon + 1
		, newtime.tm_mday
	);
	return strDate;
}


//! 11, アカウント取得
int CMainDlg::DoAccGet(const TASK_STEP& step) {
	
	CString strDate = GetAccDate();


	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());
	strPath += _T("Account\\");
	CFileOperation::CreateDirectory(strPath);

	CString strCurrentFile;
	strCurrentFile.Format(STR_FMT_CUR, strPath, m_cStep.strWndSearch, strDate);

	CString strCopyFile;
	strCopyFile.Format(STR_FMT_COPY, strPath, m_cStep.strWndSearch, strDate);


	CString strSrcFile = strPath + m_cStep.strWndSearch + _T(".txt");

	if (CFileOperation::IsExistFile(strCopyFile) == FALSE) {
		// コピーファイルがないならコピーを作成する
		// テンポラリクリア
		ComFunc::CleanupFiles();

		if (CFileOperation::IsExistFile(strSrcFile) == FALSE) {
			AddLog(_T("アカウントファイルがありません。") + strSrcFile);
			return RET_NG;
		}
		if (CFileOperation::CopyFile(strSrcFile, strCopyFile,FALSE) == FALSE) {
			AddLog(_T("ファイルのコピーに失敗しました。") + strCopyFile);
			return RET_NG;
		}
	}
	CString strCopy, strCurrent;
	if (CFileOperation::ReadSJISFile(strCopyFile, strCopy) == FALSE) {
		AddLog(_T("ファイルの読み込みに失敗しました。") + strCopyFile);
		return RET_NG;
	}
	if (strCopy.IsEmpty()) {
		AddLog(_T("アカウントファイルが空です。") + strCopyFile);
		return RET_NG;
	}

	if (CFileOperation::IsExistFile(strCurrentFile)) {
		if (CFileOperation::ReadSJISFile(strCurrentFile, strCurrent) == FALSE) {
			AddLog(_T("ファイルの読み込みに失敗しました。") + strCurrentFile);
			return RET_NG;
		}
		m_strAccount = strCurrent;
		m_strSendMessage.Empty();

		if (m_strAccount == STR_ACC_END) {
			AddLog(_T("終端に到達済み"));
			m_strSendMessage = _T(" -- END -- ");
			return RET_NG;
		}

		// 現在のアカウントがコピーファイル上に存在するか？
		if (m_strAccount.IsEmpty() == FALSE) {
			int nIdx = strCopy.Find(m_strAccount);
			if ( nIdx >= 0) {
				AddLog(_T("Get Account: ") + m_strAccount);
				// 総数をカウントする
				int nItemCount = 1;
				int nItemIndex = 1;
				
				for (int nOffset = strCopy.Find(_T("\n"));
					nOffset >= 0; nOffset = strCopy.Find(_T("\n"), nOffset + 1)) {
					if (nOffset <= nIdx) {
						nItemIndex++;
					}
					nItemCount++;
				}
				m_strSendMessage.Format(_T(" %d / %d "),nItemIndex,nItemCount);
				return RET_OK;
			}
		}
		// アカウントファイル内にないならクリア
		m_strAccount.Empty();
		strCurrent.Empty();
		m_strSendMessage.Empty();
	}


	// アカウントをコピーファイルから取得する
	int idx = strCopy.Find(_T("\n"));
	if (idx > 0) {
		strCurrent = strCopy.Left(idx);
	}
	else {
		strCurrent = strCopy;
	}
	strCurrent = strCurrent.Trim(_T("\r"));
	strCurrent = strCurrent.Trim();

	if (CFileOperation::WriteSJISFile(strCurrentFile, strCurrent) == FALSE) {
		AddLog(_T("ファイルの書き込みに失敗しました。") + strCurrentFile);
		return RET_NG;
	}
	AddLog(_T("アカウントの更新: ") + strCurrentFile);
	m_strAccount = strCurrent;
	AddLog(_T("Get Account: ") + m_strAccount);
	return RET_OK;
}

//! 12, アカウント次へ
int CMainDlg::DoAccNext(const TASK_STEP& step) {

	CString strDate = GetAccDate();

	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());
	strPath += _T("Account\\");
	CFileOperation::CreateDirectory(strPath);

	CString strCurrentFile;
	strCurrentFile.Format(STR_FMT_CUR, strPath, m_cStep.strWndSearch, strDate);

	CString strCopyFile;
	strCopyFile.Format(STR_FMT_COPY, strPath, m_cStep.strWndSearch, strDate);


	CString strSrcFile = strPath + m_cStep.strWndSearch + _T(".txt");

	if (CFileOperation::IsExistFile(strCopyFile) == FALSE) {
		// コピーファイルがないならコピーを作成する
		if (CFileOperation::IsExistFile(strSrcFile) == FALSE) {
			AddLog(_T("アカウントファイルがありません。") + strSrcFile);
			return RET_NG;
		}
		if (CFileOperation::CopyFile(strSrcFile, strCopyFile, FALSE) == FALSE) {
			AddLog(_T("ファイルのコピーに失敗しました。") + strCopyFile);
			return RET_NG;
		}
	}
	CString strCopy, strCurrent;
	if (CFileOperation::ReadSJISFile(strCopyFile, strCopy) == FALSE) {
		AddLog(_T("ファイルの読み込みに失敗しました。") + strCopyFile);
		return RET_NG;
	}
	if (strCopy.IsEmpty()) {
		AddLog(_T("アカウントファイルが空です。") + strCopyFile);
		return RET_NG;
	}

	if (CFileOperation::IsExistFile(strCurrentFile)) {
		if (CFileOperation::ReadSJISFile(strCurrentFile, strCurrent) == FALSE) {
			AddLog(_T("ファイルの読み込みに失敗しました。") + strCurrentFile);
			return RET_NG;
		}

		if (strCurrent == STR_ACC_END) {
			AddLog(_T("終端に到達済み"));
			return RET_NG;
		}

		// 現在のアカウントがコピーファイル上に存在するか？
		if (strCurrent.IsEmpty() == FALSE) {
			if (strCopy.Find(strCurrent) < 0) {
				// アカウントファイル内にないならクリア
				strCurrent.Empty();
			}
		}
	}

	if (strCurrent.IsEmpty()) {
		// アカウントをコピーファイルから取得する
		int idx = strCopy.Find(_T("\n"));
		if (idx > 0) {
			strCurrent = strCopy.Left(idx);
		}
		else {
			strCurrent = strCopy;
		}
		strCurrent = strCurrent.Trim(_T("\r"));
		strCurrent = strCurrent.Trim();
	}
	else {
		int idx = strCopy.Find(strCurrent);
		do {
			if (idx < 0) {
				break;
			}
			idx = strCopy.Find(_T("\n"),idx);
			if (idx < 0) {// 最終行
				strCurrent.Empty();
				break;
			}
			idx++;

			do {
				int next = strCopy.Find(_T("\n"), idx);
				/*
				CString str;
				str.Format(_T("Find idx=%d, next=%d"),idx, next);
				AddLog(str);
				*/
				if (next < 0) {// 最終行を取得
					strCurrent = strCopy.Mid(idx);
				}
				else {
					strCurrent = strCopy.Mid(idx, next - idx);
				}
				strCurrent = strCurrent.Trim(_T("\r"));
				strCurrent = strCurrent.Trim();
				if (strCurrent.IsEmpty()==FALSE) {
					break;
				}
				if (next < 0) {
					break;
				}
				idx = next+1;
			} while (TRUE);
		} while (FALSE);

		if (strCurrent.IsEmpty()) {
			// 終点到達
			strCurrent = STR_ACC_END;
			AddLog(_T("終端に到達"));
		}
	}

	if (CFileOperation::WriteSJISFile(strCurrentFile, strCurrent) == FALSE) {
		AddLog(_T("ファイルの書き込みに失敗しました。") + strCurrentFile);
		return RET_NG;
	}
	AddLog(_T("Next Account: ") + strCurrent);
	return RET_OK;
}

//! 13, アカウント入力
int CMainDlg::DoAccInput(const TASK_STEP& step) {
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL) {
		return RET_ERR;
	}
	if (m_strAccount.IsEmpty()) {
		AddLog(_T("アカウントIDが空です。"));
		return RET_ERR;
	
	}
	AddLog(_T("アカウント入力: ") + m_strAccount);
	if (m_bADB) {
		CString strCmd;
		CString temp(m_strAccount);
		temp.Replace(_T(" "),_T(""));
		strCmd.Format(_T("adb {DEVICE} shell input text %s"), temp);
		DoADBCommand(strCmd);
		return RET_OK;
	}


	CStringA strInput = CStringA(m_strAccount);

	int nLen = strInput.GetLength();
	int nSleep = step.GetValue("sleep", 100);

	CMutexHolder mutex;

	CPoint posPre;
	if (m_nMouseMode == 0) {
		posPre = DoActiveWindow(hWnd, nSleep);
	}

	for (int i = 0; i < nLen; i++) {
		char input = strInput[i];
		if (input >= 'a' && input <= 'z') {
			WORD wKey = VKD_A + WORD(input - 'a');

			keyDown(wKey);
			Sleep(nSleep);
			keyUp(wKey);
		}
		else if (input >= 'A' && input <= 'Z') {
			WORD wKey = VKD_A + WORD(input - 'A');

			keyDown(VKD_SHIFT);
			keyDown(wKey);
			Sleep(nSleep);
			keyUp(wKey);
			keyUp(VKD_SHIFT);
		}
		else if (input >= '0' && input <= '9') {
			WORD wKey = VKD_0 + WORD(input - '0');

			keyDown(wKey);
			Sleep(nSleep);
			keyUp(wKey);
		}
	}

	if (m_nMouseMode==0) {
		// 戻す
		DoMouseMove(posPre.x, posPre.y);
	}

	return RET_OK;
}

//! 15, Noxシェル実行
int CMainDlg::DoNoxShell(const TASK_STEP& step)
{

	CString strCmd = step.strParam;

	AddLog(_T("Nox シェル実行: ") + strCmd);

	DoADBCommand(strCmd);

	return RET_OK;
}


//! 502, 状態送信
int CMainDlg::DoDesktopPost(const TASK_STEP& step) {

	m_uStateSendCounter++;
	if (m_uStateSendCounter > step.nStartTime) {
		m_uStateSendCounter = 0;
		CString str = GetLocalIPAddr();
		AddLog(_T("Source IP: ") + str);
		return DoDesktopPostSub(m_strStateSendURL, str,1);
	}
	return RET_OK;
}
//! 状態送信の実行
int CMainDlg::DoDesktopPostSub(LPCTSTR url,LPCTSTR fname,int nType) {

	CString strURL(url);
	HINTERNET hSession=NULL, hConnect=NULL, hRequest=NULL;

	CString strType = _T("image/png");
	switch (nType) {
	case 1:
		strType= _T("image/jpeg");
		break;
	}

	//! デスクトップのスナップショット画像を作成
	CString strFile = DoSnapShot(_T("desktop"), 
		_T(""), 
		GetDesktopWindow()->GetSafeHwnd(),
		nType);

	int nRet = RET_ERR;
	try {
		do {
			CString str;
			str.Format(_T("DoDesktopPost Start: %s"), url);
			AddLog(str);

			// WinHTTPの初期化
			hSession = WinHttpOpen(_T("OcvAuto/1.0"),
				WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
				WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (hSession == NULL) {
				break;
			}

			// URL解析
			WCHAR szHostName[256], szUrlPath[2048];
			URL_COMPONENTS urlComponents = { 0 };
			urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
			urlComponents.lpszHostName = szHostName;
			urlComponents.dwHostNameLength = sizeof(szHostName) / sizeof(WCHAR);
			urlComponents.lpszUrlPath = szUrlPath;
			urlComponents.dwUrlPathLength = sizeof(szUrlPath) / sizeof(WCHAR);
			if (!WinHttpCrackUrl(strURL, strURL.GetLength(), 0, &urlComponents)) {
				break;
			}

			// HTTPの開始
			hConnect = WinHttpConnect(hSession, szHostName, urlComponents.nPort, 0);
			if (hConnect == NULL) {
				break;
			}
			hRequest = WinHttpOpenRequest(hConnect, L"POST", szUrlPath,
				NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
				(INTERNET_SCHEME_HTTPS == urlComponents.nScheme) ? WINHTTP_FLAG_SECURE : 0);
			if (hRequest == NULL) {
				break;
			}

			CString strBoundary = _T("----ImagePostBoundary");

			/*
			WinHttpSetCredentials(hRequest,
				WINHTTP_AUTH_TARGET_SERVER,
				WINHTTP_AUTH_SCHEME_BASIC,
				_T("admin"), _T("admin"),NULL);
			*/

			WinHttpAddRequestHeaders(hRequest,
				_T("Content-Type: multipart/form-data; boundary=") + strBoundary,
				-1,
				WINHTTP_ADDREQ_FLAG_ADD);


			CFile cFile;
			if (!cFile.Open(strFile, CFile::modeRead)) {
				break;
			}
			ULONGLONG ullLen = cFile.GetLength();
			DWORD dwLen = (DWORD)ullLen;
			BYTE* pBuff = NULL;

			try {
				pBuff = new BYTE[dwLen];
				cFile.Read(pBuff, dwLen);

				CStringA strHeader0, strHeader1, strHeader2;

				strHeader0.Format("--%s\r\n"
					"Content-Disposition: form-data; name=\"msg\"\r\n\r\n%s\r\n",
					CStringA(strBoundary),
					CStringA(m_strSendMessage)
				);
				AddLog(m_strSendMessage);

				strHeader1.Format("--%s\r\n"
					"Content-Disposition: form-data; name=\"upfile\"; filename=\"%s\"\r\n"
					"Content-Type: %s\r\n\r\n",
					CStringA(strBoundary),
					CStringA(fname),
					CStringA(strType));

				strHeader1 = strHeader0 + strHeader1;

				strHeader2.Format("\r\n--%s--\r\n", CStringA(strBoundary));

				BOOL bRet = WinHttpSendRequest(hRequest,
					WINHTTP_NO_ADDITIONAL_HEADERS,
					0,
					WINHTTP_NO_REQUEST_DATA,
					0,
					strHeader1.GetLength() + strHeader2.GetLength() + dwLen, NULL);


				if (bRet)
				{
					bRet = WinHttpWriteData(hRequest, (LPVOID)(LPCSTR)strHeader1, strHeader1.GetLength(), NULL);
					bRet = WinHttpWriteData(hRequest, pBuff, dwLen, NULL);
					bRet = WinHttpWriteData(hRequest, (LPVOID)(LPCSTR)strHeader2, strHeader2.GetLength(), NULL);
				}

				WinHttpReceiveResponse(hRequest, NULL);

				DWORD dwStatusCode, dwSize = sizeof(DWORD);
				WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
					&dwStatusCode,
					&dwSize, WINHTTP_NO_HEADER_INDEX);

				str.Format(_T("DoDesktopPost StatusCode: %d"), dwStatusCode);
				AddLog(str);

				delete [] pBuff;
				pBuff = NULL;
			}
			catch (CException* p) {
				if (pBuff) {
					delete [] pBuff;
				}
				throw p;
			}


			nRet = RET_OK;

		} while (FALSE);

		if (hRequest != NULL) {
			WinHttpCloseHandle(hRequest);
		}
		if (hConnect != NULL) {
			WinHttpCloseHandle(hConnect);
		}
		if (hSession != NULL) {
			WinHttpCloseHandle(hSession);
		}
	}
	catch (CException* p) {
		TCHAR tcErr[1024];
		p->GetErrorMessage(tcErr, 1024);
		CString str;
		str.Format(_T("DoDesktopPost Error: %s"), tcErr);
		AddLog(str);
	}
	CFileOperation::DeleteFile(strFile);
	return nRet;
}

/** 
 *  21, ウィンドウから対象画像(複数)を探して、あればOK
 */
int CMainDlg::DoSearchBorderExist(const TASK_STEP& step, CString& strSnapShot)
{
	CWnd* pTarget = CWnd::FromHandle(GetTaskWindow());
	if (pTarget == NULL)return RET_ERR;


	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	CString strErr;
	if (CreateSnapShot(pTarget, strSnapShot, strErr) == FALSE)
	{// 取得失敗
		return RET_ERR;
	}

	IMAGE_FIND cFindData;
	CString strFiles = step.strSearchImage;
	while (!strFiles.IsEmpty())
	{
		CString strName;
		int idx = strFiles.Find(_T(","));
		if (idx >= 0)
		{
			strName = strFiles.Left(idx);
			strName.Trim();
			strFiles = strFiles.Mid(idx + 1);
		}
		else
		{
			strName = strFiles;
			strName.Trim();
			strFiles.Empty();
		}

		CString strSearchImage = strPath + strName;

		if (DoSearch(cFindData, strSnapShot, strSearchImage, step.dThreshold) < 0)
		{
			return RET_ERR;
		}

		if (cFindData.vecFind.size() >0)
		{// 検出!
			// 1個でも見つかったら終了
			break;
		}
	}
	if (cFindData.vecFind.size() >= step.GetValue("find",1))
	{
		return RET_OK;
	}
	return RET_NG;
}
//! 22, ウィンドウから対象画像(複数)を探して、なければOK
int CMainDlg::DoSearchBorderNone(const TASK_STEP& step, CString& strSnapShot)
{
	int nRet = DoSearchBorderExist(step, strSnapShot);
	switch(nRet)
	{
	case RET_OK: 
		return RET_NG;
	case RET_NG:
		return RET_OK;
	}
	return nRet;
}

/**
* 23, ウィンドウから対象画像を探して、ボーダー以上なら縦n番目をクリック
*/
int CMainDlg::DoSearchBorderOverClickOK(const TASK_STEP& step, CString& strSnapShot)
{
	// ウィンドウスナップショット取得
	CWnd* pTarget = CWnd::FromHandle(GetTaskWindow());
	if (pTarget == NULL)return RET_ERR;

	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	CString strSearchImage = strPath + step.strSearchImage;

	if (strSnapShot.IsEmpty())
	{
		CString strErr;
		if (CreateSnapShot(pTarget, strSnapShot, strErr) == FALSE)
		{// 取得失敗
			return RET_ERR;
		}
	}

	IMAGE_FIND cFindData;
	CString strFiles = step.strSearchImage;
	while (!strFiles.IsEmpty())
	{
		CString strName;
		int idx = strFiles.Find(_T(","));
		if (idx >= 0)
		{
			strName = strFiles.Left(idx);
			strName.Trim();
			strFiles = strFiles.Mid(idx + 1);
		}
		else
		{
			strName = strFiles;
			strName.Trim();
			strFiles.Empty();
		}

		CString strSearchImage = strPath + strName;

		if (DoSearch(cFindData, strSnapShot, strSearchImage, step.dThreshold) < 0)
		{
			return RET_ERR;
		}

		if (cFindData.vecFind.size() > 0)
		{// 1個はみつかった
			break;
		}
	}

	// 検出ソートと間引き処理
	int mode = step.GetValue("mode");
	
	VEC_FIND vec = cFindData.vecFind;

	switch (mode){
	case 0:
	default:
		break;
	case 1:// 検出位置ボーダー
		{
			int nMinX = step.GetValue("find-min-x");
			int nMinY = step.GetValue("find-min-y");
			int nMaxX = step.GetValue("find-max-x");
			int nMaxY = step.GetValue("find-max-y");
			VEC_FIND::iterator iter = vec.begin();
			while (iter != vec.end()) {
				if (nMinX !=0 &&
					iter->rc.left < nMinX) {
					iter = vec.erase(iter);
				}
				else if (nMaxX > nMinX &&
							iter->rc.left > nMaxX) {
					iter = vec.erase(iter);
				}
				else if (nMinY != 0 &&
					iter->rc.top < nMinY) {
					iter = vec.erase(iter);
				}
				else if (nMaxY > nMinY &&
					iter->rc.top > nMaxY) {
					iter = vec.erase(iter);
				}
				else {
					++iter;
				}
			}
		}
		break;
	}
	// 検出制度でソート
	// Y 軸ソート
	std::sort(vec.begin(), vec.end(), ascTop);


	if (vec.size() < step.GetValue("find", 1))
	{// 検出数が足りない
		return RET_NG;
	}

	// ログ出力
	CString str;
	if (vec.size() == 1) {
		str.Format(_T("Find Target(1) : %.3f (%d,%d)"), vec[0].val,
			vec[0].rc.left,
			vec[0].rc.top);
	}
	else
	{
		str.Format(_T("Find Target(%d) : "), vec.size());
		int idx = 0;
		for (VEC_FIND::const_iterator iter = vec.begin();
			iter != vec.end(); ++iter) {
			CString temp;
			temp.Format(_T("[%d] %.3f (%d,%d),"), idx, iter->val, iter->rc.left, iter->rc.top);
			str += temp;
			idx++;

		}
	}
	AddLog(str);


	CRect rcTarget(0,0,0,0);
	if (!m_bADB) {
		pTarget->GetWindowRect(rcTarget);
	}

	int index = step.GetValue("index");
	if (index >= vec.size()) {
		index = vec.size() - 1;
		if (index < 0)index = 0;
	}
	CPoint pos = rcTarget.TopLeft() + vec[index].rc.CenterPoint();

	// クリック位置をずらす
	pos.x += step.GetValue("add-x");
	pos.y += step.GetValue("add-y");
	int nSleep = step.GetValue("sleep");
	BOOL bMsg = (step.GetValue(STR_MNU) > 0) ? FALSE:TRUE;
	BOOL bDbl = (step.GetValue("dclick") > 0) ? TRUE : FALSE;

	if (bDbl == FALSE)
	{
		ActLButtonClick(GetTaskWindow(), pos, nSleep, bMsg);
		str.Format(_T("LClick (%d,%d) (%d)"), pos.x, pos.y, index);
		AddLog(str);

	}
	else
	{
		ActLButtonDClick(GetTaskWindow(), pos, nSleep, bMsg);
		str.Format(_T("LDClick (%d,%d) (%d)"), pos.x, pos.y, index);
		AddLog(str);
	}

	return RET_OK;
}

int CMainDlg::DoSearchBorderOverClickNG(const TASK_STEP& step, CString& strSnapShot)
{
	int nRet = DoSearchBorderOverClickOK(step, strSnapShot);
	switch (nRet)
	{
	case RET_OK: return RET_NG;
	case RET_NG: return RET_OK;
	}
	return nRet;
}


/**
* 25, ウィンドウから対象画像を探して、ボーダー以上ならクリック ＆サブ項目もクリック
*/
int CMainDlg::DoSearchBorderOverClick2(const TASK_STEP& step, CString& strSnapShot)
{
	// ウィンドウスナップショット取得
	CWnd* pTarget = CWnd::FromHandle(GetTaskWindow());
	if (pTarget == NULL)return RET_ERR;


	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	CString strSearchImage = strPath + step.strSearchImage;

	if (strSnapShot.IsEmpty())
	{
		CString strErr;
		if (CreateSnapShot(pTarget, strSnapShot, strErr) == FALSE)
		{// 取得失敗
			return RET_ERR;
		}
	}

	IMAGE_FIND cFindData;
	if (DoSearch(cFindData, strSnapShot, strSearchImage, step.dThreshold) < 0)
	{
		return RET_ERR;
	}

	if (cFindData.vecFind.size() < step.GetValue("find",1))
	{// 検出数が足りない
		return RET_NG;
	}
	CString str;
	str.Format(_T("Find Target(%d) : %.3f"), cFindData.vecFind.size(), cFindData.vecFind[0].val);
	AddLog(str);

	CRect rcTarget(0, 0, 0, 0);
	if (!m_bADB) {
		pTarget->GetWindowRect(rcTarget);
	}

	{// サブ項目を探して、先にクリック
		CPoint pos;
		BOOL bFind = FALSE;
		CString strFiles = step.strParam;
		while (!strFiles.IsEmpty())
		{
			CString strName;
			int idx = strFiles.Find(_T(","));
			if (idx >= 0)
			{
				strName = strFiles.Left(idx);
				strName.Trim();
				strFiles = strFiles.Mid(idx + 1);
			}
			else
			{
				strName = strFiles;
				strName.Trim();
				strFiles.Empty();
			}

			CString strSearchImage = strPath + strName;

			IMAGE_FIND cFindData;
			if (DoSearch(cFindData, strSnapShot, strSearchImage, step.dThreshold) < 0)
			{
			}

			if (cFindData.vecFind.size() >= step.GetValue("find",1))
			{// 検出!
				bFind = TRUE;
				CString str;
				str.Format(_T("Find Sub Target(%d) : %.3f"), cFindData.vecFind.size(), cFindData.vecFind[0].val);
				AddLog(str);

				pos = cFindData.vecFind[0].rc.CenterPoint();
				break;
			}
		}
		if (bFind)
		{
			ActLButtonClick(GetTaskWindow(),rcTarget.TopLeft() + pos);
		}
	}

	VEC_FIND vec = cFindData.vecFind;
	std::sort(vec.begin(), vec.end(), ascTop);

	int idx = step.GetValue("index");
	if (vec.size()) {
		if (idx >= vec.size()) {
			idx = vec.size() - 1;
		}

		ActLButtonClick(GetTaskWindow(),
			rcTarget.TopLeft() + vec[idx].rc.CenterPoint());
	}

	return RET_OK;
}

/**
* 31, 位置間ドラッグ
*/
int CMainDlg::DoDrugP2P(const TASK_STEP& step, CString& strSnapShot)
{
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}


	CPoint pos(step.GetValue("start-x"), step.GetValue("start-y")), dest(step.GetValue("end-x"), step.GetValue("end-y"));
	
	BOOL bMsg = (step.GetValue(STR_MNU) > 0) ? FALSE : TRUE;

	// スワイプ移動スリープ [msec]
	int nSleep = step.GetValue("sleep");
	if (nSleep == 0) {
		nSleep = 3;
	}

	if (bMsg) {
		CRect rc;
		::GetClientRect(hWnd, &rc);

		// WM_LBUTTONDOWN を送る	
		::SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELONG(pos.x, pos.y));

		int dX = (dest.x - pos.x);
		if (dX != 0)
		{
			dX = (dX > 0) ? 1 : -1;
		}
		int dY = (dest.y - pos.y);
		if (dY != 0)
		{
			dY = (dY > 0) ? 1 : -1;
		}

		while (pos != dest)
		{
			Sleep(nSleep);
			if (pos.x != dest.x)
			{
				pos.x += dX;
			}
			if (pos.y != dest.y)
			{
				pos.y += dY;
			}
			::SendMessage(hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(pos.x, pos.y));
		}

		::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(dest.x, dest.y));
		::SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELONG(dest.x, dest.y));
	}
	else {// 物理スワイプ
		if (m_bADB) {
			CString strCmd;
			strCmd.Format(_T("adb {DEVICE} shell input swipe %d %d %d %d"), pos.x, pos.y,dest.x,dest.y);
			DoADBCommand(strCmd);
			return RET_OK;
		}

		CWnd* pTarget = CWnd::FromHandle(hWnd);
		CRect rcTarget(0, 0, 0, 0);
		if (!m_bADB) {
			pTarget->GetWindowRect(rcTarget);
		}

		pos.Offset(rcTarget.TopLeft());
		dest.Offset(rcTarget.TopLeft());

		int dX = (dest.x - pos.x);
		if (dX != 0)
		{
			dX = (dX > 0) ? 1 : -1;
		}
		int dY = (dest.y - pos.y);
		if (dY != 0)
		{
			dY = (dY > 0) ? 1 : -1;
		}


		CMutexHolder mutex;
		AddLog(_T("ドラッグ開始"));

		CPoint posPre;
		GetCursorPos(&posPre);

		// マウス移動
		DoMouseMove(pos.x, pos.y);

		// クリック	
		DoLButtonDown();

		while (pos != dest)
		{
			Sleep(nSleep);
			if (pos.x != dest.x)
			{
				pos.x += dX;
			}
			if (pos.y != dest.y)
			{
				pos.y += dY;
			}
			// マウス移動
			DoMouseMove(pos.x, pos.y);
		}

		DoLButtonUp();

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}

		AddLog(_T("ドラッグ終了"));

	}

	return RET_OK;
}

/**
* 101, 移動
*/
int CMainDlg::DoMoveL(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	return DoMoveSub(VKD_A, step, strSnapShot, nState);
}
int CMainDlg::DoMoveR(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	BOOL bMove;
	return DoMoveSub(VKD_D, step, strSnapShot, nState);
}
int CMainDlg::DoMoveU(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	return DoMoveSub(VKD_W, step, strSnapShot, nState);
}
int CMainDlg::DoMoveD(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	return DoMoveSub(VKD_S, step, strSnapShot, nState);
}
int CMainDlg::DoMoveLU(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	int nRet = DoMoveSub(VKD_A, step, strSnapShot, nState, MOVE_SUB_UP);
	return nRet;
}
int CMainDlg::DoMoveLD(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	int nRet =  DoMoveSub(VKD_A, step, strSnapShot, nState, MOVE_SUB_DOWN);
	return nRet;
}

int CMainDlg::DoMoveRU(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	int nRet =  DoMoveSub(VKD_D, step, strSnapShot, nState, MOVE_SUB_UP);
	return nRet;
}
int CMainDlg::DoMoveRD(const TASK_STEP& step, CString& strSnapShot, int& nState)
{
	int nRet = DoMoveSub(VKD_D, step, strSnapShot, nState, MOVE_SUB_DOWN);
	return nRet;
}

/**
 *
 */
int CMainDlg::DoKeyDownUp(const TASK_STEP& step)
{
	int nKey = GetKey(step.strParam);

	if (nKey < 0)
	{
		return RET_ERR;
	}
	HWND hWnd = GetTaskWindow();

	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;

	if (bMsg) {
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
		Sleep(1);
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);
	}
	else {
		if (m_bADB) {
			CString strCmd;
			strCmd = _T("adb {DEVICE} shell input keyevent KEYCODE_HOME");
			switch (nKey) {
			case VKD_ESCAPE:
			case VKD_BACK:
				strCmd = _T("adb {DEVICE} shell input keyevent KEYCODE_BACK");
				break;
			}
			DoADBCommand(strCmd);

			return RET_OK;
		}

		int nSleep = step.GetValue("sleep");

		CMutexHolder mutex;


		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd, nSleep);
		}
		Sleep(nSleep);

		// キー押下
		keyDown(nKey);
		Sleep(nSleep);
		keyUp(nKey);

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}
	}

	return RET_OK;

}
int CMainDlg::DoKeyDown(const TASK_STEP& step)
{
	int nKey = GetKey(step.strParam);

	if (nKey < 0)
	{
		return RET_ERR;
	}
	HWND hWnd = GetTaskWindow();
	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;

	if (bMsg) {
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
	}
	else {
		int nSleep = step.GetValue("sleep");

		CMutexHolder mutex;


		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd, nSleep);
		}
		Sleep(nSleep);

		// キー押下
		keyDown(nKey);

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}
	}

	return RET_OK;

}
int CMainDlg::DoKeyUp(const TASK_STEP& step)
{
	int nKey = GetKey(step.strParam);

	if (nKey < 0)
	{
		return RET_ERR;
	}
	HWND hWnd = GetTaskWindow();
	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;

	if (bMsg) {
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);
	}
	else {
		int nSleep = step.GetValue("sleep");

		CMutexHolder mutex;


		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd, nSleep);
		}
		Sleep(nSleep);

		// キーアップ
		keyUp(nKey);

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}
	}
	return RET_OK;

}


/**
 * メモリ使用量チェック
 */
int CMainDlg::DoMemoryCheck(const TASK_STEP& step)
{
	double dThreshold = step.dThreshold;

	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}

	DWORD dwPID;

	GetWindowThreadProcessId(hWnd, &dwPID);

	if (dwPID == 0) {
		return RET_ERR;
	}

	PROCESS_MEMORY_COUNTERS pmc;
	::ZeroMemory(&pmc, sizeof(pmc));
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
	if (hProcess == NULL) {
		return RET_ERR;
	}
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		CString str;

		double dCurrent = double(pmc.WorkingSetSize) / 1024. / 1024.;
		if (dCurrent >= dThreshold) {
			str.Format(_T("Memory Over : %.1f [MB]"), dCurrent);
			AddLog(str);
			return RET_NG;
		}
		str.Format(_T("Memory OK : %.1f [MB]"), dCurrent);
		AddLog(str);
	}
	else {
		return RET_ERR;
	}

	return RET_OK;
}


int CMainDlg::GetKey(LPCTSTR str)
{
	CStringA key(str);
	key.MakeLower();
	if (key.GetLength() == 1)
	{
		if (key[0] >= 'a' && key[0] <= 'z')
		{
			return VKD_A + (key[0] - 'a');
		}
		if (key[0] >= '0' && key[0] <= '9')
		{
			return VKD_0 + (key[0] - '0');
		}
		if (key == '.')return VKD_OEM_PERIOD;
		if (key == '@')return VKD_OEM_3;
		
	}
	if (key.Compare("back")==0)return VKD_BACK;
	if (key.Compare("home") == 0)return VKD_HOME;
	if (key.Compare("end") == 0)return VKD_END;
	if (key.Compare("pagedown") == 0)return VKD_NEXT;
	if (key.Compare("pgdn") == 0)return VKD_NEXT;
	if (key.Compare("pageup") == 0) return VKD_PRIOR;
	if (key.Compare("pgup")==0) return VKD_PRIOR;
	if (key.Compare("left") == 0)return VKD_LEFT;
	if (key.Compare("back") == 0)return VKD_BACK;
	if (key.Compare("up") == 0)return VKD_UP;
	if (key.Compare("down") == 0)return VKD_DOWN;
	if (key.Compare("esc") == 0)return VKD_ESCAPE;
	if (key.Compare("escape") == 0)return VKD_ESCAPE;
	return -1;
}


int CMainDlg::ExeKeyDownUp(int nKey, int nSleep) {
	HWND hWnd = GetTaskWindow();
	::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
	Sleep((int)((double)nSleep*m_dSleepRatio));
	::SendMessage(hWnd, WM_KEYUP, nKey, 0);

	return 0;
}
int CMainDlg::ExeKeyDown(int nKey) {
	HWND hWnd = GetTaskWindow();
	::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
	return 0;
}
int CMainDlg::ExeKeyUp(int nKey) {
	HWND hWnd = GetTaskWindow();
	::SendMessage(hWnd, WM_KEYUP, nKey, 0);
	return 0;
}

/**
 * 移動処理
 */
int CMainDlg::DoMoveSub( int nKey, 
										const TASK_STEP& step, 
										CString& strSnapShot, 
										int& nState,
										int nSub)
{
	CWnd* pTarget = CWnd::FromHandle(GetTaskWindow());
	if (pTarget == NULL)return RET_ERR;

	CRect rcTarget(0, 0, 0, 0);
	if (!m_bADB) {
		pTarget->GetWindowRect(rcTarget);
	}
	CString strFiles = step.strSearchImage;

	int nRet = RET_NG;
	if (!strFiles.IsEmpty())
	{// 検索画像が存在
		if (strSnapShot.IsEmpty())
		{
			CString strErr;
			if (CreateSnapShot(pTarget, strSnapShot, strErr) == FALSE)
			{// 取得失敗
				return RET_ERR;
			}
		}

		CRect rcFind;
		BOOL bFind = DoSearchFiles(strSnapShot, strFiles, step.dThreshold, rcFind);

		if (bFind)
		{// 見つかったら 終了
			return RET_OK;
		}

		if (step.strParam3.IsEmpty() == FALSE)
		{// 除外項目 これがあったら移動処理しない
			bFind = DoSearchFiles(strSnapShot, step.strParam3, step.dThreshold, rcFind);
			if (bFind) {
				return RET_NG;
			}
		}

		// オーバー判定
		if (nState % 2 == 0)
		{// 正方向に移動中
			if (!step.strParam.IsEmpty())
			{
				bFind = DoSearchFiles(strSnapShot, step.strParam, step.dThreshold, rcFind);
				if (bFind) {// 行き過ぎ
					nState++;
				}
			}
		}
		else
		{// 逆方向に移動中
			if (!step.strParam2.IsEmpty())
			{
				bFind = DoSearchFiles(strSnapShot, step.strParam2, step.dThreshold, rcFind);
				if (bFind) {// 戻りすぎ
					nState++;
				}
			}
		}
	}
	else
	{
		nRet = RET_OK;
	}

	// 移動する
	if (nState % 2 == 0)
	{// 正方向移動
	}
	else
	{// 逆方向移動
		switch (nKey)
		{
		case VKD_A:nKey = VKD_D; break;
		case VKD_D:nKey = VKD_A; break;
		case VKD_W:nKey = VKD_S; break;
		case VKD_S:nKey = VKD_W; break;
		}
	}

	double dSleep = (double)step.GetValue("sleep");
	double dCount = (double)step.GetValue("count");
	for (int i = nState; i > 1; i -= 2)
	{
		dSleep *= 0.8;
		dCount *= 0.8;
	}
	int nSleep = (int)dSleep;
	int nCount = (int)dCount;
	if (nCount <= 0)
	{
		nCount = 1;
	}

	for (int i = 0; i < nCount; i++)
	{
		mouseMove(pTarget->GetSafeHwnd(), nKey, nSleep);
		switch (nSub) {
		case MOVE_SUB_UP:
			mouseMove(GetTaskWindow(), VKD_W, 300);
			break;
		case MOVE_SUB_DOWN:
			mouseMove(GetTaskWindow(), VKD_S, 300);
			break;
		}
	}
	return nRet;
}


/**
 * 対象画像(複数) を探す
 */
BOOL CMainDlg::DoSearchFiles(CString strSnapShot, 
												CString strFiles,
												double dThreshold,
												CRect& rc)
{
	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	while (!strFiles.IsEmpty())
	{
		CString strName;
		int idx = strFiles.Find(_T(","));
		if (idx >= 0)
		{
			strName = strFiles.Left(idx);
			strName.Trim();
			strFiles = strFiles.Mid(idx + 1);
		}
		else
		{
			strName = strFiles;
			strName.Trim();
			strFiles.Empty();
		}

		CString strSearchImage = strPath + strName;

		IMAGE_FIND cFindData;
		if (DoSearch(cFindData, strSnapShot, strSearchImage, dThreshold) < 0)
		{
		}

		if (cFindData.vecFind.size() > 0)
		{// 検出!
			rc = cFindData.vecFind[0].rc;
			return TRUE;
		}
	}
	return FALSE;
}

/**
 */
int CMainDlg::DoTaskKillW(const TASK_STEP& step)
{
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}
	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;

	if (bMsg) {

		int nKey = VKD_PRIOR;
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
		Sleep(1);
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);

		Sleep(2000);

		CRect rc;
		::GetClientRect(hWnd, &rc);

		CPoint pos, dest;
		pos.y = rc.CenterPoint().y;
		pos.x = dest.x = rc.Width() * 9 / 10;

		dest.y = rc.top + (rc.Height() / 10);

		// WM_LBUTTONDOWN を送る	
		::SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELONG(pos.x, pos.y));

		int nMove = dest.y - pos.y;
		for (int i = 0; i < abs(nMove); i += 10)
		{
			::SendMessage(hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(pos.x, pos.y + ((nMove > 0) ? i : -i)));
			Sleep(10);
		}

		::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(dest.x, dest.y));
		::SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELONG(dest.x, dest.y));

		Sleep(2000);

		nKey = VKD_HOME;
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
		Sleep(1);
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);
	}
	else {
		CMutexHolder mutex;

		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd);
		}
		int nKey = VKD_PRIOR;
		keyDown(nKey);
		Sleep(1);
		keyUp(nKey);

		Sleep(2000);

		CRect rc;
		::GetClientRect(hWnd, &rc);

		CPoint pos, dest;
		pos.x = rc.CenterPoint().x;
		pos.y = dest.y = rc.Height() * 9 / 10;

		dest.x = rc.left + (rc.Width() / 10);

		::ClientToScreen(hWnd, &pos);
		::ClientToScreen(hWnd, &dest);

		// マウス移動
		DoMouseMove(pos.x, pos.y);
		// クリック
		DoLButtonDown();

		int nMove = dest.y - pos.y;
		for (int i = 0; i < abs(nMove); i += 10)
		{
			DoMouseMove(pos.x, pos.y + ((nMove > 0) ? i : -i));
			Sleep(10);
		}

		// クリックアップ
		DoLButtonUp();

		Sleep(2000);

		nKey = VKD_HOME;
		keyDown(nKey);
		Sleep(1);
		keyUp(nKey);

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}
	}

	return RET_OK;
}


extern LPCTSTR STR_DEVICE;

//! 800
int  CMainDlg::DoADBPSStart(const TASK_STEP& step) {
	/**
	*   コマンド: adb shell am start -n {パッケージ名}/{クラス名}
	* 
	*   パッケージ名を調べるには
	*	adb shell pm dump {パッケージ名}
	*	 を実行して、
	*	「android.intent.action.MAIN:」 を探す
	*/

	CString strCmd;
	strCmd.Format(_T("adb %s shell am start -n %s"), STR_DEVICE, step.strParam);
	DoADBCommand(strCmd);
	return RET_OK;
}

//! プロセスチェック
int  CMainDlg::DoADBPSCheck(const TASK_STEP& step) {
	CString strFile = CFileOperation::CreateTmpFileInDir(m_strTempDir, _T("ret"));

	CString strCmd,strBuff;
	strCmd.Format(_T("adb %s shell pidof %s > \"%s\""), 
		STR_DEVICE, step.strParam, strFile);
	DoADBCommand(strCmd);

	if (CFileOperation::ReadSJISFile(strFile, strBuff)) {
		AddLog(CString(_T("Process Check: ")) + strBuff);
		if (_ttol(strBuff) > 0)return RET_OK;
	}
	return RET_NG;
}
// ADBコマンドタスクキル
int  CMainDlg::DoADBPSKill(const TASK_STEP& step) {
	CString strCmd;
	strCmd.Format(_T("adb %s shell am force-stop %s"), STR_DEVICE, step.strParam);
	DoADBCommand(strCmd);
	return RET_OK;
}

/**
 *
 */
int CMainDlg::DoTaskKill(const TASK_STEP& step)
{
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}
	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;

	if (bMsg) {
		int nKey = VKD_PRIOR;
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
		Sleep(1);
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);

		Sleep(2000);

		CRect rc;
		::GetClientRect(hWnd, &rc);

		CPoint pos, dest;
		pos.x = rc.CenterPoint().x;
		pos.y = dest.y = rc.Height() * 9 / 10;

		dest.x = rc.left + (rc.Width() / 10);

		// WM_LBUTTONDOWN を送る	
		::SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELONG(pos.x, pos.y));

		int nMove = dest.x - pos.x;
		for (int i = 0; i < abs(nMove); i += 10)
		{
			::SendMessage(hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELONG(pos.x + ((nMove > 0) ? i : -i), dest.y));
			Sleep(10);
		}

		::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(dest.x, dest.y));
		::SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELONG(dest.x, dest.y));

		Sleep(2000);

		nKey = VKD_HOME;
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
		Sleep(1);
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);
	}
	else {
		CMutexHolder mutex;


		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd);
		}
		int nKey = VKD_PRIOR;
		keyDown(nKey);
		Sleep(1);
		keyUp(nKey);

		Sleep(2000);

		CRect rc;
		::GetClientRect(hWnd, &rc);

		CPoint pos, dest;
		pos.x = rc.left + (rc.Width() * 7 / 10);
		pos.y = dest.y = rc.top + rc.Height() * 6 / 10;

		dest.x = rc.left + (rc.Width() * 1.5/10);

		::ClientToScreen(hWnd, &pos);
		::ClientToScreen(hWnd, &dest);

		// マウス移動
		DoMouseMove(pos.x, pos.y);
		// クリック
		DoLButtonDown();

		int nMove = dest.x - pos.x;
		for (int i = 0; i < abs(nMove); i += 10)
		{
			DoMouseMove( pos.x + ((nMove > 0) ? i : -i), dest.y);
			Sleep(10);
		}

		// クリックアップ
		DoLButtonUp();

		Sleep(2000);

		nKey = VKD_HOME;
		keyDown(nKey);
		Sleep(1);
		keyUp(nKey);

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}
	}

	return RET_OK;
}

//! ウィンドウアクティブ化(物理)
CPoint CMainDlg::DoActiveWindow(HWND hWnd,int nSleep) {
	::SetForegroundWindow(hWnd);

	// ウィンドウをアクティブ化するために (1,1)をクリック
	CPoint pos(m_posActivate);
	::ClientToScreen(hWnd, &pos);

	CPoint posPre;
	GetCursorPos(&posPre);

	// マウス移動
	DoMouseMove(pos.x, pos.y);

	// クリック
	DoLButtonDownUp(nSleep);

	::Sleep(nSleep);

	return posPre;
}


/**
* HOME キー 押下
*/
int CMainDlg::DoKeyHome(const TASK_STEP& step)
{
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}

	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;
	
	if (bMsg) {
		int nKey = VKD_HOME;
		::SendMessage(hWnd, WM_KEYDOWN, nKey, 0);
		Sleep(1);
		::SendMessage(hWnd, WM_KEYUP, nKey, 0);
	}
	else {
		if (m_bADB) {
			AddLog(_T("ADBホームキー実行"));
			CString strCmd;
			strCmd = _T("adb {DEVICE} shell input keyevent KEYCODE_HOME");
			DoADBCommand(strCmd);
			return RET_OK;
		}

		AddLog(_T("物理ホームキー実行"));

		int nSleep = step.GetValue("sleep");

		CMutexHolder mutex;


		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd, nSleep);
		}
		Sleep(nSleep);

		// Homeキー押下
		keyDown(VKD_HOME);
		Sleep(nSleep);
		keyUp(VKD_HOME);

		if (m_nMouseMode == 0) {
			// 戻す
			DoMouseMove(posPre.x, posPre.y);
		}
	}

	return RET_OK;
}

int CMainDlg::DoKeyPSW(const TASK_STEP& step)
{
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}
	BOOL bMsg = step.GetValue(STR_MNU) ? FALSE : TRUE;

	if (bMsg) {
		ExeKeyDownUp(VKD_A);
		ExeKeyDownUp(VKD_W);
		ExeKeyDownUp(VKD_A);
		ExeKeyDownUp(VKD_S);
		ExeKeyDownUp(VKD_D);
		ExeKeyDownUp(VKD_W);
		ExeKeyDownUp(VKD_D);
		ExeKeyDownUp(VKD_S);
	}
	else {
		if (m_bADB) {
			CString strCmd;
			strCmd = _T("adb {DEVICE} shell input text AWASDWDS");
			DoADBCommand(strCmd);

			return RET_OK;
		}

		int nSleep = 100;

		CMutexHolder mutex;


		CPoint posPre;
		if (m_nMouseMode == 0) {
			posPre = DoActiveWindow(hWnd, nSleep);
		}
		Sleep(nSleep);

		WORD lst[] = {
			VKD_A
			, VKD_W
			, VKD_A
			, VKD_S
			, VKD_D
			, VKD_W
			, VKD_D
			, VKD_S
		};

		keyDown(VKD_SHIFT);
		for (int i = 0; i < 8; i++) {
			WORD key = lst[i];
			keyDown(key);
			Sleep(nSleep);
			keyUp(key);
			Sleep(nSleep);
		}
		keyUp(VKD_SHIFT);
	}
	return RET_OK;
}

/**
* キー入力:物理 
*/
int CMainDlg::DoKeyInput(const TASK_STEP& step)
{
	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL){
		return RET_ERR;
	}
	if (m_bADB) {
		CString strCmd;
		strCmd.Format(_T("adb {DEVICE} shell input text %s"), step.strParam);
		DoADBCommand(strCmd);

		return RET_OK;
	}


	CStringA strInput = CStringA(step.strParam);

	int nLen = strInput.GetLength();
	if (nLen == 0) {
		return RET_OK;
	}

	int nSleep = step.GetValue("sleep", 100);

	CMutexHolder mutex;


	CPoint posPre;
	if (m_nMouseMode == 0) {
		posPre = DoActiveWindow(hWnd, nSleep);
	}
	for (int i = 0; i < nLen; i++) {
		char input = strInput[i];
		if (input >= 'a' && input <= 'z') {
			WORD wKey = VKD_A + WORD(input - 'a');

			keyDown(wKey);
			Sleep(nSleep);
			keyUp(wKey);
		}
		else if (input >= 'A' && input <= 'Z') {
			WORD wKey = VKD_A + WORD(input - 'A');

			keyDown(VKD_SHIFT);
			keyDown(wKey);
			Sleep(nSleep);
			keyUp(wKey);
			keyUp(VKD_SHIFT);
		}
		else if (input >= '0' && input <= '9') {
			WORD wKey = VKD_0 + WORD(input - '0');

			keyDown(wKey);
			Sleep(nSleep);
			keyUp(wKey);
		}
	}

	if (m_nMouseMode == 0) {
		// 戻す
		DoMouseMove(posPre.x, posPre.y);
	}

	return RET_OK;
}


int CMainDlg::DoProcessKillService(const TASK_STEP& step){

	HWND hWnd = GetTaskWindow();
	if (hWnd == NULL)
	{
		return RET_ERR;
	}

	UINT uTemp = (UINT)step.nStartTime;
	if ( uTemp == 0){
		uTemp = 7200;
	}

	if (m_uTimerCounter >= uTemp){
		m_uTimerCounter = 0;
		return DoProcessKill();
	}
	return RET_NG;
}
int CMainDlg::DoProcessKill()
{
	HWND hWnd = GetTaskWindow();
	if (hWnd != NULL)
	{
		DWORD dwPID;

		GetWindowThreadProcessId(hWnd, &dwPID);

		if (dwPID) {
			CString str;
			str.Format(_T("taskkill /F /PID %d"), dwPID);

			AddLog(str);

			_tsystem(str);
		}
	}
	if (!m_cStep.strExeName.IsEmpty()) {
		CString str;
		str.Format(_T("wmic process where \"name='%s'\" delete"), m_cStep.strExeName);
		AddLog(str);
		_tsystem(str);

	}


	return RET_OK;
}
