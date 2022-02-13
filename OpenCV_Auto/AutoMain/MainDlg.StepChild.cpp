#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"

#include <opencv2/core.hpp>    // coreモジュールのヘッダーをインクルード
#include <opencv2/highgui.hpp> // highguiモジュールのヘッダーをインクルード
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;

/**
* 81, 子ウィンドウから対象画像を探して、縦n番目をクリック
*/
int CMainDlg::DoStepChildSearchClickV(const TASK_STEP& step)
{
	// 子ウィンドウスナップショット取得
	CWnd* pTarget = CWnd::FromHandle(m_hTaskWndChild);
	if (pTarget == NULL)return RET_ERR;
	CRect rc;
	pTarget->GetWindowRect(rc);
	if (rc.left < 0)
	{
		rc.left = 0;
	}
	if (rc.top < 0)
	{
		rc.top = 0;
	}

	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());

	CString strSearchImage = strPath + step.strSearchImage;
	CString strErr, strSnapShot;
	if (CreateSnapShot(pTarget, strSnapShot, strErr) == FALSE)
	{// 取得失敗
		return RET_ERR;
	}

	IMAGE_FIND cFindData;
	if (DoSearch(cFindData, strSnapShot, strSearchImage, step.dThreshold) < 0)
	{
		CFileOperation::DeleteFile(strSnapShot);
		return RET_ERR;
	}
	CFileOperation::DeleteFile(strSnapShot);

	if (cFindData.vecFind.size() < step.GetValue("find",1))
	{// 検出数が足りない
		return RET_NG;
	}

	VEC_FIND vec = cFindData.vecFind;
	std::sort(vec.begin(), vec.end(), ascTop);

	// クリックポイント取得
	CPoint pt = vec[step.GetValue("index")].rc.CenterPoint();

	// 子ウィンドウはメッセージだと反応しない
	ActLButtonClick(m_hTaskWndChild, rc.TopLeft() + pt, 0, FALSE);

	return RET_OK;
}

/**
* 82, 子ウィンドウから対象画像を探して、なければOK
*/
int CMainDlg::DoStepChildSearchNone(const TASK_STEP& step)
{
	int nRet = DoStepChildSearchExist(step);
	switch (nRet)
	{
	case RET_OK:
		return RET_NG;
	case RET_NG:
		return RET_OK;
	}
	return nRet;
}

/**
* 83, 子ウィンドウから対象画像を探して、あればOK
*/
int CMainDlg::DoStepChildSearchExist(const TASK_STEP& step)
{
	// 子ウィンドウスナップショット取得

	CWnd* pTarget = CWnd::FromHandle(m_hTaskWndChild);
	if (pTarget == NULL)return RET_ERR;
	CRect rc;
	pTarget->GetWindowRect(rc);
	if (rc.left < 0)
	{
		rc.left = 0;
	}
	if (rc.top < 0)
	{
		rc.top = 0;
	}

	CString strErr, strSnapShot;
	if (CreateSnapShot(pTarget, strSnapShot, strErr) == FALSE)
	{// 取得失敗
		return RET_ERR;
	}

	// 対象画像を探す
	CString strPath = CFileOperation::GetDirNameAtPath(GetSettingFile());
	CString strSearchImage = strPath + step.strSearchImage;

	IMAGE_FIND cFindData;
	if (DoSearch(cFindData, strSnapShot, strSearchImage, step.dThreshold) < 0)
	{
		CFileOperation::DeleteFile(strSnapShot);
		return RET_ERR;
	}
	CFileOperation::DeleteFile(strSnapShot);

	if (cFindData.vecFind.size())
	{// あるならOK
		return RET_OK;
	}
	// ないなら失敗
	return RET_NG;
}

