
#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"
#include "DataDef.h"


//#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;
using namespace SETTING;

/**
 * タスク実行
 */
void CMainDlg::OnBnClickedTaskstartButton()
{
//	CWnd* pBtn = GetDlgItem(IDC_TASKSTART_BUTTON);
//	if (pBtn == NULL)return;

	if (m_uTimer)
	{// 実行中
		KillTimer(m_uTimer);

		m_cmbName.EnableWindow(TRUE);
		m_cmbSelectTask.EnableWindow(TRUE);


		m_uTimer = 0;
		//pBtn->SetWindowTextW(L">>>>> タスク実行 >>>>>");
		m_cCurStep.SetText(STR_TASK_STOP);
		SetWindowText(STR_APP_TITLE);

		m_cmbTask.SetCurSel(m_cTaskState.nIdx);
	}
	else
	{
		UpdateTargetWindow();
		if (m_hTaskWindow == NULL) {
			if (m_cRescue.vecStep.size() == 0) {
				::AfxMessageBox(_T("ウィンドウが見つかりません。"));
			}
			else {
				AddLog(_T("ウィンドウが見つかりません。復旧を実行します。"));
				DoRescue();
			}
			return;
		}
		SetSetting(STR_WND_NAME_DEFAULT, m_cStep.strWndSearch);

		m_chkService.SetCheck(1);
		::GetWindowRect(m_hTaskWindow, &m_rcTaskWnd);

		m_strDevice.Empty();
		if (m_bADB) {
			if (m_bNox) {// Noxデバイス取得
				GetNoxDevice(m_cStep.strWndSearch, m_strDevice);
			}
			else {// ADB デバイス取得
				GetADBDevice(m_cStep.strWndSearch, m_strDevice);
			}
		}

		m_cmbName.EnableWindow(FALSE);
		m_cmbSelectTask.EnableWindow(FALSE);

		m_cTaskState.Clear();

		{
			CString str;
			m_edtPreTime.GetWindowText(str);
			m_cTaskState.nPreTime = _ttoi(str);
			m_edtPreTime.SetWindowText(_T("0"));
		}
		m_cTaskState.nIdx = m_cmbTask.GetCurSel();
		m_uTimer = SetTimer(1, 1000, NULL);
		m_uTimerCounter = 0;

		UpdateTaskState();
		//pBtn->SetWindowTextW(L"-------- タスク停止 -------");
	}
}

/**
 *
 */
void CMainDlg::UpdateTargetWindow()
{
	m_hTaskWindow = NULL;
	m_hTaskWndChild = NULL;
	m_edtTaskWnd.SetWindowTextW(NULL);
	{// ウィンドウ探索
		CString temp;
		m_cmbName.GetWindowText(temp);
		if (temp.IsEmpty())
		{
			//::AfxMessageBox(L"WndSearch Empty!");
			return;
		}
		m_cStep.strWndSearch = temp.MakeLower();
		if (!m_cStep.strWndSearch.IsEmpty())
		{
			if (m_cStep.strWndSearch.CompareNoCase(_T("desktop")) == 0)
			{
				SetFindWindowTask(::GetDesktopWindow());
				return;
			}

			// 画面上のすべてのトップレベルウィンドウを列挙します。
			EnumWindows(EnumWindowsProcTask, (LPARAM)this);
		}
	}
}
/**
 * タスクタイマー
 */
void CMainDlg::OnTimer(UINT_PTR id)
{
	__super::OnTimer(id);
	try {
		// 知らないタイマーIDはスルー
		if (id != 1)return;

		m_uTimerCounter++;

		CString log;
		// 状態を進める
		m_cTaskState.Add();

		if (::IsWindow(GetTaskWindow()) == FALSE) {
			static UINT uNotFindCnt = 0;
			if (GetTaskWindow() != NULL)
			{
				uNotFindCnt = 0;
			}

			if ((uNotFindCnt % 120) == 0)
			{
				AddLog(_T("ウィンドウが見つかりません。"));
			}
			uNotFindCnt++;

			UpdateTargetWindow();
			OnBnClickedResetButton();

			// 復旧処理があるなら実行する
			if (m_cRescue.vecStep.size() > 0) {
				AddLog(_T("復旧を実行します。"));
				DoRescue();
			}

			return;
		}
		// ウィンドウ位置調整
		static DWORD dwCnt = 0;
		dwCnt++;
		if (dwCnt % 10 == 0) {
			DoWndHold();
		}


		if (m_cTaskState.nPreTime > 0)
		{
			CString str;
			str.Format(_T("%s [ Count Down : %d ]"), STR_APP_TITLE, m_cTaskState.nPreTime);
			SetWindowText(str);
			if (m_cTaskState.nPreTime == 1)
			{
				OnBnClickedResetButton();
			}
			return;
		}

		CString str;
		str.Format(_T("%s [ %4d / %4d ]"), STR_APP_TITLE,m_cTaskState.Total(), m_cStep.nWholeTime);
		SetWindowText(str);

		if (m_cTaskState.Total() > m_cStep.nWholeTime)
		{// タイムアウト判定
			// 最初から

			if ((int)m_cStep.vecStep.size() > m_cTaskState.nIdx)
			{// ステップデータが存在
				 // ステップ実行
				const TASK_STEP& step = m_cStep.vecStep[m_cTaskState.nIdx];
				log.Format(_T("[TimeOut] Step%d : %s"), step.nNumber,step.strName);
				AddLog(log);

				CString str;
				str.Format(_T("WholeTimeOut%d"), step.nNumber);
				// スナップショットを撮る
				DoSnapShot(m_cStep.strWndSearch, str);
			}else{
				AddLog(_T("Whole TimeOut."));
				DoSnapShot(m_cStep.strWndSearch, _T("WholeTimeOut"));

			}

			m_cTaskState.Clear();
			// タイムアウト回数を集計
			m_cTaskState.AddFail(0);
			if (m_cStep.nErrorMax > 0 && m_cStep.nRefresh > 0 &&
				m_cTaskState.GetFail(0) > m_cStep.nErrorMax)
			{// タイムアウト数が規定数を超えた

				log.Format(_T("Goto Refresh Step%d"), m_cStep.nRefresh);
				AddLog(log);
				// リフレッシュ処理へ向かう
				StepGoto(m_cStep.nRefresh);
			}
		}

		CString strSnapShot;

		BOOL bServiceExecute = FALSE;
		if (m_cTaskState.nServiceInterval <=0 &&
			m_chkService.GetCheck()>0)
		{// サービス実行時
			TASK_STEP step;
			bServiceExecute =DoService(strSnapShot,step);
			if (bServiceExecute)
			{// サービス実行時はステップを無効化
				m_cTaskState.nStepInterval = m_cStep.nInterval;
				if (step.GetValue("step-sleep") > 0 )
				{
					m_cTaskState.nStepInterval = step.GetValue("step-sleep");
					CString str;
					str.Format(_T("Step Interval: %d"), m_cTaskState.nStepInterval);
					AddLog(str);
				}
				if (step.nOK){// ステップ移行があるなら
					StepGoto(step.nOK);
				}
			}
			m_cTaskState.nServiceInterval = m_cService.nInterval;
		}
		
		if(bServiceExecute == FALSE &&
			m_cTaskState.nStepInterval <= 0)
		{
			m_cTaskState.nStepInterval = m_cStep.nInterval;
			int nIdxTemp = m_cTaskState.nIdx;
			int nRet = DoStep(strSnapShot,m_cTaskState.nStepState);
			
			UpdateTaskState();

			switch (nRet)
			{
			case RET_TIMEOUT:
				{// 予期しないタイムアウト
				   // 念のためウィンドウを探す
					EnumWindows(EnumWindowsProcTask, (LPARAM)this);

					BOOL deny=FALSE;
					CString name = _T("Timeout");
					if ((int)m_cStep.vecStep.size() > nIdxTemp)
					{// ステップデータが存在
						const TASK_STEP& step = m_cStep.vecStep[nIdxTemp];
						deny=step.GetValue("snap-deny")?TRUE:FALSE;
						name.Format(_T("TimeOut_Step%d"), step.nNumber);
					}

					if (deny == FALSE)
					{
						// スナップショットを撮る
						DoSnapShot(m_cStep.strWndSearch,name);
					}
				}
				break;
			}

		}
		else
		{
#ifdef _DEBUG
//			AddLog(_T("待ち"));
#endif
		}
		m_cDeleteCache.Add(strSnapShot);
	}
	catch (...) {
	}
}

/**
 * タスク状態更新
 */
void  CMainDlg::UpdateTaskState() {
	{
		if ((int)m_cStep.vecStep.size() > m_cTaskState.nIdx)
		{
			const TASK_STEP& step = m_cStep.vecStep[m_cTaskState.nIdx];

			CString str;
			str.Format(_T("%d, %s"), step.nNumber, step.strName);
			m_cCurStep.SetText(str);
		}
		else
		{
			m_cCurStep.SetText(_T(""));
		}
	}

}



/**
 *  ステップ実行
 */
BOOL CMainDlg::DoStep(CString& strSnapShot,int& nStepState)
{
	CString log;
	int nRet = RET_WAIT;
	if((int)m_cStep.vecStep.size() > m_cTaskState.nIdx)
	{// ステップデータが存在
		// ステップ実行
		const TASK_STEP& step = m_cStep.vecStep[m_cTaskState.nIdx];
		if (m_cTaskState.nCounter < step.nStartTime)
		{// 開始待ち
			return RET_WAIT;
		}
		else if (step.nTimeOut>0 && m_cTaskState.nCounter > step.nTimeOut)
		{// ステップタイムアウト
			// ステップタイムアウトはクリアしない
			//m_cTaskState.Clear();
			m_cTaskState.AddFail(step.nNumber);

			int nErrMax = step.GetValue("error");
			if (nErrMax > 0 && m_cTaskState.GetFail(step.nNumber) > nErrMax)
			{// エラー限界を超えた
			 // リフレッシュへ
				StepGoto(step.GetValue("refresh"));
			}
			else
			{
				StepGoto(step.nErr);
			}

			CString str;
			str.Format(_T("[TimeOut] Goto %d"), step.nErr);
			AddLog(str);
			return RET_TIMEOUT;
		}
		nRet = DoStepSub(step, strSnapShot, nStepState);
		switch (nRet) {
		case RET_ERR:
			{
				CString str;
				str.Format(_T("[Error] Step%d : %s"), step.nNumber,step.strName);
				AddLog(str);
			}

			if (step.nErr > 0)
			{// エラージャンプ指定がある
				// エラーとしてはカウントしない
				StepGoto(step.nErr);

				CString str;
				str.Format(_T("[Jump] Goto %d"), step.nErr);
				AddLog(str);
			}
			else
			{// 指定がないなら最初から
				m_cTaskState.Clear();
				m_cTaskState.AddFail(step.nNumber);
				int nErrMax = step.GetValue("error");
				if (nErrMax > 0 && m_cTaskState.GetFail(step.nNumber) > nErrMax)
				{// エラー限界を超えた
					// リフレッシュへ
					StepGoto(step.GetValue("refresh"));
				}
				AddLog(_T("[Jump] Return Head."));
				nRet = RET_TIMEOUT;
			}
			break;
		case RET_OK:
			log.Format(_T("[〇] Step%d : %s"), step.nNumber, step.strName);
			AddLog(log);

			if (step.nOK == 0)
			{// 指定がなければ次へ
				m_cTaskState.NextStep();
			}
			else
			{// 指定がある
				StepGoto(step.nOK);
			}
			break;
		case RET_NG:
			log.Format(_T("[×] Step%d : %s"), step.nNumber, step.strName);
			AddLog(log);
			if (step.nNG != 0)
			{// 失敗時にジャンプ指定がある
				StepGoto(step.nNG);
			}
			else
			{// 指定がなければ繰り返す

			}
			break;
		case RET_WAIT:
			// タスク終了待ち
			if (m_cTaskState.nCounter % 10 == 0)
			{
				log.Format(_T("[ - ] Step%d : %s"), step.nNumber, step.strName);
				AddLog(log);
			}
			break;
		case RET_RESET:
			log.Format(_T("[Reset] Step%d : %s"), step.nNumber, step.strName);
			AddLog(log);
			m_cTaskState.Clear();
			// 失敗数をクリアーする ※ Clear だけではリセットされない
			m_cTaskState.ClearFail();
			break;
		case RET_UNKNOWN:
			log.Format(_T("[Unknown] Step%d : %s"), step.nNumber,step.strName);
			AddLog(log);
			m_cTaskState.NextStep();
			break;
		}
	}
	return nRet;
}

/**
 * サービス実行
 */
BOOL CMainDlg::DoService(CString& strSnapShot, TASK_STEP& cExecStep) {
	BOOL bRet = FALSE;
	if (m_cService.IsExist())
	{
		for (VEC_STEP::const_iterator iter = m_cService.vecStep.begin();
		iter != m_cService.vecStep.end(); ++iter)
		{
			int nState = TASK_STATE::STAT_INIT;

			int nRet = DoStepSub(*iter, strSnapShot, nState);
			if (nRet == RET_OK)
			{
				CString str;
				str.Format(_T("[Service][OK] %s"),iter->strName);
				AddLog(str);
				bRet = TRUE;
				cExecStep = *iter;
				break;
			}
		}
	}
	return bRet;
}

/**
 * 復旧実行
 */
void CMainDlg::DoRescue() {
	BOOL bRet = FALSE;
	if (m_cRescue.IsExist())
	{
		CString strSnapShot;
		for (VEC_STEP::const_iterator iter = m_cRescue.vecStep.begin();
			iter != m_cRescue.vecStep.end(); ++iter)
		{
			int nState = TASK_STATE::STAT_INIT;
			int nRet = DoStepSub(*iter, strSnapShot, nState);
			if (nRet == RET_OK)
			{
				CString str;
				str.Format(_T("[Rescue][OK] %s"), iter->strName);
				AddLog(str);
				bRet = TRUE;
				break;
			}
		}
	}
}


/**
* リセットボタン
*/
void CMainDlg::OnBnClickedResetButton()
{
	m_cmbTask.SetCurSel(0) ;
	m_cTaskState.nIdx = 0;
	m_cTaskState.Clear();
}

/**
 * タスク読込み
 */
void CMainDlg::OnBnClickedTaskloadButton()
{
	int nIdx = m_cmbSelectTask.GetCurSel();
	CString temp;
	temp.Format(_T("%d"), nIdx);
	SetSetting(STR_KEY_TASK_FILE_INDEX, temp);
	m_cCurDFile = m_vecDataFile[nIdx];

	UpdateTask();
}

#define STEP_MAX 2000
#define STEP_MAX_DEFAULT 500

/**
 * タスクの更新
 */
void CMainDlg::UpdateTask()
{
	CString temp;

	// スリープ時間の調整 ※ Debug と Release でスリープ時間を調整する
	GetSetting(STR_KEY_SLEEPR, temp);
	m_dSleepRatio = _ttof(temp);
	if (m_dSleepRatio <= 0.)
	{
		m_dSleepRatio = 1.0;
	}

	// マウスモード
	GetTask(STR_KEY_MOUSE_MODE, temp);
	m_nMouseMode = _ttoi(temp);

	// 状態送信
	GetTask(STR_KEY_STATE_SEND_URL, temp,_T("http://localhost/state_monitor/upload.php"));
	m_strStateSendURL = temp;

	// ステップ読込み
	m_cStep.Clear();

	int nCur = m_cmbTask.GetCurSel();
	if (nCur < 0)
	{
		nCur = 0;
	}
	m_cmbTask.ResetContent();


	// 全体時間
	GetTask(STR_KEY_TIME,temp);
	m_cStep.nWholeTime = _ttol(temp);

	// 最大ステップ数
	int nStepMax = STEP_MAX_DEFAULT;
	GetTask(STR_KEY_STEPMAX, temp);
	if (!temp.IsEmpty())
	{
		nStepMax = _ttoi(temp);
		if (nStepMax < STEP_MAX_DEFAULT)
		{
			nStepMax = STEP_MAX_DEFAULT;
		}
	}

	// Nox モード
	GetTask(STR_KEY_NOX, temp);
	m_bNox = (_ttoi(temp)) ? TRUE : FALSE;

	GetTask(STR_KEY_ADB, temp);
	m_bADB = (m_bNox || (_ttoi(temp))) ? TRUE : FALSE;

	// ウィンドウ位置固定
	GetTask(STR_KEY_WNDHOLD, temp);	
	m_nWndHold = (_ttoi(temp));
	
	GetTask(STR_KEY_ACT_X, temp,_T("1"));
	m_posActivate.x = _ttoi(temp);
	GetTask(STR_KEY_ACT_Y, temp, _T("1"));
	m_posActivate.y = _ttoi(temp);

	// StepInterval
	GetTask(STR_KEY_STEP_INTERVAL, temp);
	m_cStep.nInterval = _ttol(temp);
	if (m_cStep.nInterval < 1)
	{
		m_cStep.nInterval = 1;
	}

	GetTask(STR_KEY_ERRMAX, temp);
	m_cStep.nErrorMax = _ttol(temp);

	GetTask(STR_KEY_REFRESH, temp);
	m_cStep.nRefresh = _ttol(temp);

	GetTask(STR_KEY_REBOOT, temp);
	m_cStep.nReboot = _ttol(temp);

	//! アカウント日付変更時刻
	GetTask(STR_KEY_ACCTIMEDIFF, temp);
	m_nAccTimeDiff = _ttoi(temp);

	GetTask(STR_KEY_CHILDSEARCH, temp);
	if (temp.IsEmpty())
	{
		//::AfxMessageBox(L"WndChildSearch Empty!");
		//return;
	}
	m_cStep.strChildWndSearch = temp.MakeLower();
	
	GetTask(STR_KEY_WNDSEARCH_R, temp);
	m_cStep.strWndSearchR = temp.MakeLower();
	
	m_cStep.strExeName.Empty();
	GetTask(STR_KEY_EXE_NAME, temp);
	if (!temp.IsEmpty())
	{
		m_cStep.strExeName = temp;
	}
	//m_edtWndChildSearch.SetWindowText(temp);

	UpdateTargetWindow();

	///////////////////////////////////////////////////////////////////////////
	// ステップ読込み
	for (int i = 1; i < nStepMax;i++)
	{
		TASK_STEP step;
		GetStep(i, STR_KEY_TYPE, temp);
		step.type = _ttoi(temp);

		if (step.type == 0)
		{
			continue;
		}
		step.nNumber = i;

		GetStep(i, STR_KEY_NAME, temp);
		step.strName = temp;

		GetStep(i, STR_KEY_SIMG, temp);
		step.strSearchImage = temp;

		GetStep(i, STR_KEY_TH, temp);
		step.dThreshold = (double)_ttof(temp);

		GetStep(i, STR_KEY_OPTION, temp);
		step.ReadOption(temp);

		GetStep(i, L"ParamStr", temp);
		step.strParam = temp;
		GetStep(i, L"ParamStr2", temp);
		step.strParam2 = temp;
		GetStep(i, L"ParamStr3", temp);
		step.strParam3 = temp;

		GetStep(i, L"OK", temp);
		step.nOK = _ttoi(temp);
		GetStep(i, L"NG", temp);
		step.nNG = _ttoi(temp);
		GetStep(i, L"Err", temp);
		step.nErr = _ttoi(temp);

		GetStep(i, L"StartTime", temp);
		step.nStartTime = _ttoi(temp);
		GetStep(i, L"TimeOut", temp);
		step.nTimeOut = _ttoi(temp);
		
		m_cStep.vecStep.push_back(step);

		CString str;
		str.Format(_T("%d, %s"), step.nNumber, step.strName);
		m_cmbTask.AddString(str);
	}
	if (m_cmbTask.GetCount())
	{
		m_cmbTask.SetCurSel(nCur);
	}
	
	///////////////////////////////////////////////////////////////////////////
	// サービス
	m_cService.Clear();

	GetTask(STR_KEY_SERVICE_INTERVAL, temp);
	m_cService.nInterval = _ttol(temp);
	if (m_cService.nInterval < 1)
	{
		m_cService.nInterval = 1;
	}

	// ====================================
	// サービスステップ読込み
	for (int i = 1; i < 30; i++)
	{
		TASK_STEP step;
		GetServiceStep(i, STR_KEY_TYPE, temp);
		step.type = _ttoi(temp);

		if (step.type == 0)
		{
			continue;
		}
		step.nNumber = 1;

		GetServiceStep(i, STR_KEY_NAME, temp);
		step.strName = temp;

		GetServiceStep(i, STR_KEY_SIMG, temp);
		step.strSearchImage = temp;

		GetServiceStep(i, STR_KEY_TH, temp);
		step.dThreshold = (double)_ttof(temp);

		GetServiceStep(i, STR_KEY_OPTION, temp);
		step.ReadOption(temp);

		GetServiceStep(i, L"ParamStr", temp);
		step.strParam = temp;
		GetServiceStep(i, L"ParamStr2", temp);
		step.strParam2 = temp;
		GetServiceStep(i, L"ParamStr3", temp);
		step.strParam3 = temp;

		GetServiceStep(i, L"OK", temp);
		step.nOK = _ttoi(temp);
		GetServiceStep(i, L"NG", temp);
		step.nNG = _ttoi(temp);
		GetServiceStep(i, L"Err", temp);
		step.nErr = _ttoi(temp);

		GetServiceStep(i, L"StartTime", temp);
		step.nStartTime = _ttoi(temp);
		GetServiceStep(i, L"TimeOut", temp);
		step.nTimeOut = _ttoi(temp);

		m_cService.vecStep.push_back(step);
	}

	///////////////////////////////////////////////////////////////////////////
	// 復旧処理
	m_cRescue.Clear();

	// ====================================
	// 復旧ステップ読込み
	for (int i = 1; i < 30; i++)
	{
		TASK_STEP step;
		GetRescueStep(i, STR_KEY_TYPE, temp);
		step.type = _ttoi(temp);

		if (step.type == 0)
		{
			continue;
		}
		step.nNumber = 1;

		GetRescueStep(i, STR_KEY_NAME, temp);
		step.strName = temp;

		GetRescueStep(i, STR_KEY_SIMG, temp);
		step.strSearchImage = temp;

		GetRescueStep(i, STR_KEY_TH, temp);
		step.dThreshold = (double)_ttof(temp);

		GetRescueStep(i, STR_KEY_OPTION, temp);
		step.ReadOption(temp);

		GetRescueStep(i, L"ParamStr", temp);
		step.strParam = temp;
		GetRescueStep(i, L"ParamStr2", temp);
		step.strParam2 = temp;
		GetRescueStep(i, L"ParamStr3", temp);
		step.strParam3 = temp;

		GetRescueStep(i, L"OK", temp);
		step.nOK = _ttoi(temp);
		GetRescueStep(i, L"NG", temp);
		step.nNG = _ttoi(temp);
		GetRescueStep(i, L"Err", temp);
		step.nErr = _ttoi(temp);

		GetRescueStep(i, L"StartTime", temp);
		step.nStartTime = _ttoi(temp);
		GetRescueStep(i, L"TimeOut", temp);
		step.nTimeOut = _ttoi(temp);

		m_cRescue.vecStep.push_back(step);
	}

}


/**
 * ステップ実行
 */
void CMainDlg::OnBnClickedDostepButton()
{
	int nIdx = m_cmbTask.GetCurSel();

	if (m_cmbTask.GetCount() == 0 || nIdx < 0)
	{
		return;
	}

	if (m_hTaskWindow == NULL)
	{
		UpdateTargetWindow();
		if (m_hTaskWindow == NULL)
		{
			::AfxMessageBox(_T("ウィンドウがみつかりません。"),MB_OK);
			return;
		}
	}

	CString temp;
	m_cmbTask.GetWindowText(temp);
	CString str;
	str.Format(_T("ステップ実行 : %s"), temp);
	AddLog(str);

	static int nStepState = TASK_STATE::STAT_INIT;
	static int nTempIdx = -1;
	if (nTempIdx != nIdx)
	{
		nTempIdx = nIdx;
		nStepState = TASK_STATE::STAT_INIT;
	}

	CString strSnapshot;
	int nRet = DoStepSub(m_cStep.vecStep[nIdx],strSnapshot, nStepState);
	switch(nRet)
	{// 
	case RET_OK:
		AddLog(_T("〇 ステップ成功"));
		nIdx++;
		if (nIdx < m_cmbTask.GetCount())
		{
			m_cmbTask.SetCurSel(nIdx);
		}
		else
		{
			m_cmbTask.SetCurSel(0);
		}
		break;
	case RET_WAIT:
		AddLog(_T("- ステップ完了待ち"));
		break;
	case RET_NG:
		AddLog(_T("▲ ステップ失敗"));
		break;
	case RET_ERR:
		AddLog(_T("× ステップエラー"));
		break;
	case RET_RESET:
		AddLog(_T("〇 リセット要求"));
		m_cmbTask.SetCurSel(0);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// ウィンドウ列挙

/**
* ウィンドウ列挙
*/
BOOL CALLBACK CMainDlg::EnumWindowsProcTask(HWND hWnd, LPARAM lp)
{
	try
	{
		if (hWnd)
		{
			CMainDlg* pDlg = reinterpret_cast<CMainDlg*>(lp);
			if (pDlg)
			{
				CString str, title;
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				if (pWnd)
				{
					// ウィンドウタイトル
					pWnd->GetWindowText(title);
					if (pDlg->IsSearchWndTask(title))
					{
						pDlg->SetFindWindowTask(hWnd);
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
BOOL CALLBACK CMainDlg::EnumWindowsProcTask2(HWND hWnd, LPARAM lp)
{
	try
	{
		if (hWnd)
		{
			CMainDlg* pDlg = reinterpret_cast<CMainDlg*>(lp);
			if (pDlg)
			{
				CString str, title;
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				if (pWnd)
				{
					HWND hParent = ::GetParent(hWnd);
					if (pDlg->GetTaskWindow() == hParent)
					{
						// ウィンドウタイトル
						pWnd->GetWindowText(title);
						if (pDlg->IsSearchWndChildTask(title)	)
						{
							pDlg->SetFindWindowChildTask(hWnd);
						}
						else if (title == _T("Nox"))
						{
							pDlg->SetFindWindowToolTask(hWnd);
						}
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
* ウィンドウ列挙
*/
BOOL CALLBACK CMainDlg::EnumWindowsProcTaskR(HWND hWnd, LPARAM lp)
{
	try
	{
		if (hWnd)
		{
			CMainDlg* pDlg = reinterpret_cast<CMainDlg*>(lp);
			if (pDlg)
			{
				CString str, title;
				CWnd* pWnd = CWnd::FromHandle(hWnd);
				if (pWnd)
				{
					// ウィンドウタイトル
					pWnd->GetWindowText(title);
					if (pDlg->IsSearchWndTaskR(title))
					{
						pDlg->MoveWindowTaskR(hWnd);
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
void CMainDlg::SetFindWindowTask(HWND hWnd)
{
	if (m_hTaskWindow != NULL)
	{
		return;
	}
	m_hTaskWindow = hWnd;

	::GetWindowText(hWnd, m_strTaskWindowName.GetBuffer(1024), 1024);
	m_strTaskWindowName.ReleaseBuffer();

	CWnd* pWnd = CWnd::FromHandle(hWnd);

	CString str, title;
	CRect rc;
	pWnd->GetWindowText(title);
	pWnd->GetWindowRect(rc);
	str.Format(_T("Find! [%08x(%d,%d)-(%d,%d)] %s "), hWnd, rc.left, rc.top, rc.Width(), rc.Height(), title);
	AddLog(str);

	str.Format(_T("%x"), hWnd);
	m_edtTaskWnd.SetWindowText(str);

	if (!m_cStep.strChildWndSearch.IsEmpty())
	{
		SetFindWindowToolTask(NULL);
		// 小窓検索開始
		EnumWindows(EnumWindowsProcTask2, (LPARAM)this);
	}
}

/**
* 見つかったウィンドウをセット
*/
void CMainDlg::SetFindWindowChildTask(HWND hWnd)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);

	CString str, title;
	CRect rc;
	pWnd->GetWindowText(title);
	pWnd->GetWindowRect(rc);
	str.Format(_T("Find Child! [%08x(%d,%d)-(%d,%d)] %s "), hWnd, rc.left, rc.top, rc.Width(), rc.Height(), title);
	AddLog(str);

	m_edtTaskWnd.GetWindowText(str);
	HWND hParent = (HWND)wcstol(str, NULL, 16);

	CWnd* pParent = pWnd->GetParent();
	if (hParent && pParent && pParent->GetSafeHwnd() == hParent)
	{
		m_hTaskWndChild = hWnd;
	}
}

//////////////////////////////////////////////////////////////////////////////

/**
* 戻る
*/
void CMainDlg::OnBnClickedBackStepButton()
{
	m_cTaskState.BackStep();
	UpdateTaskState();
}


/**
* 次へ
*/
void CMainDlg::OnBnClickedNextStepButton()
{
	m_cTaskState.NextStep();
	UpdateTaskState();
}

/**
*
*/
void CMainDlg::OnBnClickedNextStepButton2()
{
	m_cTaskState.NextStep(10);
	UpdateTaskState();
}

/**
 * 
 */
void CMainDlg::OnBnClickedBackStepButton2()
{
	m_cTaskState.BackStep(10);
	UpdateTaskState();
}

/**
 * 
 */
void CMainDlg::OnBnClickedBackStepButton3()
{
	m_cTaskState.BackStep(m_cTaskState.nIdx);
	UpdateTaskState();
}

/**
 * 
 */
void CMainDlg::OnBnClickedNextStepButton3()
{
	m_cTaskState.NextStep(m_cStep.vecStep.size() - m_cTaskState.nIdx);
	UpdateTaskState();
}

	
void CMainDlg::DoWndHold()
{
	do
	{
		if (m_bADB)return;

		HWND hTarget = GetTaskWindow();
		if (hTarget == NULL)
		{
			break;
		}
		CRect rc;
		::GetWindowRect(hTarget, &rc);

		if (!m_nWndHold)
		{
			if (rc.TopLeft() == m_rcTaskWnd.TopLeft())
			{
				break;
			}
			// 位置が変わっているなら移動する
			CPoint posMove = m_rcTaskWnd.TopLeft();

			::SetWindowPos(hTarget,
				NULL,
				posMove.x,
				posMove.y,
				0, 0,
				SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		}
		else {
			if (rc == m_rcTaskWnd)
			{
				break;
			}
			// 位置サイズが変わっているなら戻す
			CPoint posMove = m_rcTaskWnd.TopLeft();

			::SetWindowPos(hTarget,
				NULL,
				m_rcTaskWnd.left,
				m_rcTaskWnd.top,
				m_rcTaskWnd.Width(), 
				m_rcTaskWnd.Height(),
				SWP_NOACTIVATE | SWP_NOZORDER);

		}


		if (::IsIconic(hTarget)) {
			::ShowWindow(hTarget, SW_RESTORE);
		}
		if(rc.Width() > 1200){
			// ウィンドウ幅が1200px 以上ならプロセスキル
			DoProcessKill();
			return;
		}
		// 右ウィンドウも動かす
		if (!m_cStep.strWndSearchR.IsEmpty()) {
			AddLog(_T("Right Window Search Start."));
			EnumWindows(EnumWindowsProcTaskR, (LPARAM)this);
		}
	} while (FALSE);

}

/**
* 見つかったウィンドウを移動
*/
void CMainDlg::MoveWindowTaskR(HWND hWnd)
{
	HWND hTarget = GetTaskWindow();
	if (hTarget == NULL)
	{
		return;
	}
	CRect rc;
	::GetWindowRect(hTarget, &rc);

	//AddLog(_T("Move Right Window."));
	::SetWindowPos(hWnd,
		NULL,
		rc.right,
		rc.top,
		0, 0,
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}