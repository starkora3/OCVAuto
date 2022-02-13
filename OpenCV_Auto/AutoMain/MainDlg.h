
// OpenCVSample001Dlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "DlgResize.h"
#include "ComFunc.h"

#include "DataDef.h"
#include "afxcmn.h"
#include "staticex.h"
#include "AppDef.h"

#define FEHAUTO_GLOBAL_MUTEX _T("Global\\FEHAUTO_MUTEX")

using namespace OCVAUTO;
class CSubDlg;

#define STR_APP_TITLE _T("自動化")
#define STR_TASK_STOP _T("■ 停止中")

#define TYPE_PNG 0
#define TYPE_JPEG 1

class CMutexCreator {
public:
	CMutexCreator() {
		m_hMutex = CreateMutex(NULL, FALSE, FEHAUTO_GLOBAL_MUTEX);
	}
	virtual ~CMutexCreator() {
		if (m_hMutex) {
			::CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}
private:
	HANDLE m_hMutex;
};

class CMutexHolder {
public:
	CMutexHolder() {
		m_hMutex = NULL;
		HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, FEHAUTO_GLOBAL_MUTEX);
		if (hMutex==NULL)
		{
			if (::GetLastError() == ERROR_ALREADY_EXISTS)
			{
				m_hMutex = NULL;
			}
		}
		else
		{
			m_hMutex = hMutex;
			WaitForSingleObject(m_hMutex, INFINITE);
		}
	}

	virtual ~CMutexHolder() {
		if (m_hMutex)
		{
			::ReleaseMutex(m_hMutex);
			::CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}

	BOOL IsEnable()const {
		return (m_hMutex) ? TRUE : FALSE;
	}
private:
	HANDLE m_hMutex;
	BOOL m_bEnable;
};

// CMainDlg ダイアログ
class CMainDlg : public CResizeDialog
{
// コンストラクション
public:
	CMainDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENCVSAMPLE001_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

	CMutexCreator m_cMutex;

	/******************************************/
	//! コントロールのエントリー処理
	virtual void InitAdjustCtrl() 
	{// 実装サンプル
		AddAdjustCtrl(IDC_LOG_EDIT, ADJUST_TBLR);
		//AddAdjustCtrl(IDOK, ADJUST_TR);
		//AddAdjustCtrl(IDC_OPEN_SRC_BUTTON, ADJUST_TR);
		//AddAdjustCtrl(IDC_OPEN_SEARCH_BUTTON, ADJUST_TR);
//		AddAdjustCtrl(IDC_WND_SEARCH_BUTTON, ADJUST_TR);

		//AddAdjustCtrl(IDC_EDIT1,ADJUST_TLR);
		//AddAdjustCtrl(IDC_EDIT2,ADJUST_TLR);

		//AddAdjustCtrl(IDC_THRESHOLD_STATIC, ADJUST_TR);
		//AddAdjustCtrl(IDC_THRESHOLD_EDIT, ADJUST_TR);

		AddAdjustCtrl(IDC_TASK_COMBO, ADJUST_TBL);

		
			
		// ウィンドウ最小サイズを設定
		CRect rc;
		GetWindowRect(rc);
		m_szWndMin = rc.Size();
	}
	/******************************************/
// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	//--------------------------
public:
	//! 設定
	BOOL GetSetting(
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""));
	//! 設定
	BOOL SetSetting(
		LPCTSTR key,
		LPCTSTR strValue);
protected:
	void InitLoadSetting();

	//! 設定
	BOOL GetTask(
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""),
		LPCTSTR lpszPath = _T(""));

	//! 設定
	BOOL GetStep(
		int nIdx,
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""));

	BOOL GetStepSub(
		LPCTSTR name,
		int nIdx,
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""));

	//! サービスステップ取得
	BOOL GetServiceStep(
		int nIdx,
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""));
	//! 復旧ステップ取得
	BOOL GetRescueStep(
		int nIdx,
		LPCTSTR key,
		CString& strValue,
		LPCTSTR lpszDefault = _T(""));

	//! 設定
	CString m_strIniFile;
	CString m_strIniDir;

	struct DATAFILE {
		CString disp;
		CString path;
		DATAFILE() {
			disp = _T("デフォルト");
		}
	};
	typedef std::vector<DATAFILE> VEC_DFILE;
	VEC_DFILE m_vecDataFile;
	DATAFILE m_cCurDFile;
public:
	//! Nox デバイス取得
	BOOL GetNoxDevice(
		LPCTSTR key,
		CString& strValue);
	//! ADB デバイス 取得
	BOOL GetADBDevice(
		LPCTSTR search,
		CString& strValue);

	//----------------------------
public:

	//! ソート
	static bool ascTop(const FINDDATA& left, const FINDDATA& right);

	//! 画像検索
	int DoSearch(IMAGE_FIND& ret, 
						LPCTSTR src,
						LPCTSTR target,
						double dThreshold = 0.5, 
						double dOverlap=0.5);


	//! テンポラリ
	CString m_strTempDir;
	LPCTSTR GetTempDir()const { return m_strTempDir; };

	//! ログ
	void AddLog(LPCTSTR str);
	CString m_strLog;
	CEdit m_edtLog;

	//! Goto
	void StepGoto(int nGoto) {
		int nIdx = 0;
		for (int i = 0; i<(int)m_cStep.vecStep.size(); i++)
		{
			if (nGoto == m_cStep.vecStep[i].nNumber)
			{// 発見
				nIdx = i;
				break;
			}
		}
		m_cTaskState.SetStep(nIdx);
	}

	// -----------------------------------------------------------

	//! タスク
	BOOL IsSearchWndTask(LPCTSTR str) {
		CString title(str);
		title.MakeLower();
		return (title.Find(m_cStep.strWndSearch) >= 0) ? TRUE : FALSE;
	}
	//! タスク
	BOOL IsSearchWndChildTask(LPCTSTR str) {
		CString title(str);
		title.MakeLower();
		return (title.Find(m_cStep.strChildWndSearch) >= 0) ? TRUE : FALSE;
	}
	//! タスク
	BOOL IsSearchWndTaskR(LPCTSTR str) {
		CString title(str);
		title.MakeLower();
		return (title.Find(m_cStep.strWndSearchR) >= 0) ? TRUE : FALSE;
	}

	// ------------------------------------------------------------

	static BOOL CALLBACK EnumWindowsProcTask(HWND hwnd, LPARAM lp);
	static BOOL CALLBACK EnumWindowsProcTask2(HWND hwnd, LPARAM lp);
	static BOOL CALLBACK EnumWindowsProcTaskR(HWND hwnd, LPARAM lp);


	//! タスク
	void SetFindWindowTask(HWND hWnd);
	HWND GetTaskWindow()const {return m_hTaskWindow;	}
	void SetFindWindowChildTask(HWND hWnd);
	void SetFindWindowToolTask(HWND hWnd) { m_hTaskWindowTool = hWnd; };

	void MoveWindowTaskR(HWND hWnd);

	HWND m_hTaskWindow;
	CRect m_rcTaskWnd;

	CString m_strTaskWindowName;
	HWND m_hTaskWindowTool;

	void AddChildWindow(CWnd* pChild, int nDepsth);

	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR);
	void OnBnClickedResetButton();
	afx_msg void OnBnClickedDostepButton();
	afx_msg void OnBnClickedTaskloadButton();
	afx_msg void OnBnClickedTaskstartButton();

	void UpdateTargetWindow();
	void DoWndHold();

	//! ステップ実行
	int DoStep(CString& strSnapShot,int& nStepState);
	//! サービス実行
	BOOL DoService(CString& strSnapShot, TASK_STEP& cExecStep);

	//! 復旧処理実行
	void DoRescue();

//	CWnd* GetClientWnd();

	//! 
	BOOL CreateSnapShot(CWnd* pTarget, CString& strFile, CString& strErr,int nType=TYPE_PNG);
	BOOL CreateSnapShotDirect(CWnd* pTarget, CString& strFile, CString& strErr);

	//! ADB スナップの取得
	BOOL CreateSnapShotADB(CString& strFile, 
		CString& strErr,
		LPCTSTR device=NULL);

	BOOL DoADBCommand(CString cmd,LPCTSTR device=NULL);


	CString GetSettingFile(LPCTSTR strName = _T(""));
	CComboBox m_cmbTask;

	LPCTSTR GetScreenShotPath();
	CString m_strSavePath;

	void UpdateTask();

	//! タスク状態管理
	TASK_STATE m_cTaskState;
	//! タイマーID
	UINT m_uTimer;
	UINT m_uTimerCounter;

	TASK_DATA m_cStep;
	TASK_DATA m_cService;
	TASK_DATA m_cRescue;

	//! ステップ実行
	int DoStepSub(const TASK_STEP& step, CString& strSnapShot,int& nState);

	//! 
	static void DoMouseMove(int x, int y);

	//! 
	void DoLButtonDownUp(int nSleep=0);
	void DoLButtonDown();
	void DoLButtonUp();

	//! 1,終了する
	int DoEnd();
	//! 2, スナップショットをとる
	CString DoSnapShot(LPCTSTR pre = _T("ss"),
						LPCTSTR name=_T("manual"),
						HWND hTarget=NULL,
						int nType=TYPE_PNG);
	//! 3, クリック
	int DoClick(const TASK_STEP& step);
	//! 4, クリック
	int DoDClick(const TASK_STEP& step);

	//! 10, シェル実行
	int DoShell(const TASK_STEP& step);
	//! 11, アカウント取得
	int DoAccGet(const TASK_STEP& step);
	//! 12, アカウント次へ
	int DoAccNext(const TASK_STEP& step);
	//! 13, アカウント入力
	int DoAccInput(const TASK_STEP& step);

	//! 15, Noxシェル実行
	int DoNoxShell(const TASK_STEP& step);

	//! アカウント日付取得
	CString GetAccDate();

	//! 21, ウィンドウから対象画像(複数)を探して、あればOK
	int DoSearchBorderExist(const TASK_STEP& step, CString& strSnapShot);
	//! 22, ウィンドウから対象画像(複数)を探して、なければOK
	int DoSearchBorderNone(const TASK_STEP& step, CString& strSnapShot);
	//! 23, ウィンドウから対象画像を探して、ボーダー以上ならクリックしてOKを返す
	int DoSearchBorderOverClickOK(const TASK_STEP& step, CString& strSnapShot);
	//! 24, ウィンドウから対象画像を探して、ボーダー以上ならクリックしてNGを返す
	int DoSearchBorderOverClickNG(const TASK_STEP& step, CString& strSnapShot);
	//! 25, ウィンドウから対象画像を探して、ボーダー以上ならクリック ＆サブ項目もクリック
	int DoSearchBorderOverClick2(const TASK_STEP& step, CString& strSnapshot);

	//! 31, 位置間ドラッグ
	int DoDrugP2P(const TASK_STEP& step, CString& strSnapshot);

	//! 81, 子ウィンドウから対象画像を探して、縦n番目をクリック
	int DoStepChildSearchClickV(const TASK_STEP& step);
	//! 82, 子ウィンドウから対象画像を探して、なければOK、
	int DoStepChildSearchNone(const TASK_STEP& step);
	//! 83, 子ウィンドウから対象画像を探して、あればOK
	int DoStepChildSearchExist(const TASK_STEP& step);

	//! 101 
	int DoMoveL(const TASK_STEP& step, CString& strSnapshot,int& nState);
	int DoMoveR(const TASK_STEP& step, CString& strSnapshot, int& nState);
	int DoMoveU(const TASK_STEP& step, CString& strSnapshot, int& nState);
	int DoMoveD(const TASK_STEP& step, CString& strSnapshot, int& nState);

	int DoMoveLU(const TASK_STEP& step, CString& strSnapshot, int& nState);
	int DoMoveLD(const TASK_STEP& step, CString& strSnapshot, int& nState);
	int DoMoveRU(const TASK_STEP& step, CString& strSnapshot, int& nState);
	int DoMoveRD(const TASK_STEP& step, CString& strSnapshot, int& nState);

	//! 201 
	int DoKeyDownUp(const TASK_STEP& step);
	int DoKeyDown(const TASK_STEP& step);
	int DoKeyUp(const TASK_STEP& step);

	//! 501, メモリチェック
	int DoMemoryCheck(const TASK_STEP& step);
	//! 502, 状態送信
	int DoDesktopPost(const TASK_STEP& step);
	//! 状態送信の実行
	int DoDesktopPostSub(LPCTSTR url,
		LPCTSTR fname,
		int nType = TYPE_PNG);

	//! IPアドレスの取得
	static CString GetLocalIPAddr();

	//! 800
	int DoADBPSStart(const TASK_STEP& step);
	int DoADBPSCheck(const TASK_STEP& step);
	int DoADBPSKill(const TASK_STEP& step);

	//! 900
	int DoTaskKillW(const TASK_STEP& step);
	int DoTaskKill(const TASK_STEP& step);
	int DoKeyHome(const TASK_STEP& step);
	int DoKeyPSW(const TASK_STEP& step);
	int DoKeyInput(const TASK_STEP& step);

	int DoProcessKillService(const TASK_STEP& step );
	int DoProcessKill();

	//! ウィンドウアクティブ化(物理)
	CPoint DoActiveWindow(HWND hWnd, int nSleep=0);


	static int GetKey(LPCTSTR str);

	int ExeKeyDownUp(int nKey, int nSleep = 0);
	int ExeKeyDown(int nKey);
	int ExeKeyUp(int nKey);

	enum {
		MOVE_SUB_NONE=0,
		MOVE_SUB_UP,
		MOVE_SUB_DOWN,
	};
	int DoMoveSub( int nKey, 
							const TASK_STEP& step, 
							CString& strSnapshot, 
							int& nState,
							int nSub= MOVE_SUB_NONE);


//	int DoLClick(const TASK_STEP& step, CString& strSnapshot, int& nState);

	//! 対象画像(複数) を探す
	BOOL DoSearchFiles(
								CString strSnapShot, 
								CString strFiles,
								double dThreshold,
								CRect& rc);

	BOOL mouseMove(HWND hWnd, int nKey, int nSleep);
	BOOL keyDown(WORD wKey);
	BOOL keyUp(WORD wKey);
	BOOL keyDownUp(WORD wKey);
	BOOL keyDownTask(WORD* pKeyArray, int nSize);
	BOOL keyUpTask(WORD* pKeyArray, int nSize);
	BOOL keyTask(WORD* pKeyArray, int nSize, DWORD dwFlags=0);

	CEdit m_edtTaskWnd;
	HWND m_hTaskWndChild;

	//! ファイル削除キャッシュ
	DELETE_CACHE m_cDeleteCache;

	void ActLButtonClick(HWND, CPoint pos, int nSleep = 1, BOOL bMsg = TRUE);
	void ActLButtonDClick(HWND,CPoint pos,int nSleep=1, BOOL bMsg = TRUE);
	afx_msg void OnBnClickedBackStepButton();
	afx_msg void OnBnClickedNextStepButton();

	void UpdateTaskState();
	afx_msg void OnBnClickedNextStepButton2();
	afx_msg void OnBnClickedBackStepButton2();
	CProgEx m_cCurStep;
	afx_msg void OnBnClickedBackStepButton3();
	afx_msg void OnBnClickedNextStepButton3();

	afx_msg LRESULT OnBtnProgress(WPARAM wParam, LPARAM lParam);

	double m_dSleepRatio;
	int   m_nWndHold;
	CPoint m_posActivate;
	CComboBox m_cmbSelectTask;

	//! Nox専用モード
	BOOL m_bNox;
	//! ADB のみ
	BOOL m_bADB;
	CString m_strDevice;


	int m_nMouseMode;

	//! 状態送信URL
	CString m_strStateSendURL;
	UINT m_uStateSendCounter;

	//! アカウント
	CString m_strAccount;
	int m_nAccTimeDiff;
	CString m_strSendMessage;

	CSubDlg* m_pSubDlg;
	afx_msg void OnBnClickedSubdlgButton();
	afx_msg LRESULT OnSubDlgEvent(WPARAM, LPARAM);
	afx_msg void OnCbnSelchangeSelectTaskfileCombo();
	CButton m_chkService;
	CComboBox m_cmbName;
	afx_msg void OnCbnEditchangeSelectNameCombo();

	CString m_strLogFile;
	//void FlushLog();

	afx_msg void OnBnClickedExecButton();
	CEdit m_edtPreTime;
	afx_msg void OnBnClickedPkillButton();
	afx_msg void OnBnClickedExtendButton();
	afx_msg void OnBnClickedDesksendButton();
};
