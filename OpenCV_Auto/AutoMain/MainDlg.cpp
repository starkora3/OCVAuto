
// OpenCVSample001Dlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "SubDlg.h"

#include <opencv2/core.hpp>    // coreモジュールのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiモジュールのヘッダーをインクルード
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv/highgui.h>

//#include <gdiplus.h>

#include <winsock2.h>
#include <ws2tcpip.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;


// CMainDlg ダイアログ


CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CResizeDialog(IDD_OPENCVSAMPLE001_DIALOG, pParent), m_pSubDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_EDIT, m_edtLog);
	DDX_Control(pDX, IDC_TASK_COMBO, m_cmbTask);
	DDX_Control(pDX, IDC_TASKWND_EDIT, m_edtTaskWnd);
	DDX_Control(pDX, IDC_CURRENT_STEP_STATIC, m_cCurStep);
	DDX_Control(pDX, IDC_SELECT_TASKFILE_COMBO, m_cmbSelectTask);
	DDX_Control(pDX, IDC_SERVICE_CHECK, m_chkService);
	DDX_Control(pDX, IDC_SELECT_NAME_COMBO, m_cmbName);
	DDX_Control(pDX, IDC_PRETIME_EDIT, m_edtPreTime);
}

BEGIN_MESSAGE_MAP(CMainDlg, CResizeDialog)
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DOSTEP_BUTTON, &CMainDlg::OnBnClickedDostepButton)
	ON_BN_CLICKED(IDC_TASKLOAD_BUTTON, &CMainDlg::OnBnClickedTaskloadButton)
	//ON_BN_CLICKED(IDC_TASKSTART_BUTTON, &CMainDlg::OnBnClickedTaskstartButton)
	ON_BN_CLICKED(IDC_BACK_STEP_BUTTON, &CMainDlg::OnBnClickedBackStepButton)
	ON_BN_CLICKED(IDC_NEXT_STEP_BUTTON, &CMainDlg::OnBnClickedNextStepButton)
	ON_BN_CLICKED(IDC_NEXT_STEP_BUTTON2, &CMainDlg::OnBnClickedNextStepButton2)
	ON_BN_CLICKED(IDC_BACK_STEP_BUTTON2, &CMainDlg::OnBnClickedBackStepButton2)
	ON_BN_CLICKED(IDC_BACK_STEP_BUTTON3, &CMainDlg::OnBnClickedBackStepButton3)
	ON_BN_CLICKED(IDC_NEXT_STEP_BUTTON3, &CMainDlg::OnBnClickedNextStepButton3)
	ON_REGISTERED_MESSAGE(CProgEx::m_uWM_PROGRESSEX, OnBtnProgress)
	ON_BN_CLICKED(IDC_SUBDLG_BUTTON, &CMainDlg::OnBnClickedSubdlgButton)
	ON_MESSAGE(CSubDlg::MSG_SUBDLG,&CMainDlg::OnSubDlgEvent)
	ON_CBN_SELCHANGE(IDC_SELECT_TASKFILE_COMBO, &CMainDlg::OnCbnSelchangeSelectTaskfileCombo)
	ON_CBN_EDITCHANGE(IDC_SELECT_NAME_COMBO, &CMainDlg::OnCbnEditchangeSelectNameCombo)
	ON_BN_CLICKED(IDC_EXEC_BUTTON, &CMainDlg::OnBnClickedExecButton)
	ON_BN_CLICKED(IDC_PKILL_BUTTON, &CMainDlg::OnBnClickedPkillButton)
	ON_BN_CLICKED(IDC_EXTEND_BUTTON, &CMainDlg::OnBnClickedExtendButton)
	ON_BN_CLICKED(IDC_DESKSEND_BUTTON, &CMainDlg::OnBnClickedDesksendButton)
END_MESSAGE_MAP()


// CMainDlg メッセージ ハンドラー

/**
 * Init
 **/
BOOL CMainDlg::OnInitDialog()
{
	CResizeDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	InitLoadSetting();

	ComFunc::GetTemporaryPath(m_strTempDir);

	DragAcceptFiles();

	m_cCurStep.SetText(STR_TASK_STOP);

	// タスクの読み込み
	UpdateTask();

	m_chkService.SetCheck(1);
	m_edtPreTime.SetWindowText(_T("0"));
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CMainDlg::OnClose()
{
	try
	{
		if (m_pSubDlg)
		{
			m_pSubDlg->SendMessage(WM_CLOSE, 0, 0);
			delete m_pSubDlg;
			m_pSubDlg = NULL;
		}
		// テンポラリクリア
		ComFunc::CleanupFiles();
	}
	catch (...)
	{

	}
	__super::OnClose();
}


#define BUFFLEN 8192*8

/**
* ログ追加
*/
void CMainDlg::AddLog(LPCTSTR str)
{
	// 現在の表示分を取得

	time_t timer;
	struct tm newtime;

	/* 現在時刻の取得 */
	time(&timer);

	/* 現在時刻を構造体に変換 */
	localtime_s(&newtime, &timer);

	CString line;
	line.Format( _T("%02d/%02d %02d:%02d:%02d %s\r\n")
//		, newtime.tm_year + 1900
		, newtime.tm_mon + 1
		, newtime.tm_mday
		, newtime.tm_hour
		, newtime.tm_min
		, newtime.tm_sec
		, str);

	m_strLog = line + m_strLog;

	if (m_strLog.GetLength() > BUFFLEN)
	{
		m_strLog = m_strLog.Left(BUFFLEN);
	}
	m_edtLog.SetWindowText(m_strLog);
}


/**
 * プログレスバーイベント
 */
LRESULT CMainDlg::OnBtnProgress(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)
	{// 左クリック
		OnBnClickedTaskstartButton();
	}
	else
	{// 右クリック
		OnBnClickedResetButton();
	}
	return 0;
}


void CMainDlg::OnBnClickedSubdlgButton()
{
	if (m_pSubDlg ==NULL)
	{
		//子作成
		m_pSubDlg = new CSubDlg(this);
		if (m_pSubDlg)
		{
			m_pSubDlg->Create(IDD_SUB_DIALOG, this);
			m_pSubDlg->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_pSubDlg->ShowWindow(SW_SHOW);
	}
}

LRESULT CMainDlg::OnSubDlgEvent(WPARAM evt, LPARAM)
{
	try
	{
		switch (evt)
		{
		case CSubDlg::EVENT_CLOSE:
			if (m_pSubDlg)
			{
				delete m_pSubDlg;
				m_pSubDlg = NULL;
			}
			break;
		}
	}
	catch (...)
	{
	}
	return 0;
}


void CMainDlg::OnCbnSelchangeSelectTaskfileCombo()
{
	OnBnClickedTaskloadButton();
}


void CMainDlg::OnCbnEditchangeSelectNameCombo()
{
	m_edtTaskWnd.SetWindowTextW(NULL);
	m_hTaskWindow = NULL;
}


void CMainDlg::OnBnClickedExecButton()
{
	CString strPath;
	DWORD dwResult = ::GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();
	if (dwResult == 0)return;

	::ShellExecute(NULL,_T("open"), strPath, NULL, NULL, 0);
}


void CMainDlg::OnBnClickedPkillButton()
{
	DoProcessKill();
}


void CMainDlg::OnBnClickedExtendButton()
{// 拡張展開ボタン
	try {
		CRect rc;
		GetWindowRect(&rc);

		SetWindowPos(NULL, 0, 0, rc.Width() + 150, rc.Height(), SWP_NOMOVE);


	}
	catch (CException* p) {

	}

}

/**
 * ローカルIP の取得
 */
CString CMainDlg::GetLocalIPAddr() {
	CString strIP;
	do {
		WSADATA wsadata;
		if (WSAStartup(MAKEWORD(1, 1), &wsadata) != 0) {
			break;
		}

		//ホスト名を取得する
		char hostname[256];
		if (gethostname(hostname, sizeof(hostname)) != 0) {
			break;
		}

		//ホスト名からIPアドレスを取得する
		struct addrinfo hints, * res;
		::ZeroMemory(&hints, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_family = AF_INET;

		if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
			break;
		}
		IN_ADDR inaddr;
		inaddr.S_un = ((sockaddr_in*)res->ai_addr)->sin_addr.S_un;

		freeaddrinfo(res);

		char pBuff[256];
		// IPv4 アドレスを文字列変換してバッファーに格納します。
		if (inet_ntop(AF_INET, &inaddr, pBuff, 256) == NULL) {
			break;
		}

		strIP = CString(pBuff);
	} while (FALSE);
	WSACleanup();
	return strIP;
}

/**
* 状態送信
*/
void CMainDlg::OnBnClickedDesksendButton()
{
	CString str = GetLocalIPAddr();
	AddLog(_T("Source IP: ") + str);
	DoDesktopPostSub(m_strStateSendURL,str);
}
