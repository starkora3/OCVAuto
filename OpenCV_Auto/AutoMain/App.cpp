
// OpenCVSample001.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <opencv2/core.hpp>    // coreモジュールのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiモジュールのヘッダーをインクルード


#ifdef _DEBUG
#pragma comment(lib,"opencv_bgsegm450d.lib")
#pragma comment(lib,"opencv_bioinspired450d.lib")
#pragma comment(lib,"opencv_calib3d450d.lib")
#pragma comment(lib,"opencv_ccalib450d.lib")
#pragma comment(lib,"opencv_core450d.lib")
#pragma comment(lib,"opencv_datasets450d.lib")
#pragma comment(lib,"opencv_face450d.lib")
#pragma comment(lib,"opencv_features2d450d.lib")
#pragma comment(lib,"opencv_flann450d.lib")
//#pragma comment(lib,"opencv_hal450d.lib")
#pragma comment(lib,"opencv_highgui450d.lib")
#pragma comment(lib,"opencv_imgcodecs450d.lib")
#pragma comment(lib,"opencv_imgproc450d.lib")
//#pragma comment(lib,"opencv_latentsvm450d.lib")
#pragma comment(lib,"opencv_line_descriptor450d.lib")
#pragma comment(lib,"opencv_ml450d.lib")
#pragma comment(lib,"opencv_objdetect450d.lib")
#pragma comment(lib,"opencv_optflow450d.lib")
#pragma comment(lib,"opencv_photo450d.lib")
#pragma comment(lib,"opencv_reg450d.lib")
#pragma comment(lib,"opencv_rgbd450d.lib")
#pragma comment(lib,"opencv_saliency450d.lib")
#pragma comment(lib,"opencv_shape450d.lib")
#pragma comment(lib,"opencv_stitching450d.lib")
#pragma comment(lib,"opencv_superres450d.lib")
#pragma comment(lib,"opencv_surface_matching450d.lib")
#pragma comment(lib,"opencv_text450d.lib")
#pragma comment(lib,"opencv_tracking450d.lib")
//#pragma comment(lib,"opencv_ts450d.lib")
#pragma comment(lib,"opencv_video450d.lib")
#pragma comment(lib,"opencv_videoio450d.lib")
#pragma comment(lib,"opencv_videostab450d.lib")
#pragma comment(lib,"opencv_ximgproc450d.lib")
#pragma comment(lib,"opencv_xobjdetect450d.lib")
#pragma comment(lib,"opencv_xphoto450d.lib")
#else
#pragma comment(lib,"opencv_bgsegm450.lib")
#pragma comment(lib,"opencv_bioinspired450.lib")
#pragma comment(lib,"opencv_calib3d450.lib")
#pragma comment(lib,"opencv_ccalib450.lib")
#pragma comment(lib,"opencv_core450.lib")
#pragma comment(lib,"opencv_datasets450.lib")
#pragma comment(lib,"opencv_face450.lib")
#pragma comment(lib,"opencv_features2d450.lib")
#pragma comment(lib,"opencv_flann450.lib")
//#pragma comment(lib,"opencv_hal450.lib")
#pragma comment(lib,"opencv_highgui450.lib")
#pragma comment(lib,"opencv_imgcodecs450.lib")
#pragma comment(lib,"opencv_imgproc450.lib")
//#pragma comment(lib,"opencv_latentsvm450.lib")
#pragma comment(lib,"opencv_line_descriptor450.lib")
#pragma comment(lib,"opencv_ml450.lib")
#pragma comment(lib,"opencv_objdetect450.lib")
#pragma comment(lib,"opencv_optflow450.lib")
#pragma comment(lib,"opencv_photo450.lib")
#pragma comment(lib,"opencv_reg450.lib")
#pragma comment(lib,"opencv_rgbd450.lib")
#pragma comment(lib,"opencv_saliency450.lib")
#pragma comment(lib,"opencv_shape450.lib")
#pragma comment(lib,"opencv_stitching450.lib")
#pragma comment(lib,"opencv_superres450.lib")
#pragma comment(lib,"opencv_surface_matching450.lib")
#pragma comment(lib,"opencv_text450.lib")
#pragma comment(lib,"opencv_tracking450.lib")
//#pragma comment(lib,"opencv_ts450.lib")
#pragma comment(lib,"opencv_video450.lib")
#pragma comment(lib,"opencv_videoio450.lib")
#pragma comment(lib,"opencv_videostab450.lib")
#pragma comment(lib,"opencv_ximgproc450.lib")
#pragma comment(lib,"opencv_xobjdetect450.lib")
#pragma comment(lib,"opencv_xphoto450.lib")
#endif

#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
using namespace Gdiplus;

BEGIN_MESSAGE_MAP(COpenCVSample001App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COpenCVSample001App コンストラクション

COpenCVSample001App::COpenCVSample001App()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の COpenCVSample001App オブジェクトです。

COpenCVSample001App theApp;


// COpenCVSample001App 初期化

BOOL COpenCVSample001App::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	AfxEnableControlContainer();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	GdiplusShutdown(gdiplusToken);

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

