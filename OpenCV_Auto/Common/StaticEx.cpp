
#include "stdafx.h"
#include "StaticEx.h"
#include <math.h>


//! �f�U�C����`

namespace
{
	const INT BORDER_WIDTH = 2;

	const COLORREF COLOR_BORDER = RGB(100, 100, 255);
	const COLORREF COLOR_BORDER2 = RGB(200, 200, 200);
	const COLORREF COLOR_BACK = RGB(255, 255, 255);

	const COLORREF COLOR_BAR = RGB(220, 220, 255);
	const COLORREF COLOR_BAR2 = RGB(160, 160, 255);

	const COLORREF COLOR_BAR3 = RGB(255, 255, 200);
	const COLORREF COLOR_BAR4 = RGB(255, 255, 0);

	const COLORREF COLOR_BAR5 = RGB(255, 200, 200);
	const COLORREF COLOR_BAR6 = RGB(255, 80, 80);

	const COLORREF COLOR_TEXT = RGB(100, 100, 100);
	const COLORREF COLOR_TEXT2 = RGB(70, 70, 70);
	const COLORREF COLOR_TEXT3 = RGB(70, 70, 70);
	const COLORREF COLOR_TEXT4 = RGB(70, 70, 70);
	const COLORREF COLOR_TEXT5 = RGB(0, 0, 0);
	const COLORREF COLOR_TEXT6 = RGB(0, 0, 0);
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSongListCtrl �_�C�A���O
const ULONG CProgEx::m_uWM_PROGRESSEX = ::RegisterWindowMessage(_T("AUTO_PROGRESSEX"));

/**
* �R���X�g���N�^
*/
CProgEx::CProgEx()
{
#ifdef _DEBUG 
	m_uMax = 10;
#else
	m_uMax = 20;
#endif
	m_uMin = 0;
	m_uPos = 0;
	m_szDib.SetSize(0, 0);

	m_cTimeFont.CreateFont(
		16,                         // �t�H���g�̍��� 
		0,                          // �t�H���g�̉����H 
		0,                          // ��������̕����p�x 
		0,                          // �x�[�X���C���̊p�x 
		FW_BOLD,                // �t�H���g�̑��� 
		FALSE,                      // �Α̂��ǂ��� 
		FALSE,                      // �����̃^�C�v 
		FALSE,                      // �ł������� 
		SHIFTJIS_CHARSET,           // �t�H���g�̕����Z�b�g 
		OUT_DEFAULT_PRECIS,         // �o�͐��x 
		CLIP_DEFAULT_PRECIS,        // �N���b�s���O 
		ANTIALIASED_QUALITY,        // �o�͕i�� 
		VARIABLE_PITCH | FF_SCRIPT, // �t�H���g�̃s�b�`�ƃt�@�~�� 
		NULL             // �t�H���g�̎�ށi NULL �ł��� �j 
		);
}

IMPLEMENT_DYNAMIC(CProgEx, CStatic)

BEGIN_MESSAGE_MAP(CProgEx, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/**
* �`��
*/
void CProgEx::OnPaint()
{

	CPaintDC cDC(this);

	try
	{
		CRect rc;
		GetClientRect(rc);

		if (m_pDrawDib.get() == NULL ||
			m_szDib != rc.Size())
		{
			m_pDrawDib.reset(new CBitmap);
			m_pDrawDib->CreateCompatibleBitmap(&cDC, rc.Width(), rc.Height());
			m_szDib = rc.Size();

		}

		CDC cMemDC;
		cMemDC.CreateCompatibleDC(&cDC);
		HBITMAP hSaveBmp = (HBITMAP)cMemDC.SelectObject(m_pDrawDib.get());

		Draw(&cMemDC, rc);

		cDC.BitBlt(0, 0, rc.Width(), rc.Height(), &cMemDC, 0, 0, SRCCOPY);

		cMemDC.SelectObject(hSaveBmp);
	}
	catch (...)
	{
	}
}

/**
* �A�C�e���`��
*/
void CProgEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}


/**
*
*/
BOOL CProgEx::OnEraseBkgnd(CDC* pDC)
{//�f�t�H���g�֎~
	return TRUE;
}


/**
* �`��
*/
void  CProgEx::Draw(CDC* pDC, CRect rc)
{
	const CRect rcAll(rc);

	CPen cPen(PS_SOLID, 1, 0xff0000);
	CPen cPen2(PS_SOLID, 1, 0xffffff);

	CBrush cBrush(COLOR_BACK);

	CString str;
	GetWindowText(str);
	if (str.IsEmpty())
	{
		str = _T(" --- (None) --- ");
	}

	pDC->FillSolidRect(rcAll, RGB(200,200,200));

	HPEN hSavePen = (HPEN)pDC->SelectObject(&cPen);
	HBRUSH hSaveBrush = (HBRUSH)pDC->SelectStockObject(NULL_BRUSH);

	pDC->MoveTo(rc.left, rc.top);
	pDC->LineTo(rc.right - 1, rc.top);
	pDC->LineTo(rc.right - 1, rc.bottom-1);
	pDC->LineTo(rc.left, rc.bottom - 1);
	pDC->LineTo(rc.left, rc.top);
	rc.DeflateRect(1, 1, 1, 1);

	pDC->SelectObject(&cPen2);
	pDC->MoveTo(rc.left, rc.top);
	pDC->LineTo(rc.right - 1, rc.top);
	pDC->LineTo(rc.right - 1, rc.bottom - 1);
	pDC->LineTo(rc.left, rc.bottom - 1);
	pDC->LineTo(rc.left, rc.top);

	rc.DeflateRect(1, 1, 1, 1);

	//if(str.IsEmpty()==FALSE)
	{// �o�[�`��
		UINT uMin, uMax;
		GetRange(uMin, uMax);
		UINT nWidth = 0;

		if (GetPos())
		{
			UINT uVal = GetPos() % uMax;
			if (uVal == 0)
			{
				uVal = uMax;
			}
			nWidth = (UINT)(((double)uVal / (double)uMax*(double)rc.Width()) + 0.5);

			COLORREF clr;
			switch ((m_uPos-1) / m_uMax)
			{
			case 0:clr = COLOR_BAR; break;
			case 1:clr = COLOR_BAR2; break;
			case 2:clr = COLOR_BAR3; break;
			case 3:clr = COLOR_BAR4; break;
			case 4:clr = COLOR_BAR5; break;
			case 5:
			default:
				clr = COLOR_BAR6; break;
			}
			pDC->FillSolidRect(CRect(rc.TopLeft(), CSize(nWidth, rc.Height())), clr);
		}
		if (nWidth < (UINT)rc.Width())
		{
			pDC->FillSolidRect(CRect(nWidth + 1, rc.top, rc.left + rc.Width(), rc.top + rc.Height()), COLOR_BACK);

		}
	}

	//�e�L�X�g�`��
	if (str.IsEmpty()==FALSE)
	{
		COLORREF clr = COLOR_TEXT;
		if (m_uPos)
		{
			switch ((m_uPos - 1) / m_uMax)
			{
			case 0:clr = COLOR_TEXT; break;
			case 1:clr = COLOR_TEXT2; break;
			case 2:clr = COLOR_TEXT3; break;
			case 3:clr = COLOR_TEXT4; break;
			case 4:clr = COLOR_TEXT5; break;
			case 5:
			default:
				clr = COLOR_TEXT6; break;
			}
		}

		COLORREF clrText = pDC->SetTextColor(clr);
		HFONT hFontTemp = (HFONT)pDC->SelectObject(&m_cTimeFont);
		UINT nModeTemp = pDC->SetBkMode(TRANSPARENT);

		pDC->DrawText(str, CRect(rcAll), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		pDC->SelectObject(hFontTemp);
		pDC->SetBkMode(nModeTemp);
		pDC->SetTextColor(clrText);
	}

	pDC->SelectObject(&hSaveBrush);
	pDC->SelectObject(&hSavePen);
}

/**
* �N���b�N
*/
void CProgEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);

	try
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			::SendMessage(pWnd->GetSafeHwnd(), m_uWM_PROGRESSEX, 0, 0);
		}
	}
	catch (...)
	{
	}

}

/**
* �N���b�N
*/
void CProgEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);

	try
	{
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			::SendMessage(pWnd->GetSafeHwnd(), m_uWM_PROGRESSEX, 1, 0);
		}
	}
	catch (...)
	{
	}

}