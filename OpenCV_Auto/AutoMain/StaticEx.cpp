
#include "stdafx.h"
#include "StaticEx.h"
#include <math.h>


//! デザイン定義

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

// CSongListCtrl ダイアログ
const ULONG CProgEx::m_uWM_PROGRESSEX = ::RegisterWindowMessage(_T("AUTO_PROGRESSEX"));

/**
* コンストラクタ
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
		16,                         // フォントの高さ 
		0,                          // フォントの横幅？ 
		0,                          // 文字送りの方向角度 
		0,                          // ベースラインの角度 
		FW_BOLD,                // フォントの太さ 
		FALSE,                      // 斜体かどうか 
		FALSE,                      // 下線のタイプ 
		FALSE,                      // 打ち消し線 
		SHIFTJIS_CHARSET,           // フォントの文字セット 
		OUT_DEFAULT_PRECIS,         // 出力精度 
		CLIP_DEFAULT_PRECIS,        // クリッピング 
		ANTIALIASED_QUALITY,        // 出力品質 
		VARIABLE_PITCH | FF_SCRIPT, // フォントのピッチとファミリ 
		NULL             // フォントの種類（ NULL でも可 ） 
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
* 描画
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
* アイテム描画
*/
void CProgEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}


/**
*
*/
BOOL CProgEx::OnEraseBkgnd(CDC* pDC)
{//デフォルト禁止
	return TRUE;
}


/**
* 描画
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
	{// バー描画
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

	//テキスト描画
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
* クリック
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
* クリック
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