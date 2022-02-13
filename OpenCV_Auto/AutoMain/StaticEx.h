#pragma once
#include <memory>

//! プログレス
class CProgEx : public CStatic
{
public:
	//! コンストラクタ
	CProgEx();

	//! メッセージ
	static const ULONG m_uWM_PROGRESSEX;

	DECLARE_DYNAMIC(CProgEx)

	//! メッセージ
//	static const ULONG m_uWM_PROGRESSEX;
	enum
	{
		MODE_SETPOS = 0,
		MODE_SETPOS_DBL,
	};

	//! 位置設定
	void SetPos(UINT n) { m_uPos = n; Invalidate(); };
	//! 位置取得
	UINT GetPos()const { return m_uPos; };
	//! 範囲指定
	void SetRange32(UINT uMin, UINT uMax) { m_uMax = uMax; m_uMin = uMin; };
	//! 範囲取得
	void GetRange(UINT& uMin, UINT& uMax)const { uMax = m_uMax; uMin = m_uMin; };

	void Add() {
		SetPos(GetPos() + 1);
		Invalidate();
	}

	void SetText(LPCTSTR str) {
		if (GetSafeHwnd())
		{
			CString temp;
			GetWindowText(temp);
			if (temp.IsEmpty() == FALSE &&
				temp.Compare(str)==0)
			{
				m_uPos++;
				Invalidate();
				return;
			}
			m_uPos = 0;
			SetWindowText(str);
			Invalidate();
		}
	}

protected:
	//! 左クリック
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//! 左ダブルクリック
/*	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//! 右クリック*/
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//! 描画
	afx_msg void OnPaint();
	//! 背景描画
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//! アイテム描画
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//! 最大
	UINT m_uMax;
	//! 最小
	UINT m_uMin;
	//! 位置
	UINT m_uPos;

	//! 描画フォント
	CFont m_cTimeFont;

	//! 裏ＤＩＢ
	std::auto_ptr<CBitmap> m_pDrawDib;
	//! ＤＩＢサイズ
	CSize m_szDib;

	//! 描画
	void Draw(CDC* pDC, CRect rc);

	DECLARE_MESSAGE_MAP()
};
