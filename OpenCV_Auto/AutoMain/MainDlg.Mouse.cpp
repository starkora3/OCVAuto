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
* ���N���b�N
*/
void CMainDlg::ActLButtonClick(HWND hWnd, CPoint pos, int nSleep, BOOL bMsg)
{
	if (bMsg)
	{
		CRect rc;
		::GetClientRect(hWnd, rc);
		if (::ScreenToClient(hWnd, &pos) &&
			rc.PtInRect(pos))
		{// �E�B���h�E�̃N���C�A���g�̈���N���b�N
			// WM_LBUTTONDOWN �𑗂�	
			::SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELONG(pos.x, pos.y));
			::SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(pos.x, pos.y));

			if (nSleep >= 0)
			{
				Sleep(nSleep);
			}

			::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(pos.x, pos.y));
			::SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELONG(pos.x, pos.y));
			return;
		}
	}
	if(m_bADB){// adb �R�}���h���M
		CString strCmd;
		strCmd.Format(_T("adb {DEVICE} shell input touchscreen tap %d %d"), pos.x, pos.y);
		DoADBCommand(strCmd);
		return;
	}

	CMutexHolder mutex;

	::SetForegroundWindow(hWnd);

	//CPoint posPre;
	//GetCursorPos(&posPre);

	CPoint posPre = DoActiveWindow(hWnd, nSleep);

	if (m_nMouseMode > 0) {
		for (int i = 10; i >= 0; i--) {
			// �}�E�X�ړ�
			DoMouseMove(pos.x + (i * i), pos.y);
			Sleep(20);
		}
		Sleep(200);
	}
	else {
		// �}�E�X�ړ�
		DoMouseMove(pos.x, pos.y);
	}

	// �N���b�N
	DoLButtonDownUp(nSleep);

	if (m_nMouseMode == 0) {
		// �߂�
		DoMouseMove(posPre.x, posPre.y);
	}
}

/**
* ���N���b�N
*/
void CMainDlg::ActLButtonDClick(HWND hWnd, CPoint pos, int nSleep, BOOL bMsg)
{
	if (bMsg)
	{
		CRect rc;
		::GetClientRect(hWnd, rc);
		if (::ScreenToClient(hWnd, &pos) &&
			rc.PtInRect(pos))
		{// �E�B���h�E�̃N���C�A���g�̈���N���b�N
		 // WM_LBUTTONDOWN �𑗂�	

			::SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELONG(pos.x, pos.y));
			::SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(pos.x, pos.y));

			if (nSleep >= 0)
			{
				Sleep(nSleep);
			}

			::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(pos.x, pos.y));
			::SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELONG(pos.x, pos.y));

			Sleep(nSleep);

			::SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELONG(pos.x, pos.y));
			::SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(pos.x, pos.y));

			if (nSleep >= 0)
			{
				Sleep(nSleep);
			}

			::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(pos.x, pos.y));
			::SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELONG(pos.x, pos.y));
			return;
		}
	}
	if (m_bADB) {// adb �R�}���h���M
		CString strCmd;
		strCmd.Format(_T("adb {DEVICE} shell input touchscreen tap %d %d"), pos.x, pos.y);
		strCmd = strCmd + _T(" & ") + strCmd;
		DoADBCommand(strCmd);
		return;
	}
	CMutexHolder mutex;

	CPoint posPre;
	GetCursorPos(&posPre);

	if (m_nMouseMode > 0) {
		for (int i = 5; i > 0; i--) {
			// �}�E�X�ړ�
			DoMouseMove(pos.x+i*i, pos.y + i * 10);
			Sleep(20);
		}
		Sleep(200);
	}
	// �}�E�X�ړ�
	DoMouseMove(pos.x, pos.y);

	// �N���b�N
	DoLButtonDownUp(nSleep);
	::Sleep(nSleep);
	DoLButtonDownUp(nSleep);

	if (m_nMouseMode == 0) {
		// �߂�
		DoMouseMove(posPre.x, posPre.y);
	}
}


/**
* �}�E�X���� �ړ�
*/
void CMainDlg::DoMouseMove(int x, int y)
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT) * 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	int nWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int nWidth2 = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	pInput[nCount].mi.dx = x * 0xFFFF / nWidth;
	if (nWidth2 > 0 && nWidth2 != nWidth) {
		pInput[nCount].mi.dx = x * 0xFFFF / nWidth2;
	}
	pInput[nCount].mi.dy = y * 0xFFFF / nHeight;
	nCount++;

#ifdef _DEBUG
	/*	TCHAR str[256];
	_stprintf_s(str, 256, _T("\nScreen(%d,%d),Move(%d,%d)\n"), nWidth, nHeight, pInput[0].mi.dx, pInput[0].mi.dy);
	::OutputDebugString(str);*/
#endif

	::SendInput(nCount, pInput, sizeof(INPUT));
}

/**
* �}�E�X���� ���N���b�N
*/
void CMainDlg::DoLButtonDownUp(int nSleep)
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT) * 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));

	if (nSleep >= 0)
	{
		::Sleep(nSleep);
	}

	nCount = 0;
	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));
}

/**
* �}�E�X���� ���N���b�N
*/
void CMainDlg::DoLButtonDown()
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT) * 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	nCount++;

	//	pInput[nCount].type = INPUT_MOUSE;
	//	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	//	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));
}

/**
* �}�E�X���� ���N���b�N
*/
void CMainDlg::DoLButtonUp()
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT) * 6);
	int nCount = 0;

	/*
	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	nCount++;
	*/
	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));
}

/**
* �ړ� : �}�E�X���h���b�O���ăz�[���h
*/
BOOL CMainDlg::mouseMove(HWND hWnd , int nKey, int nSleep)
{
	::SendMessage(hWnd,WM_KEYDOWN, nKey, 0);
	Sleep((int)((double)nSleep*m_dSleepRatio));
	::SendMessage(hWnd, WM_KEYUP, nKey, 0);

	return TRUE;
}

BOOL CMainDlg::keyDown(WORD wKey)
{
	try
	{
		const int nSize = 1;
		INPUT pInput[1];
		::ZeroMemory(pInput, sizeof(INPUT) * 1);
		int index = 0;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		//pInput[index].ki.dwFlags = KEYEVENTF_KEYUP;
		index++;

		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// �S�Ă͎��s�ł��Ȃ�����
			return FALSE;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMainDlg::keyUp(WORD wKey)
{
	try
	{
		const int nSize = 1;
		INPUT pInput[1];
		::ZeroMemory(pInput, sizeof(INPUT) * 1);
		int index = 0;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		pInput[index].ki.dwFlags = KEYEVENTF_KEYUP;
		index++;

		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// �S�Ă͎��s�ł��Ȃ�����
			return FALSE;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

/*
*
*/
BOOL CMainDlg::keyDownUp(WORD wKey)
{
	try
	{
		const int nSize = 2;
		INPUT pInput[2];
		::ZeroMemory(pInput, sizeof(INPUT) * 2);
		int index = 0;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		index++;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		pInput[index].ki.dwFlags = KEYEVENTF_KEYUP;

		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// �S�Ă͎��s�ł��Ȃ�����
			return FALSE;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

/**
* �L�[�_�E���^�X�N
*/
BOOL CMainDlg::keyDownTask(WORD* pKeyArray, int nSize)
{
	return keyTask(pKeyArray, nSize);
}

/**
* �L�[�A�b�v�^�X�N
*/
BOOL CMainDlg::keyUpTask(WORD* pKeyArray, int nSize)
{
	return keyTask(pKeyArray, nSize, KEYEVENTF_KEYUP);
}
/**
* �L�[�^�X�N
*/
BOOL CMainDlg::keyTask(WORD* pKeyArray, int nSize, DWORD dwFlags)
{
	if (nSize == 0)
	{
		return TRUE;
	}
	BOOL bRet = TRUE;
	// �L�[�{�[�h����
	INPUT* pInput = NULL;
	const int nTotalSize = sizeof(INPUT)*nSize;
	try
	{
		pInput = (INPUT*)new BYTE[nTotalSize];
		if (pInput == NULL)
		{
			throw GetLastError();
		}
		::ZeroMemory(pInput, nTotalSize);

		for (int i = 0; i < nSize; i++)
		{
			//�L�[�Z�b�g
			pInput[i].type = INPUT_KEYBOARD;
			pInput[i].ki.dwFlags = dwFlags;
			pInput[i].ki.wVk = pKeyArray[i];
		}
		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// �S�Ă͎��s�ł��Ȃ�����
			bRet = FALSE;
		}

		delete[] pInput;

		pInput = NULL;
	}
	catch (...)
	{
		if (pInput)
		{
			delete[] pInput;
		}
		bRet = FALSE;
	}
	return TRUE;
}
