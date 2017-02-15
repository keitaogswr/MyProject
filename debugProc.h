/*******************************************************************************
* タイトル名：
* ファイル名：debugProc.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifdef _DEBUG
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

#include "renderer.h"

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
class CDebugProc
{
public:
	CDebugProc(){}
	~CDebugProc(){}
	static void Init(void);
	static void Uninit(void);
	static void Update(void);
	static void Draw(void);
	static void Print(char *fmt, ...);
private:
	static LPD3DXFONT m_pFont;
	static char m_aStr[10240];
};

#endif
#endif