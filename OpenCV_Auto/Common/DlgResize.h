#pragma once
#include <memory>
#include <vector>

/*************************************************************
 *
 * class CResizeDialog の使用方法
 *
 * 機能 : 
 *	CDialogの派生クラスをリサイズ可能にするための機能を提供する。
 *
 * 実装方法 :
 *	1,  リソースエディタでダイアログを開き、"Border"の設定を"サイズ変更枠"にする。
 *  2,  class の継承元を全て CDialog から CResizeDialog に変更する。
 *  3,  派生先クラスに メンバ関数 InitAdjustCtrl を追加し、ダイアログのリサイズと共に
 *		移動/リサイズさせるコントロールの定義を記述する。
 *		※ [実装例]
 			void InitAdjustCtrl()
			{// 実装サンプル
				// ウィンドウ枠と共に拡大縮小するエディットの定義
				AddAdjustCtrl(IDC_EDIT1,ADJUST_TBLR);

				// OKボタンとキャンセルボタンを、サイズ固定,位置がウィンドウの右下からの距離固定 で定義
				AddAdjustCtrl(IDOK,ADJUST_BR);
				AddAdjustCtrl(IDCANCEL,ADJUST_BR);

				// ウィンドウの初期サイズを最小サイズとして設定
				CRect rc;
				GetWindowRect(rc);
				m_szWndMin = rc.Size();
			}
 *************************************************************/
namespace NHCOM
{

class CResizeDialog;

//! コントロール初期情報
struct CCtrlPosData
{
	CCtrlPosData():m_nID(-1),m_hWnd(NULL){}
	int		m_nID;
	HWND	m_hWnd;
	CRect	m_rcInit;		//!< 初期位置
	DWORD	m_dwType;		//!< リサイズ属性
	int		m_nAreaID;		//!< エリアID
	CRect	m_rcAreaInit;	//!< エリアの初期矩形
};

//! ダイアログコントロール位置の調整者
class CControlAdjuster
{
public:
	CControlAdjuster(CResizeDialog* pTarget);

	//! コントロールのマージンをとる辺の定義
	enum MARGIN_TYPE
	{
		MARGIN_TOP		=0x0001,
		MARGIN_BOTTOM	=0x0002,
		MARGIN_LEFT		=0x0004,
		MARGIN_RIGHT	=0x0008,
	};

	//! 管理コントロール追加
	int AddControl(int nCtrlID,		//!< コントロールID
					DWORD dwType,		//!< 属性
					int nAreaID			//!< エリアID
					);
	//! 管理コントロール追加
	int AddWnd(HWND hWnd,			//!< 
					DWORD dwType,		//!< 属性
					int nAreaID			//!< エリアID
					);
	//! 位置調整
	void AdjustControl();
protected :
	//! ターゲット
	CResizeDialog* m_pTarget;

	//! データ
	typedef std::vector<CCtrlPosData> VEC_DATA;
	VEC_DATA m_vecData;	
};

//! リサイズダイアログ
class CResizeDialog	: public CDialog
{
public:
	DECLARE_DYNAMIC(CResizeDialog)

	//! コンストラクタ
	CResizeDialog(UINT nID,CWnd* pParent);

	//! 区分矩形取得
	virtual CRect GetAreaRect(int nAreaID)const;
protected:

	/******************************************/
	/**			純粋仮想関数				***/
	//! コントロールのエントリー処理
	virtual void InitAdjustCtrl()=0;
	/*{// 実装サンプル
		AddAdjustCtrl(IDC_EDIT1,ADJUST_TBLR);
		// ウィンドウ最小サイズを設定
		CRect rc;
		GetWindowRect(rc);
		m_szWndMin = rc.Size();
	}
	/******************************************/

	enum
	{
		TIMER_RESIZE_ID = 1024,
		TIMER_RESIZE_INTERVAL = 100,
		TIMER_RESIZE_ADUST_INTERVAL = 500,
	};

	//! エリアID定義
	enum AREA_ID_ROOT
	{
		AREA_ALL = -1,	
		// 残りは必要に応じて派生クラスで定義する
	};

	//! 調整タイプ
	enum ADJUST_TYPE
	{
		// サイズ固定
		ADJUST_TL = CControlAdjuster::MARGIN_TOP|CControlAdjuster::MARGIN_LEFT,
		ADJUST_TR = CControlAdjuster::MARGIN_TOP|CControlAdjuster::MARGIN_RIGHT,
		ADJUST_BL = CControlAdjuster::MARGIN_BOTTOM|CControlAdjuster::MARGIN_LEFT,
		ADJUST_BR = CControlAdjuster::MARGIN_BOTTOM|CControlAdjuster::MARGIN_RIGHT,

		// 高さ固定 横幅リサイズ
		ADJUST_TLR = CControlAdjuster::MARGIN_TOP|CControlAdjuster::MARGIN_LEFT|CControlAdjuster::MARGIN_RIGHT,
		ADJUST_BLR = CControlAdjuster::MARGIN_BOTTOM|CControlAdjuster::MARGIN_LEFT|CControlAdjuster::MARGIN_RIGHT,

		// 横幅固定 縦リサイズ
		ADJUST_TBL = CControlAdjuster::MARGIN_TOP|CControlAdjuster::MARGIN_BOTTOM|CControlAdjuster::MARGIN_LEFT,
		ADJUST_TBR = CControlAdjuster::MARGIN_TOP|CControlAdjuster::MARGIN_BOTTOM|CControlAdjuster::MARGIN_RIGHT,

		// 縦横リサイズ
		ADJUST_TBLR = CControlAdjuster::MARGIN_TOP|CControlAdjuster::MARGIN_BOTTOM|CControlAdjuster::MARGIN_LEFT|CControlAdjuster::MARGIN_RIGHT,
	};

	//! 初期化
	virtual  BOOL OnInitDialog( );

	//! リサイズ
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//! サイズ制御
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//! タイマー
	afx_msg void OnTimer(UINT_PTR uTimer);

	//! コントロール位置サイズの調整
	virtual void AdjustControl();

	//! コントロール追加
	int AddAdjustCtrl(int nID,ADJUST_TYPE eType,int nAreaID = AREA_ALL);

	//! コントロール追加(HWND)
	int AddAdjustCtrlFromHwnd(HWND hWnd,ADJUST_TYPE eType,int nAreaID = AREA_ALL);

	//! コントロールアジャスター
	std::auto_ptr<CControlAdjuster>	m_pAjuster;

	//! 全体矩形
	CRect m_rcClient;

	//! エリア矩形更新処理
	virtual void UpdateAreaRect();

	//! ウィンドウ最大最小サイズ
	CSize m_szWndMax;
	CSize m_szWndMin;

	//! 
	BOOL m_bDelayResize;
	BOOL m_bResize;
	DWORD m_dwLastResizeTime;
	UINT m_uResizeTimer;

	DECLARE_MESSAGE_MAP()
};

};