/*******************************************************************************
* タイトル名：
* ファイル名：input.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#define DIRECTINPUT_VERSION ( 0x0800 )
#include "dinput.h"

/*******************************************************************************
* ライブラリのリンク
*******************************************************************************/
#pragma comment( lib, "dinput8.lib" )	// 入力処理に必要
#pragma comment( lib, "dxguid.lib" )	// DirectXコンポーネント使用に必要

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体
*******************************************************************************/
/* マウスボタンの列挙型 */
typedef enum
{
	DIM_LEFT			= 0,				// マウス左ボタン
	DIM_RIGHT,								// マウス右ボタン
	DIM_CENTER,								// マウスホイールボタン
	DIM_BUTTON3,							// マウスボタン3
	DIM_BUTTON4,							// マウスボタン4
	DIM_BUTTON5,							// マウスボタン5
	DIM_BUTTON6,							// マウスボタン6
	DIM_BUTTON7,							// マウスボタン7
	DIM_MAX
}MOUSESTATE_BUTTONS;

/* キーボード構造体 */
typedef struct
{
	BYTE press;							// キープレス状態
	BYTE trigger;						// キートリガー状態
	BYTE release;						// キーリリース状態
	BYTE repeat;						// キーリピート状態
	unsigned repeatCnt;					// キーリピートカウンタ
}KEYSTATE;

/* マウス構造体 */
typedef struct
{
	DIMOUSESTATE2 state;				// マウス情報構造体
	BYTE trigBtns[ DIM_MAX ];			// マウスボタントリガー状態
	BYTE releaseBtns[ DIM_MAX ];		// マウスボタンリリース状態

	bool bWindow;						// ウィンドウモードフラグ
	HWND hWnd;							// マウス座標取得用ウィンドウハンドル
	POINT localPos;						// マウスローカル座標
}MOUSESTATE;

/*******************************************************************************
* 前方宣言
*******************************************************************************/

/*******************************************************************************
* クラス宣言
*******************************************************************************/
// インプットクラス
class CInput
{
	private:
		static LPDIRECTINPUT8		m_pInput;					// IDirectInput8インターフェースへのポインタ
		static LPDIRECTINPUTDEVICE8 m_pDevKeyboard;				// 入力デバイスへのポインタ
		static LPDIRECTINPUTDEVICE8 m_pDevMouse;				// 入力デバイスへのポインタ

		static KEYSTATE m_aKeyState[ 256 ];						// キーの状態
		static MOUSESTATE m_aMouseState;
	public:
		static HRESULT Init( HINSTANCE hInstance, HWND hWnd );
		static void Uninit( void );
		static void Update( void );

		static HRESULT InitKeyboard( HINSTANCE hInstance, HWND hWnd );
		static void UninitKeyboard( void );
		static void UpdateKeyboard( void );
		static bool GetKeyboardPress( int nKey );
		static bool GetKeyboardTrigger( int nKey );
		static bool GetKeyboardRelease( int nKey );
		static bool GetKeyboardRepeat( int nKey );

		static HRESULT InitMouse( HINSTANCE hInstance, HWND hWnd );
		static void UninitMouse( void );
		static HRESULT UpdateMouse( void );
		static MOUSESTATE *GetMouseState( void );
		static bool GetMousePress( MOUSESTATE_BUTTONS nMouse );
		static bool GetMouseTrigger( MOUSESTATE_BUTTONS nMouse );
		static bool GetMouseRelease( MOUSESTATE_BUTTONS nMouse );
};

#endif