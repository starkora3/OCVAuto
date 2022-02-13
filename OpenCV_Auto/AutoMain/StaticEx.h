#pragma once
#include <memory>

//! �v���O���X
class CProgEx : public CStatic
{
public:
	//! �R���X�g���N�^
	CProgEx();

	//! ���b�Z�[�W
	static const ULONG m_uWM_PROGRESSEX;

	DECLARE_DYNAMIC(CProgEx)

	//! ���b�Z�[�W
//	static const ULONG m_uWM_PROGRESSEX;
	enum
	{
		MODE_SETPOS = 0,
		MODE_SETPOS_DBL,
	};

	//! �ʒu�ݒ�
	void SetPos(UINT n) { m_uPos = n; Invalidate(); };
	//! �ʒu�擾
	UINT GetPos()const { return m_uPos; };
	//! �͈͎w��
	void SetRange32(UINT uMin, UINT uMax) { m_uMax = uMax; m_uMin = uMin; };
	//! �͈͎擾
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
	//! ���N���b�N
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//! ���_�u���N���b�N
/*	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//! �E�N���b�N*/
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//! �`��
	afx_msg void OnPaint();
	//! �w�i�`��
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//! �A�C�e���`��
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//! �ő�
	UINT m_uMax;
	//! �ŏ�
	UINT m_uMin;
	//! �ʒu
	UINT m_uPos;

	//! �`��t�H���g
	CFont m_cTimeFont;

	//! ���c�h�a
	std::auto_ptr<CBitmap> m_pDrawDib;
	//! �c�h�a�T�C�Y
	CSize m_szDib;

	//! �`��
	void Draw(CDC* pDC, CRect rc);

	DECLARE_MESSAGE_MAP()
};
