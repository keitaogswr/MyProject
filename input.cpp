/******************************************************************************
	タイトル名 :インプットクラス
	ファイル名 : input.cpp
	作成者     : AT-13C-284 36 深澤佑太
	作成日     : 2016/04/26
	更新日     :
******************************************************************************/
/******************************************************************************
	インクルードファイル
******************************************************************************/
#include "renderer.h"
#include "main.h"
#include "input.h"

/******************************************************************************
	関数名 : CInput::CInput()
	説明   : コンストラクタ
			 変数の初期化と、ポインタにNULLを代入
******************************************************************************/
CInput::CInput()
{
	m_pInput       = NULL;
	m_pDevKeyBoard = NULL;
	m_Connected = false;

	for (int nCntKey = 0; nCntKey < MAX_KEY; nCntKey++) {
		m_aKeyState[nCntKey]          = 0;
		m_aKeyStateTrigger[nCntKey]   = 0;
		m_aKeyStateRelease[nCntKey]   = 0;
		m_aKeyStateRepeat[nCntKey]    = 0;
		m_aKeystateRepeatCnt[nCntKey] = 0;
	}

	for (int i = 0; i < MAX_MOUSE_KEY; i++) {
		m_aMouseKeyState.rgbButtons[i]  = 0;
		m_aMouseKeyStateTrigger[i]      = 0;
		m_aMouseKeyStateRelease[i]      = 0;
		m_aMouseKeyStateRepeat[i]       = 0;
		m_aMouseKeyStateRepeatCnt[i]    = 0;
	}
}

/******************************************************************************
	関数名 : CInput::~CInput()
	説明   : デストラクタ
******************************************************************************/
CInput::~CInput()
{

}

/******************************************************************************
	関数名 : HRESULT CInput::Init(void)
	引数   : void
	戻り値 : E_FAIL, S_OK
	説明   : DirectInputオブジェクトの生成
******************************************************************************/
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == NULL) {
		//DirectInputオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

/******************************************************************************
	関数名 : void CInput::Uninit(void)
	引数   : void
	戻り値 : なし
	説明   : ポインタの解放処理。
******************************************************************************/
void CInput::Uninit(void)
{
	if (m_pInput != NULL) {
		m_pInput->Release();
		m_pInput = NULL;
	}
}

/******************************************************************************
	関数名 : void CInput::Update(void)
	引数   : void
	戻り値 : なし
	説明   : 特になし
******************************************************************************/
void CInput::Update(void)
{

}

/******************************************************************************
	関数名 : HRESULT CInput::InitKeyboard(HINSTANCE hInstance, HWND hWnd)
	引数   : hInstance, hWnd
	戻り値 : E_FAIL, S_OK
	説明   : Keyboardの初期化, デバイスの設定, データフォーマットの設定, 協調モードの設定, アクセス権を取得
******************************************************************************/
HRESULT CInput::InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	int nKeyID;

	//Keyboardの初期化
	if (FAILED(Init(hInstance, hWnd))) {
		nKeyID = MessageBox(hWnd, " 初期化に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//デバイスの設定(KeyBoard)
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevKeyBoard, NULL))) {
		nKeyID = MessageBox(hWnd, " デバイスの設定に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//デバイスの設定(Mouse)
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevMouse, NULL))) {
		nKeyID = MessageBox(hWnd, " デバイスの設定に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//データフォーマットの設定(KeyBoard)
	if (FAILED(m_pDevKeyBoard->SetDataFormat(&c_dfDIKeyboard))) {
		nKeyID = MessageBox(hWnd, " データフォーマットの設定に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//データフォーマットの設定(Mouse)
	if (FAILED(m_pDevMouse->SetDataFormat(&c_dfDIMouse2))) {
		nKeyID = MessageBox(hWnd, " データフォーマットの設定に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//協調モードの設定(KeyBoard)
	if (FAILED(m_pDevKeyBoard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))) {
		nKeyID = MessageBox(hWnd, " 協調モードの設定に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//協調モードの設定(Mouse)
	if (FAILED(m_pDevMouse->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))) {
		nKeyID = MessageBox(hWnd, " 協調モードの設定に失敗しました。 ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//ウィンドウを破棄
		}

		return E_FAIL;
	}

	//アクセス権を取得
	m_pDevKeyBoard->Acquire();
	m_pDevMouse->Acquire();
	InitJoyStick();
	return S_OK;
}

/******************************************************************************
	関数名 : void CInput::UninitKeyboard(void)
	引数   : void
	戻り値 : なし
	説明   : 取得したアクセス権の開放
******************************************************************************/
void CInput::UninitKeyboard(void)
{
	//	キーボード
	if (m_pDevKeyBoard != NULL) {
		m_pDevKeyBoard->Unacquire();		//アクセス権の開放
		m_pDevKeyBoard->Release();
		m_pDevKeyBoard = NULL;
	}

	//	マウスの解放
	UninitMouse();
	UninitJoyStick();
	Uninit();
}

/******************************************************************************
	関数名 : void CInput::UninitMouse(void)
	引数   : void
	戻り値 : なし
	説明   : 取得したアクセス権の開放
******************************************************************************/
void CInput::UninitMouse(void)
{
	//	マウス
	if (m_pDevMouse != NULL) {
		m_pDevMouse->Unacquire();		//アクセス権の開放
		m_pDevMouse->Release();
		m_pDevMouse = NULL;
	}
}

/******************************************************************************
	関数名 : void CInput::UpdateKeyboard(void)
	引数   : void
	戻り値 : なし
	説明   : Keyboardの状態を取得
******************************************************************************/
void CInput::UpdateKeyboard(void)
{
	BYTE aKeyState[MAX_KEY];
	int nKeyID = 0;

	if (SUCCEEDED(m_pDevKeyBoard->GetDeviceState(sizeof(aKeyState), &aKeyState[0]))) {
		for (int nCntKey = 0; nCntKey < MAX_KEY; nCntKey++) {
			//Trigger
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] | aKeyState[nCntKey]) ^ m_aKeyState[nCntKey];

			//Release
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] | aKeyState[nCntKey]) ^ aKeyState[nCntKey];
			if (GetKeyboardRelease(nCntKey) == true) {
				m_aKeystateRepeatCnt[nCntKey] = 0;
			}

			//Pless処理
			m_aKeyState[nCntKey] = aKeyState[nCntKey];
			if (GetKeyboardPress(nCntKey) == true) {
				m_aKeystateRepeatCnt[nCntKey] += 1;
			}

			//Repeat
			m_aKeyStateRepeat[nCntKey] = (m_aKeyState[nCntKey] | aKeyState[nCntKey]) ^ m_aKeyState[nCntKey];
			if (GetKeyboardTrigger(nCntKey) == true | m_aKeystateRepeatCnt[nCntKey] > 40) {
				m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
			}
		}
	}
	else {
		//アクセス権を取得
		m_pDevKeyBoard->Acquire();
	}
}

/******************************************************************************
	関数名 : void CInput::UpdateMouse(void)
	引数   : void
	戻り値 : なし
	説明   : Mouseの状態を取得
******************************************************************************/
void CInput::UpdateMouse(void)
{
	BYTE aKeyState[MAX_MOUSE_KEY];
	int nKeyID = 0;
	DIMOUSESTATE2 aMouseState;

	if (SUCCEEDED(m_pDevMouse->GetDeviceState(sizeof(aMouseState), &aMouseState))) {
		for (int i = 0; i < MAX_MOUSE_KEY; i++) {
			//Trigger
			m_aMouseKeyStateTrigger[i] = (m_aMouseKeyState.rgbButtons[i] | aMouseState.rgbButtons[i]) ^ m_aMouseKeyState.rgbButtons[i];

			//Release
			m_aMouseKeyStateRelease[i] = (m_aMouseKeyState.rgbButtons[i] | aMouseState.rgbButtons[i]) ^ aMouseState.rgbButtons[i];
			if (GetMouseRelease(i) == true) {
				m_aMouseKeyStateRepeatCnt[i] = 0;
			}

			//Pless処理
			m_aMouseKeyState.rgbButtons[i] = aMouseState.rgbButtons[i];
			if (GetMousePress(i) == true) {
				m_aMouseKeyStateRepeatCnt[i] += 1;
			}

			//Repeat
			m_aMouseKeyStateRepeat[i] = (m_aMouseKeyState.rgbButtons[i] | aMouseState.rgbButtons[i]) ^ m_aMouseKeyState.rgbButtons[i];
			if (GetMouseTrigger(i) == true | m_aMouseKeyStateRepeatCnt[i] > 40) {
				m_aMouseKeyStateRepeat[i] = aMouseState.rgbButtons[i];
			}

		}
		m_Mouse = aMouseState;
	}
	else {
		//アクセス権を取得
		m_pDevMouse->Acquire();
	}
}

/******************************************************************************
	関数名 : void CInput::InitJoyStick(void)
	引数   : void
	戻り値 : なし
	説明   : JoyStickの初期化。
******************************************************************************/
void CInput::InitJoyStick(void)
{
	//	作業用変数
	DWORD dwResult;

	for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &state);

		//	デヴァイスがあるかどうか
		if (dwResult == ERROR_SUCCESS) {// Controller is connected
		 //	Press情報の初期化
			m_PressState.Gamepad.bLeftTrigger = NULL;
			m_PressState.Gamepad.bRightTrigger = NULL;
			m_PressState.Gamepad.sThumbLX = NULL;
			m_PressState.Gamepad.sThumbLY = NULL;
			m_PressState.Gamepad.sThumbRX = NULL;
			m_PressState.Gamepad.sThumbRY = NULL;
			m_PressState.Gamepad.wButtons = NULL;

			// Trigger情報の初期化
			m_TriggerState.Gamepad.bLeftTrigger = NULL;
			m_TriggerState.Gamepad.bRightTrigger = NULL;
			m_TriggerState.Gamepad.sThumbLX = NULL;
			m_TriggerState.Gamepad.sThumbLY = NULL;
			m_TriggerState.Gamepad.sThumbRX = NULL;
			m_TriggerState.Gamepad.sThumbRY = NULL;
			m_TriggerState.Gamepad.wButtons = NULL;

			//	Release情報の初期化
			m_ReleaseState.Gamepad.bLeftTrigger = NULL;
			m_ReleaseState.Gamepad.bRightTrigger = NULL;
			m_ReleaseState.Gamepad.sThumbLX = NULL;
			m_ReleaseState.Gamepad.sThumbLY = NULL;
			m_ReleaseState.Gamepad.sThumbRX = NULL;
			m_ReleaseState.Gamepad.sThumbRY = NULL;
			m_ReleaseState.Gamepad.wButtons = NULL;

			//	Repeat情報の初期化
			m_RepeatState.Gamepad.bLeftTrigger = NULL;
			m_RepeatState.Gamepad.bRightTrigger = NULL;
			m_RepeatState.Gamepad.sThumbLX = NULL;
			m_RepeatState.Gamepad.sThumbLY = NULL;
			m_RepeatState.Gamepad.sThumbRX = NULL;
			m_RepeatState.Gamepad.sThumbRY = NULL;
			m_RepeatState.Gamepad.wButtons = NULL;

			//	RepeatCntの初期化
			m_RepeatStateCnt = 0;

			m_Connected = true;
		}
		else {// Controller is not connected 

		}
	}
}

/******************************************************************************
	関数名 : void CInput::UninitJoyStick(void)
	引数   : void
	戻り値 : なし
	説明   : JoyStickの終了処理。。
******************************************************************************/
void CInput::UninitJoyStick(void)
{
}

/******************************************************************************
	関数名 : void CInput::UpdateJoyStick(void)
	引数   : void
	戻り値 : なし
	説明   : JoyStickの更新処理。。
******************************************************************************/
void CInput::UpdateJoyStick(void)
{
	//	作業用変数
	DWORD dwResult;
	XINPUT_STATE state;

	for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {
		//	0で初期化
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		//	デヴァイス情報取得
		dwResult = XInputGetState(i, &state);

		//	デヴァイスがあったら
		if (dwResult == ERROR_SUCCESS) {
			//Trigger
			m_TriggerState.Gamepad.wButtons = (m_PressState.Gamepad.wButtons | state.Gamepad.wButtons) ^ m_PressState.Gamepad.wButtons;

			//Release
			m_ReleaseState.Gamepad.wButtons = (m_PressState.Gamepad.wButtons | state.Gamepad.wButtons) ^ state.Gamepad.wButtons;
			if (ReleaseJoyStick((WORD)i) == true) {
				m_RepeatStateCnt = 0;
			}

			//Pless処理
			if (state.Gamepad.sThumbLY <= 0) {
				m_PressState = state;
			}

			m_PressState = state;
			if (PressJoyStick((WORD)i) == true) {
				m_RepeatStateCnt += 1;
			}

			//Repeat
			m_RepeatState.Gamepad.wButtons = (m_PressState.Gamepad.wButtons | state.Gamepad.wButtons) ^ m_PressState.Gamepad.wButtons;
			if (TriggerJoyStick((WORD)i) == true | m_RepeatStateCnt > REPEAT_STATE_CNT) {
				m_RepeatState = state;
			}

			m_Connected = true;
		}
		else {		//	デバイスがない。
			m_Connected = false;
		}
	}
}