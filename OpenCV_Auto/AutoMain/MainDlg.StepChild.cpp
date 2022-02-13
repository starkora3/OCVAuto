#include "stdafx.h"
#include "App.h"
#include "MainDlg.h"
#include "FileOperation.h"

#include <opencv2/core.hpp>    // core���W���[���̃w�b�_�[���C���N���[�h
#include <opencv2/highgui.hpp> // highgui���W���[���̃w�b�_�[���C���N���[�h
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NHCOM;

/**
* 81, �q�E�B���h�E����Ώۉ摜��T���āA�cn�Ԗڂ��N���b�N
*/
int CMainDlg::DoStepChildSearchClickV(const TASK_STEP& step)
{
	// �q�E�B���h�E�X�i�b�v�V���b�g�擾
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
	{// �擾���s
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
	{// ���o��������Ȃ�
		return RET_NG;
	}

	VEC_FIND vec = cFindData.vecFind;
	std::sort(vec.begin(), vec.end(), ascTop);

	// �N���b�N�|�C���g�擾
	CPoint pt = vec[step.GetValue("index")].rc.CenterPoint();

	// �q�E�B���h�E�̓��b�Z�[�W���Ɣ������Ȃ�
	ActLButtonClick(m_hTaskWndChild, rc.TopLeft() + pt, 0, FALSE);

	return RET_OK;
}

/**
* 82, �q�E�B���h�E����Ώۉ摜��T���āA�Ȃ����OK
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
* 83, �q�E�B���h�E����Ώۉ摜��T���āA�����OK
*/
int CMainDlg::DoStepChildSearchExist(const TASK_STEP& step)
{
	// �q�E�B���h�E�X�i�b�v�V���b�g�擾

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
	{// �擾���s
		return RET_ERR;
	}

	// �Ώۉ摜��T��
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
	{// ����Ȃ�OK
		return RET_OK;
	}
	// �Ȃ��Ȃ玸�s
	return RET_NG;
}

