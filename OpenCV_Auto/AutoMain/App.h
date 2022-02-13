
// OpenCVSample001.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// COpenCVSample001App:
// このクラスの実装については、OpenCVSample001.cpp を参照してください。
//

class COpenCVSample001App : public CWinApp
{
public:
	COpenCVSample001App();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern COpenCVSample001App theApp;