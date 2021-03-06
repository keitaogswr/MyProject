/*******************************************************************************
* タイトル名：
* ファイル名：debugProc.cpp
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* scanfのwarning対策
*******************************************************************************/
#ifdef _DEBUG
#define _CRT_SECURE_NO_WARNINGS

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "debugProc.h"
#include "manager.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
LPD3DXFONT CDebugProc::m_pFont;
char CDebugProc::m_aStr[10240];

/*******************************************************************************
* 関数名：void CDebugProc::Init( void )
*
* 引数	：
* 戻り値：
* 説明	：デバッグ初期化処理
*******************************************************************************/
void CDebugProc::Init(void)
{
	// デバイス取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		"Terminal",
		&m_pFont);

	// フォントのメモリクリア
	memset(m_aStr, 0, sizeof(m_aStr));
}

/*******************************************************************************
* 関数名：void CDebugProc::Uninit(void)
*
* 引数	：
* 戻り値：
* 説明	：デバッグ終了処理
*******************************************************************************/
void CDebugProc::Uninit(void)
{
	SAFE_RELEASE(m_pFont);
}

/*******************************************************************************
* 関数名：void CDebugProc::Update(void)
*
* 引数	：
* 戻り値：
* 説明	：デバッグ更新処理
*******************************************************************************/
void CDebugProc::Update(void)
{
}

/*******************************************************************************
* 関数名：void CDebugProc::Draw(void)
*
* 引数	：
* 戻り値：
* 説明	：デバッグ描画処理
*******************************************************************************/
void CDebugProc::Draw(void)
{
	// 変数定義
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// 文字描画
	m_pFont->DrawText(NULL, &m_aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));

	// メモリクリア
	memset(m_aStr, 0, sizeof(m_aStr));
}

/*******************************************************************************
* 関数名：void CDebugProc::Print(char *fmt, ...)
*
* 引数	：
* 戻り値：
* 説明	：デバッグプリント処理
*******************************************************************************/
void CDebugProc::Print(char *fmt, ...)
{
	char str[256];
	va_list arg;
	va_start(arg, fmt);

	vsprintf(str, fmt, arg);
	strcat(m_aStr, str);

	va_end(arg);

	return;
}

#endif