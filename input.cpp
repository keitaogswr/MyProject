/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Finput.cpp
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WAIT_COUNT_REPEAT	( 20 )		// ���s�[�g�J�n�܂ł̑҂�����

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
LPDIRECTINPUT8	CInput::m_pInput;
LPDIRECTINPUTDEVICE8 CInput::m_pDevKeyboard;
LPDIRECTINPUTDEVICE8 CInput::m_pDevMouse;

KEYSTATE CInput::m_aKeyState[ 256 ];
MOUSESTATE CInput::m_aMouseState;

/*******************************************************************************
* �֐����FHRESULT CInput::Init( HINSTANCE hInstance, HWND hWnd )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
HRESULT CInput::Init( HINSTANCE hInstance, HWND hWnd )
{
	if( m_pInput == NULL )
	{
		// DirectInput�I�u�W�F�N�g�̍쐬
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
			MessageBox( hWnd, "DirectInput8�I�u�W�F�N�g�̍쐬���ł��܂���ł����B", "�x���I", MB_ICONWARNING );
			return E_FAIL;
		}
	}
	return S_OK;
}

/*******************************************************************************
* �֐����Fvoid CInput::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CInput::Uninit( void )
{
	UninitKeyboard();
	SAFE_RELEASE( m_pInput );
}

/*******************************************************************************
* �֐����Fvoid CInput::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CInput::Update( void )
{

}

/*******************************************************************************
* �֐����FHRESULT CInput::InitKeyboard( HINSTANCE hInstance, HWND hWnd )
* 
* ����	�F
* �߂�l�F
* ����	�F�L�[�{�[�h����������
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
* �֐����Fvoid CInput::UninitKeyboard( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�L�[�{�[�h�I������
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
* �֐����Fvoid UpdateKeyboard( void )
* 
* ����	�F

* �߂�l�F
* ����	�F�L�[�{�[�h�X�V����
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
* �֐����Fbool CInput::GetKeyBoardPress( int nKey )
* 
* ����	�F
* �߂�l�F
* ����	�F�v���X����(�������ςȂ�)
*******************************************************************************/
bool CInput::GetKeyboardPress( int nKey )
{
	return ( m_aKeyState[ nKey ].press&0x80 )? true: false;
}

/*******************************************************************************
* �֐����Fbool CInput::GetKeyboardTrigger( int nKey )
* 
* ����	�F
* �߂�l�F
* ����	�F�g���K�[����(�������Ƃ�)
*******************************************************************************/
bool CInput::GetKeyboardTrigger( int nKey )
{
	return ( m_aKeyState[ nKey ].trigger&0x80 )? true: false;
}

/*******************************************************************************
* �֐����Fbool CInput::GetKeyboardRelease( int nKey )
* 
* ����	�F
* �߂�l�F
* ����	�F�����[�X����(�������Ƃ�)
*******************************************************************************/
bool CInput::GetKeyboardRelease( int nKey )
{
	return ( m_aKeyState[ nKey ].release&0x80 )? true: false;
}

/*******************************************************************************
* �֐����Fbool CInput::GetKeyboardRepeat( int nKey )
* 
* ����	�F
* �߂�l�F
* ����	�F���s�[�g����(�����Ă�ԌJ��Ԃ�)
*******************************************************************************/
bool CInput::GetKeyboardRepeat( int nKey )
{
	return ( m_aKeyState[ nKey ].repeat&0x80 )? true: false;
}

/*******************************************************************************
* �֐����FHRESULT CInput::InitMouse( HINSTANCE hInstance, HWND hWnd )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X������
*******************************************************************************/
HRESULT CInput::InitMouse( HINSTANCE hInstance, HWND hWnd )
{
	// �f�o�C�X�E�I�u�W�F�N�g���쐬
	if( FAILED( m_pInput->CreateDevice( GUID_SysMouse, &m_pDevMouse, NULL ) ) )
	{
		MessageBox( NULL,"DirectInputDevice8�I�u�W�F�N�g�̂̍쐬�Ɏ��s","Direct Input Error", MB_OK );
		return E_FAIL;
	}
	// �f�[�^�E�t�H�[�}�b�g��ݒ�
	if( FAILED( m_pDevMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		MessageBox( NULL,"c_dfDIMouse2 �`���̐ݒ�Ɏ��s", "Direct Input Error", MB_OK );
		return E_FAIL;
	}
	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	if( FAILED( m_pDevMouse->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) ) )
	{
		MessageBox( NULL, "�t�H�A�O���E���h����r�����[�h�̐ݒ�Ɏ��s", "Direct Input Error", MB_OK );
		return E_FAIL;
	}
	
	// �����[�h��ݒ�i���Βl���[�h�ɐݒ�j
	DIPROPDWORD diprop;
	diprop.diph.dwSize  = sizeof(diprop); 
	diprop.diph.dwHeaderSize    = sizeof(diprop.diph); 
	diprop.diph.dwObj   = 0;
	diprop.diph.dwHow   = DIPH_DEVICE;
	diprop.dwData       = DIPROPAXISMODE_REL;
	//diprop.dwData       = DIPROPAXISMODE_ABS; // ��Βl���[�h�̏ꍇ
	if ( FAILED( m_pDevMouse->SetProperty( DIPROP_AXISMODE, &diprop.diph ) ) )
	{	
		MessageBox( NULL, "�����[�h�̐ݒ�Ɏ��s", "Direct Input Error", MB_OK);
		return E_FAIL;
	}


	
	// ���͐���J�n
	m_pDevMouse->Acquire();

	m_aMouseState.hWnd = hWnd;		// �}�E�X���[�J�����W�擾�p�ɃE�B���h�E�n���h����ۑ�
	m_aMouseState.bWindow = true;
	
	return S_OK;
}

/*******************************************************************************
* �֐����Fvoid CInput::UninitMouse( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X�I������
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
* �֐����Fvoid CInput::UpdateMouse( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X�X�V����
*******************************************************************************/
HRESULT CInput::UpdateMouse( void )
{
	HRESULT hr;						// ���ʃn���h��
	DIMOUSESTATE2 mouseState;		// �}�E�X���̓o�b�t�@
	int loopMouse;					// �}�E�X���[�v

	// �}�E�X���͂̎擾
	hr = m_pDevMouse->GetDeviceState( sizeof ( DIMOUSESTATE2 ), &mouseState );

	if ( SUCCEEDED( hr ) )
	{
		if ( m_aMouseState.bWindow )
		{
			GetCursorPos( &m_aMouseState.localPos ); // �}�E�X���W�擾
			ScreenToClient( m_aMouseState.hWnd, &m_aMouseState.localPos ); // �N���C�A���g�̈���̑��΍��W�֕ϊ�
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
		m_pDevMouse->Acquire(); // �A�N�Z�X���̍Ď擾
	}

	return hr;
}

/*******************************************************************************
* �֐����Fvoid CInput::UninitMouse( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X�I������
*******************************************************************************/
MOUSESTATE *CInput::GetMouseState( void )
{
	return &m_aMouseState;
}

/*******************************************************************************
* �֐����Fbool CInput::GetMousePress( MOUSESTATE_BUTTONS nMouse )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X�̃{�^���v���X�̎擾
*******************************************************************************/
bool CInput::GetMousePress( MOUSESTATE_BUTTONS nMouse )
{
	return ( m_aMouseState.state.rgbButtons[ nMouse ] & 0x80 ) ? true : false;
}

/*******************************************************************************
* �֐����Fbool CInput::GetMouseTrigger( MOUSESTATE_BUTTONS nMouse )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X�̃{�^���v���X�̎擾
*******************************************************************************/
bool CInput::GetMouseTrigger( MOUSESTATE_BUTTONS nMouse )
{
	return ( m_aMouseState.trigBtns[ nMouse ] & 0x80 ) ? true : false;
}

/*******************************************************************************
* �֐����Fbool CInput::GetMouseTrigger( MOUSESTATE_BUTTONS nMouse )
* 
* ����	�F
* �߂�l�F
* ����	�F�}�E�X�̃{�^���v���X�̎擾
*******************************************************************************/
bool CInput::GetMouseRelease( MOUSESTATE_BUTTONS nMouse )
{
	return ( m_aMouseState.releaseBtns[ nMouse ] & 0x80 ) ? true : false;
}