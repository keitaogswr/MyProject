/******************************************************************************
	タイトル名 :インプットクラス
	ファイル名 : input.h
	作成者     : AT-13C-284 36 深澤佑太
	作成日     : 2016/04/26
	更新日     :
******************************************************************************/
/******************************************************************************
	インクルードガード
******************************************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

/******************************************************************************
	インクルードファイル
******************************************************************************/
#define DIRECTINPUT_VERSION ( 0x0800 )
#include "dinput.h"

#include "XInput.h"
#pragma comment(lib,"XInput.lib")

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )


/******************************************************************************
	マクロ定義
******************************************************************************/
const int MAX_KEY = 256;		//	キー最大数。
const int MAX_MOUSE_KEY = 8;	//	キー最大数。
const int MAX_CONTROLLERS = 1;	//	コントローラーの最大数。
const int REPEAT_STATE_CNT = 40;//	リピートのステートカウント。

/******************************************************************************
	構造体宣言
******************************************************************************/
typedef enum {
	MOUSE_INPUT_LEFT = 0,		//	左クリック。
	MOUSE_INPUT_RIGHT,			//	右クリック。
	MOUSE_INPUT_CENTER,			//	中クリック。
	MOUSE_INPUT_MAX
}MOUSE_INPUT;

/******************************************************************************
	クラス宣言
******************************************************************************/
class CInput
{
public:
	CInput();		//	コンストラクタ。
	~CInput();		//	デストラクタ。
	
	//	キーボード関連///////////////////////////////////////////////////////////////////////////////////
	HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);		//	キーボードの初期化関数(ここではデバイスの設定をしているので、マウスの初期化もここでやってます。)
	void UninitKeyboard(void);									//	キーボードの終了関数。
	void UpdateKeyboard(void);									//	キーボードの更新関数。

	bool GetKeyboardPress(int nKey) { return (m_aKeyState[nKey] & 0x80) ? true : false; };				//	キーボードのプレス情報取得。
	bool GetKeyboardTrigger(int nKey) { return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false; };		//	キーボードのトリガー情報取得。
	bool GetKeyboardRelease(int nKey) { return (m_aKeyStateRelease[nKey] & 0x80) ? true : false; };		//	キーボードのリリース情報取得。
	bool GetKeyboardRepeat(int nKey) { return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false; };		//	キーボードのリピート情報取得。
	//	キーボード関連///////////////////////////////////////////////////////////////////////////////////
	//	マウス関連///////////////////////////////////////////////////////////////////////////////////////
	void UninitMouse(void);		//	マウスの終了処理
	void UpdateMouse(void);		//	マウスの更新処理

	//	マウス情報の取得関数
	DIMOUSESTATE2 GetMouse(void) { return m_Mouse; }

	bool GetMousePress(int nKey) { return (m_aMouseKeyState.rgbButtons[nKey] & 0x80) ? true : false; }	//	マウスのプレス情報取得。
	bool GetMouseTrigger(int nKey) { return (m_aMouseKeyStateTrigger[nKey] & 0x80) ? true : false; }	//	マウスのトリガー情報取得。
	bool GetMouseRelease(int nKey) { return (m_aMouseKeyStateRelease[nKey] & 0x80) ? true : false; }	//	マウスのリリース情報取得。
	bool GetMouseRepeat(int nKey) { return (m_aMouseKeyStateRepeat[nKey] & 0x80) ? true : false; }		//	マウスのリピート情報取得。
	//	マウス関連///////////////////////////////////////////////////////////////////////////////////////
	//	JoyStick/////////////////////////////////////////////////////////////////////////////////////////
	void InitJoyStick(void);		//	JoyStickの初期化関数。
	void UninitJoyStick(void);		//	JoyStickの終了処理。
	void UpdateJoyStick(void);		//	JoyStickの更新処理。

	//	JoyStickの入力を受け取る関数
	bool PressJoyStick(WORD nKey) { return (m_PressState.Gamepad.wButtons & nKey) ? true : false; };		//	JoyStickの情報取得。
	bool TriggerJoyStick(WORD nKey) { return (m_TriggerState.Gamepad.wButtons & nKey) ? true : false; };	//	JoyStickの情報取得。
	bool ReleaseJoyStick(WORD nKey) { return (m_ReleaseState.Gamepad.wButtons & nKey) ? true : false; };	//	JoyStickの情報取得。
	bool RepeatJoyStick(WORD nKey) { return (m_RepeatState.Gamepad.wButtons & nKey) ? true : false; };		//	JoyStickの情報取得。
	bool GetJoyStickConnected(void) { return m_Connected; }			//	JoyStickの接続状況取得。
	XINPUT_STATE *GetPressState(void) { return &m_PressState; }
	//	JoyStick/////////////////////////////////////////////////////////////////////////////////////////
private:
	//	キーボード関連///////////////////////////////////////////////////////////////////////////////////
	LPDIRECTINPUT8       m_pInput;
	LPDIRECTINPUTDEVICE8 m_pDevKeyBoard;
	BYTE m_aKeyState[MAX_KEY];			//キーボードの入力情報ワーク(押した/押してない、の情報が入る)。
	BYTE m_aKeyStateTrigger[MAX_KEY];	//キートリガー情報。
	BYTE m_aKeyStateRelease[MAX_KEY];	//キーリリース情報。
	BYTE m_aKeyStateRepeat[MAX_KEY];	//キーリピート情報。
	int  m_aKeystateRepeatCnt[MAX_KEY];	//キーリピートカウンタ。
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//	マウス関連///////////////////////////////////////////////////////////////////////////////////////
	LPDIRECTINPUTDEVICE8 m_pDevMouse;
	DIMOUSESTATE2		 m_Mouse;
	DIMOUSESTATE2 m_aMouseKeyState;					//	マウスの入力情報ワーク(押した/押してない、の情報が入る)。
	BYTE m_aMouseKeyStateTrigger[MAX_MOUSE_KEY];	//	マウスのトリガー情報。
	BYTE m_aMouseKeyStateRelease[MAX_MOUSE_KEY];	//	マウスのリリース情報。
	BYTE m_aMouseKeyStateRepeat[MAX_MOUSE_KEY];		//	マウスのリピート情報。
	int  m_aMouseKeyStateRepeatCnt[MAX_MOUSE_KEY];	//	マウスのリピートカウンタ。
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//	JoyStick/////////////////////////////////////////////////////////////////////////////////////////
	XINPUT_STATE m_PressState;			//	JoyStickのプレス情報。
	XINPUT_STATE m_TriggerState;		//	JoyStickのトリガー情報。
	XINPUT_STATE m_ReleaseState;		//	JoyStickのリリース情報。
	XINPUT_STATE m_RepeatState;			//	JoyStickのリピート情報。
	int			 m_RepeatStateCnt;		//	JoyStickのリピートのカウンタ。
	bool         m_Connected;			//	接続されているかどうか。
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);		//	初期化関数。
	void Uninit(void);		//	終了関数。
	void Update(void);		//	更新関数。
};

#endif