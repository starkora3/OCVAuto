#include "stdafx.h"
#include "DlgResize.h"

using namespace NHCOM;
/**
 * コンストラクタ
 */
CControlAdjuster::CControlAdjuster(CResizeDialog* pTarget):m_pTarget(pTarget)
{
	ASSERT(m_pTarget);
}

/**
 * コントロール追加
 */
int CControlAdjuster::AddControl(int nCtrlID,
									DWORD dwType,
									int nAreaID)
{
	ASSERT(m_pTarget);
	if( m_pTarget )
	{
		CWnd* pCtrl = m_pTarget->GetDlgItem(nCtrlID);
		if( pCtrl)
		{
			CRect rc;
			pCtrl->GetWindowRect(rc);
			m_pTarget->ScreenToClient(rc);

			CCtrlPosData cData;
			cData.m_nID = nCtrlID;
			cData.m_rcInit = rc;
			cData.m_dwType = dwType;
			cData.m_nAreaID = nAreaID;
			cData.m_rcAreaInit = m_pTarget->GetAreaRect(nAreaID);

			m_vecData.push_back(cData);

			return m_vecData.size()-1;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	return -1;
}
/**
 * コントロール追加
 */
int CControlAdjuster::AddWnd(HWND hWnd,
									DWORD dwType,
									int nAreaID)
{
	ASSERT(m_pTarget);
	if( m_pTarget )
	{
		CWnd* pCtrl = CWnd::FromHandle(hWnd);
		if( pCtrl)
		{
			CRect rc;
			pCtrl->GetWindowRect(rc);
			m_pTarget->ScreenToClient(rc);

			CCtrlPosData cData;
			cData.m_hWnd = hWnd;
			cData.m_rcInit = rc;
			cData.m_dwType = dwType;
			cData.m_nAreaID = nAreaID;
			cData.m_rcAreaInit = m_pTarget->GetAreaRect(nAreaID);

			m_vecData.push_back(cData);

			return m_vecData.size()-1;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	return -1;
}
/**
 * 位置調整
 */
void CControlAdjuster::AdjustControl()
{
	if( m_pTarget ==NULL)
	{
		ASSERT(FALSE);
		return;
	}

	if(m_vecData.size())
	{
		for( VEC_DATA::const_iterator iter= m_vecData.begin();
					iter != m_vecData.end();iter++)
		{
			const CCtrlPosData& cData = (*iter);

			
			CWnd* pCtrl = NULL;
			if(cData.m_nID>0)
			{
				pCtrl = m_pTarget->GetDlgItem(cData.m_nID);
			}
			else
			{
				pCtrl = CWnd::FromHandle(cData.m_hWnd);
			}
			if( pCtrl)
			{
				const CRect& rcAreaInit = cData.m_rcAreaInit;
				CRect rcArea = m_pTarget->GetAreaRect(cData.m_nAreaID);

				CRect rcCtrl = cData.m_rcInit;
				DWORD dwFlag = SWP_NOZORDER;
				if( (cData.m_dwType & (MARGIN_TOP|MARGIN_BOTTOM)) != (MARGIN_TOP|MARGIN_BOTTOM) &&
						(cData.m_dwType & (MARGIN_LEFT|MARGIN_RIGHT)) != (MARGIN_LEFT|MARGIN_RIGHT) )
				{// リサイズ無し
					dwFlag |= SWP_NOSIZE;

					if(cData.m_dwType&MARGIN_TOP)
					{
						rcCtrl.OffsetRect(0,rcArea.top +(rcCtrl.top - rcAreaInit.top) - rcCtrl.top);
					}
					else if( cData.m_dwType&MARGIN_BOTTOM)
					{
						rcCtrl.OffsetRect(0,rcArea.bottom  - (rcAreaInit.bottom - rcCtrl.bottom + rcCtrl.Height()) - rcCtrl.top);
					}

					if(cData.m_dwType&MARGIN_LEFT)
					{
						rcCtrl.OffsetRect(rcArea.left +(rcCtrl.left - rcAreaInit.left) - rcCtrl.left,0);
					}
					else if( cData.m_dwType&MARGIN_RIGHT)
					{
						rcCtrl.OffsetRect(rcArea.right  - (rcAreaInit.right - rcCtrl.right + rcCtrl.Width()) - rcCtrl.left,0);
					}
				}
				else
				{// リサイズ有り
					if( (cData.m_dwType & (MARGIN_TOP|MARGIN_BOTTOM)) == (MARGIN_TOP|MARGIN_BOTTOM))
					{// 縦リサイズ
						rcCtrl.top = rcArea.top + (rcCtrl.top - rcAreaInit.top);
						rcCtrl.bottom = rcArea.bottom + (rcCtrl.bottom - rcAreaInit.bottom);
					}
					else
					{// 縦リサイズ無し
						if(cData.m_dwType&MARGIN_TOP)
						{
							rcCtrl.OffsetRect(0,rcArea.top +(rcCtrl.top - rcAreaInit.top) - rcCtrl.top);
						}
						else if( cData.m_dwType&MARGIN_BOTTOM)
						{
							rcCtrl.OffsetRect(0,rcArea.bottom  - (rcAreaInit.bottom - rcCtrl.bottom + rcCtrl.Height()) - rcCtrl.top);
						}
					}

					if((cData.m_dwType & (MARGIN_LEFT|MARGIN_RIGHT)) == (MARGIN_LEFT|MARGIN_RIGHT))
					{// 横リサイズ
						rcCtrl.left = rcArea.left + (rcCtrl.left - rcAreaInit.left);
						rcCtrl.right = rcArea.right + (rcCtrl.right - rcAreaInit.right);				
					}
					else
					{// 横リサイズ無し
						if(cData.m_dwType&MARGIN_LEFT)
						{
							rcCtrl.OffsetRect(rcArea.left +(rcCtrl.left - rcAreaInit.left) - rcCtrl.left,0);
						}
						else if( cData.m_dwType&MARGIN_RIGHT)
						{
							rcCtrl.OffsetRect(rcArea.right  - (rcAreaInit.right - rcCtrl.right + rcCtrl.Width()) - rcCtrl.left,0);
						}
					}
				}

				pCtrl->SetWindowPos(NULL,rcCtrl.left,rcCtrl.top,
									rcCtrl.Width(),
									rcCtrl.Height(),
									dwFlag);
			}
		}
	}
}

/**
 * コンストラクタ
 */
CResizeDialog::CResizeDialog(UINT nID,CWnd* pParent):CDialog(nID,pParent)
,m_rcClient(0,0,0,0)
,m_szWndMax(0,0)
,m_szWndMin(0,0)
,m_bResize(FALSE)
,m_uResizeTimer(0)
,m_bDelayResize(FALSE)
{
}

IMPLEMENT_DYNAMIC(CResizeDialog, CDialog)

BEGIN_MESSAGE_MAP(CResizeDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


/**
 * 初期化
 */
BOOL CResizeDialog::OnInitDialog( )
{
	BOOL bRet = CDialog::OnInitDialog();
	try
	{
		// 初期サイズ取得
		GetClientRect(m_rcClient);
		m_pAjuster.reset(new CControlAdjuster(this));

#ifdef _DEBUG
		DWORD dwStyle = ::GetWindowLong(GetSafeHwnd(),GWL_STYLE);
		DWORD dwStyleEx = ::GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE);
		if(( dwStyle&WS_SIZEBOX)==0)
		{
			::AfxMessageBox(_T("dialog is not resizable"));
		}
#endif
		// コントロールのエントリー処理
		InitAdjustCtrl();
	}
	catch(...)
	{
	}
	return bRet;
}

/**
 * リサイズ
 */
 void CResizeDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	try
	{
		if( GetSafeHwnd() &&
				nType != SIZE_MINIMIZED)
		{
			if(m_bDelayResize)
			{
				m_bResize=TRUE;
				m_dwLastResizeTime = ::GetTickCount();
				if(m_uResizeTimer==0)
				{
					m_uResizeTimer = SetTimer(TIMER_RESIZE_ID,TIMER_RESIZE_INTERVAL,NULL);
				}
			}
			else
			{
				// 調整
				AdjustControl();
			}
		}
	}
	catch(...)
	{
	}
}

 /**
  * 調整
  */
void CResizeDialog::AdjustControl()
{
	// エリア矩形の更新
	UpdateAreaRect();
	if( m_pAjuster.get())
	{
		m_pAjuster->AdjustControl();
	}
}

/**
  * エリア矩形取得
  */
CRect CResizeDialog::GetAreaRect(int nAreaID)const
{
	ASSERT(nAreaID == AREA_ALL);
	return m_rcClient;
}

/**
 * 調整コントロール追加
 */
int CResizeDialog::AddAdjustCtrl(int nID,ADJUST_TYPE eType,int nAreaID)
{
	ASSERT(m_pAjuster.get());
	if( m_pAjuster.get())
	{
		return m_pAjuster->AddControl(nID,static_cast<DWORD>(eType),nAreaID);
	}
	return -1;
}

/**
 * 調整コントロール追加
 */
int CResizeDialog::AddAdjustCtrlFromHwnd(HWND hWnd,ADJUST_TYPE eType,int nAreaID)
{
	ASSERT(m_pAjuster.get());
	if( m_pAjuster.get())
	{
		return m_pAjuster->AddWnd(hWnd,static_cast<DWORD>(eType),nAreaID);
	}
	return -1;
}


/**
 * エリア矩形の更新
 */
void CResizeDialog::UpdateAreaRect()
{
	GetClientRect(m_rcClient);
}


/**
 * ウィンドウのサイズ制限制御
 */
void CResizeDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	try
	{
		if( m_szWndMin.cx && m_szWndMin.cx)
		{
			// 最小トラッキングサイズ設定
			lpMMI->ptMinTrackSize.x = m_szWndMin.cx;
			lpMMI->ptMinTrackSize.y = m_szWndMin.cy;
		}

		if( m_szWndMax.cx && m_szWndMax.cx)
		{
			lpMMI->ptMaxSize.x = m_szWndMax.cx;
			lpMMI->ptMaxSize.y = m_szWndMax.cy;
		}
	}
	catch(CException* p)
	{
		p->Delete();
	}
}

//! タイマー
void CResizeDialog::OnTimer(UINT_PTR uTimerID)
{
	__super::OnTimer(uTimerID);

	try
	{
		if(uTimerID == TIMER_RESIZE_ID)
		{
			if(m_bResize)
			{
				DWORD dwTime = ::GetTickCount();
				if(dwTime - m_dwLastResizeTime >= TIMER_RESIZE_ADUST_INTERVAL)
				{
					// 調整
					AdjustControl();
					m_bResize = FALSE;
				}
			}
			else
			{
				KillTimer(uTimerID);
				m_uResizeTimer=0;
			}
		}
	}
	catch(...)
	{
	}
}