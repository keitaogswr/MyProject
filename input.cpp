/*******************************************************************************
* タイトル名：
* ファイル名：input.cpp
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WAIT_COUNT_REPEAT	( 20 )		// リピート開始までの待ち時間

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/
LPDIRECTINPUT8	CInput::m_pInput;
LPDIRECTINPUTDEVICE8 CInput::m_pDevKeyboard;
LPDIRECTINPUTDEVICE8 CInput::m_pDevMouse;

KEYSTATE CInput::m_aKeyState[ 256 ];
MOUSESTATE CInput::m_aMouseState;

/*******************************************************************************
* 関数名：HRESULT CInput::Init( HINSTANCE hInstance, HWND hWnd )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
HRESULT CInput::Init( HINSTANCE hInstance, HWND hWnd )
{
	if( m_pInput == NULL )
	{
		// DirectInputオブジェクトの作成
		if( FAILED(
			DirectInput8Create
			(
				hInstance,
				DIRECTINPUT_VERSION,
				IID_IDirectInput8,
				( void** )&m_pInput,
				NULL
			) ) )
		{
			MessageBox( hWnd, "DirectInput8オブジェクトの作成ができませんでした。", "警告！", MB_ICONWARNING );
			return E_FAIL;
		}
	}
	return S_OK;
}

/*******************************************************************************
* 関数名：void CInput::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CInput::Uninit( void )
{
	UninitKeyboard();
	SAFE_RELEASE( m_pInput );
}

/*******************************************************************************
* 関数名：void CInput::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CInput::Update( void )
{

}

/*******************************************************************************
* 関数名：HRESULT CInput::InitKeyboard( HINSTANCE hInstance, HWND hWnd )
* 
* 引数	：
* 戻り値：
* 説明	：キーボード初期化処理
*******************************************************************************/
HRESULT CInput::InitKeyboard( HINSTANCE hInstance, HWND hWnd )
{
	if( FAILED( Init( hInstance, hWnd ) ) )
	{
		return E_FAIL;
	}
	if( FAILED( m_pInput->CreateDevice( GUID_SysKeyboard, &m_pDevKeyboard, NULL ) ) )
	{
		return E_FAIL;
	}
	if( FAILED( m_pDevKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
	{
		return E_FAIL;
	}
	if( FAILED( m_pDevKeyboard->SetCooperativeLevel( hWnd, ( DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) )
	{
		return S_OK;
	}
	m_pDevKeyboard->Acquire();
	return S_OK;
}

/*******************************************************************************
* 関数名：void CInput::UninitKeyboard( void )
* 
* 引数	：
* 戻り値：
* 説明	：キーボード終了処理
*******************************************************************************/
void CInput::UninitKeyboard( void )
{
	if( m_pDevKeyboard != NULL )
	{
		m_pDevKeyboard->Unacquire();
		m_pDevKeyboard->Release();
		m_pDevKeyboard = NULL;
	}
}

/*******************************************************************************
* 関数名：void UpdateKeyboard( void )
* 
* 引数	：

* 戻り値：
* 説明	：キーボード更新処理
*******************************************************************************/
void CInput::UpdateKeyboard( void )
{
	BYTE aKeyState[ 256 ];

	if( SUCCEEDED( m_pDevKeyboard->GetDeviceState( sizeof( aKeyState ) , &aKeyState[ 0 ] ) ) )
	{
		for( int nCntKey = 0; nCntKey < 256 ; nCntKey++ )
		{
			m_aKeyState[ nCntKey ].trigger =  m_aKeyState[ nCntKey ].press ^ ( m_aKeyState[ nCntKey ].press | aKeyState[ nCntKey ] );
			m_aKeyState[ nCntKey ].release =  aKeyState[ nCntKey ] ^ ( m_aKeyState[ nCntKey ].press | aKeyState[ nCntKey ] );
			m_aKeyState[ nCntKey ].press = aKeyState[ nCntKey ];

			if( m_aKeyState[ nCntKey ].press )
			{
				m_aKeyState[ nCntKey ].repeatCnt++;
				if( m_aKeyState[ nCntKey ].repeatCnt >= WAIT_COUNT_REPEAT )
				{
					m_aKeyState[ nCntKey ].repeat = m_aKeyState[ nCntKey ].press;
				}
			}
			else
			{
				m_aKeyState[ nCntKey ].repeatCnt = 0;
				m_aKeyState[ nCntKey ].repeat = 0;
			}
		}
	}
	else
	{
		m_pDevKeyboard->Acquire();
	}
}

/*******************************************************************************
* 関数名：bool CInput::GetKeyBoardPress( int nKey )
* 
* 引数	：
* 戻り値：
* 説明	：プレス処理(押しっぱなし)
*******************************************************************************/
bool CInput::GetKeyboardPress( int nKey )
{
	return ( m_aKeyState[ nKey ].press&0x80 )? true: false;
}

/*******************************************************************************
* 関数名：bool CInput::GetKeyboardTrigger( int nKey )
* 
* 引数	：
* 戻り値：
* 説明	：トリガー処理(押したとき)
*******************************************************************************/
bool CInput::GetKeyboardTrigger( int nKey )
{
	return ( m_aKeyState[ nKey ].trigger&0x80 )? true: false;
}

/*******************************************************************************
* 関数名：bool CInput::GetKeyboardRelease( int nKey )
* 
* 引数	：
* 戻り値：
* 説明	：リリース処理(離したとき)
*******************************************************************************/
bool CInput::GetKeyboardRelease( int nKey )
{
	return ( m_aKeyState[ nKey ].release&0x80 )? true: false;
}

/*******************************************************************************
* 関数名：bool CInput::GetKeyboardRepeat( int nKey )
* 
* 引数	：
* 戻り値：
* 説明	：リピート処理(押してる間繰り返し)
*******************************************************************************/
bool CInput::GetKeyboardRepeat( int nKey )
{
	return ( m_aKeyState[ nKey ].repeat&0x80 )? true: false;
}

/*******************************************************************************
* 関数名：HRESULT CInput::InitMouse( HINSTANCE hInstance, HWND hWnd )
* 
* 引数	：
* 戻り値：
* 説明	：マウス初期化
*******************************************************************************/
HRESULT CInput::InitMouse( HINSTANCE hInstance, HWND hWnd )
{
	// デバイス・オブジェクトを作成
	if( FAILED( m_pInput->CreateDevice( GUID_SysMouse, &m_pDevMouse, NULL ) ) )
	{
		MessageBox( NULL,"DirectInputDevice8オブジェクトのの作成に失敗","Direct Input Error", MB_OK );
		return E_FAIL;
	}
	// データ・フォーマットを設定
	if( FAILED( m_pDevMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		MessageBox( NULL,"c_dfDIMouse2 形式の設定に失敗", "Direct Input Error", MB_OK );
		return E_FAIL;
	}
	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if( FAILED( m_pDevMouse->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) ) )
	{
		MessageBox( NULL, "フォアグラウンド＆非排他モードの設定に失敗", "Direct Input Error", MB_OK );
		return E_FAIL;
	}
	
	// 軸モードを設定（相対値モードに設定）
	DIPROPDWORD diprop;
	diprop.diph.dwSize  = sizeof(diprop); 
	diprop.diph.dwHeaderSize    = sizeof(diprop.diph); 
	diprop.diph.dwObj   = 0;
	diprop.diph.dwHow   = DIPH_DEVICE;
	diprop.dwData       = DIPROPAXISMODE_REL;
	//diprop.dwData       = DIPROPAXISMODE_ABS; // 絶対値モードの場合
	if ( FAILED( m_pDevMouse->SetProperty( DIPROP_AXISMODE, &diprop.diph ) ) )
	{	
		MessageBox( NULL, "軸モードの設定に失敗", "Direct Input Error", MB_OK);
		return E_FAIL;
	}


	
	// 入力制御開始
	m_pDevMouse->Acquire();

	m_aMouseState.hWnd = hWnd;		// マウスローカル座標取得用にウィンドウハンドルを保存
	m_aMouseState.bWindow = true;
	
	return S_OK;
}

/*******************************************************************************
* 関数名：void CInput::UninitMouse( void )
* 
* 引数	：
* 戻り値：
* 説明	：マウス終了処理
*******************************************************************************/
void CInput::UninitMouse( void )
{
	if( m_pDevMouse != NULL )
	{
		m_pDevMouse->Unacquire();
		m_pDevMouse->Release();
		m_pDevMouse = NULL;
	}
}

/*******************************************************************************
* 関数名：void CInput::UpdateMouse( void )
* 
* 引数	：
* 戻り値：
* 説明	：マウス更新処理
*******************************************************************************/
HRESULT CInput::UpdateMouse( void )
{
	HRESULT hr;						// 結果ハンドル
	DIMOUSESTATE2 mouseState;		// マウス入力バッファ
	int loopMouse;					// マウスループ

	// マウス入力の取得
	hr = m_pDevMouse->GetDeviceState( sizeof ( DIMOUSESTATE2 ), &mouseState );

	if ( SUCCEEDED( hr ) )
	{
		if ( m_aMouseState.bWindow )
		{
			GetCursorPos( &m_aMouseState.localPos ); // マウス座標取得
			ScreenToClient( m_aMouseState.hWnd, &m_aMouseState.localPos ); // クライアント領域内の相対座標へ変換
		}
		else
		{
			GetCursorPos( &m_aMouseState.localPos );
		}

		for ( loopMouse = 0; loopMouse < DIM_MAX; loopMouse++ )
		{
			m_aMouseState.trigBtns[ loopMouse ]		= ~m_aMouseState.state.rgbButtons[ loopMouse ] & mouseState.rgbButtons[ loopMouse ];
			m_aMouseState.releaseBtns[ loopMouse ]	=  m_aMouseState.state.rgbButtons[ loopMouse ] & ~mouseState.rgbButtons[ loopMouse ];
		}

		m_aMouseState.state = mouseState;
	}
	else
	{
		m_pDevMouse->Acquire(); // アクセス権の再取得
	}

	return hr;
}

/*******************************************************************************
* 関数名：void CInput::UninitMouse( void )
* 
* 引数	：
* 戻り値：
* 説明	：マウス終了処理
*******************************************************************************/
MOUSESTATE *CInput::GetMouseState( void )
{
	return &m_aMouseState;
}

/*******************************************************************************
* 関数名：bool CInput::GetMousePress( MOUSESTATE_BUTTONS nMouse )
* 
* 引数	：
* 戻り値：
* 説明	：マウスのボタンプレスの取得
*******************************************************************************/
bool CInput::GetMousePress( MOUSESTATE_BUTTONS nMouse )
{
	return ( m_aMouseState.state.rgbButtons[ nMouse ] & 0x80 ) ? true : false;
}

/*******************************************************************************
* 関数名：bool CInput::GetMouseTrigger( MOUSESTATE_BUTTONS nMouse )
* 
* 引数	：
* 戻り値：
* 説明	：マウスのボタンプレスの取得
*******************************************************************************/
bool CInput::GetMouseTrigger( MOUSESTATE_BUTTONS nMouse )
{
	return ( m_aMouseState.trigBtns[ nMouse ] & 0x80 ) ? true : false;
}

/*******************************************************************************
* 関数名：bool CInput::GetMouseTrigger( MOUSESTATE_BUTTONS nMouse )
* 
* 引数	：
* 戻り値：
* 説明	：マウスのボタンプレスの取得
*******************************************************************************/
bool CInput::GetMouseRelease( MOUSESTATE_BUTTONS nMouse )
{
	return ( m_aMouseState.releaseBtns[ nMouse ] & 0x80 ) ? true : false;
}