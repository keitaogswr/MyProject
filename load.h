/*******************************************************************************
* タイトル名：
* ファイル名：load.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#pragma once

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "mode.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CLogo;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CLoad : public CMode
{
private:
	static unsigned int __stdcall NowLoading(void *p);
	/* スレッド用変数 */
	DWORD m_ExitCode;				// スレッドの終了判定
	CLogo *m_Logo;					// 
	static HANDLE m_Thread;			// スレッド情報
	static unsigned int m_ThreadId;	// スレッドID
	static CGame *m_Game;			// ゲーム

public:
	CLoad();
	~CLoad();
	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CGame *GetMode(void) { return m_Game; }
	CCamera *GetCamera(void) { return 0; }
};

