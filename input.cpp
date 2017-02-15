/******************************************************************************
	�^�C�g���� :�C���v�b�g�N���X
	�t�@�C���� : input.cpp
	�쐬��     : AT-13C-284 36 �[�V�C��
	�쐬��     : 2016/04/26
	�X�V��     :
******************************************************************************/
/******************************************************************************
	�C���N���[�h�t�@�C��
******************************************************************************/
#include "renderer.h"
#include "main.h"
#include "input.h"

/******************************************************************************
	�֐��� : CInput::CInput()
	����   : �R���X�g���N�^
			 �ϐ��̏������ƁA�|�C���^��NULL����
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
	�֐��� : CInput::~CInput()
	����   : �f�X�g���N�^
******************************************************************************/
CInput::~CInput()
{

}

/******************************************************************************
	�֐��� : HRESULT CInput::Init(void)
	����   : void
	�߂�l : E_FAIL, S_OK
	����   : DirectInput�I�u�W�F�N�g�̐���
******************************************************************************/
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == NULL) {
		//DirectInput�I�u�W�F�N�g�̐���
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

/******************************************************************************
	�֐��� : void CInput::Uninit(void)
	����   : void
	�߂�l : �Ȃ�
	����   : �|�C���^�̉�������B
******************************************************************************/
void CInput::Uninit(void)
{
	if (m_pInput != NULL) {
		m_pInput->Release();
		m_pInput = NULL;
	}
}

/******************************************************************************
	�֐��� : void CInput::Update(void)
	����   : void
	�߂�l : �Ȃ�
	����   : ���ɂȂ�
******************************************************************************/
void CInput::Update(void)
{

}

/******************************************************************************
	�֐��� : HRESULT CInput::InitKeyboard(HINSTANCE hInstance, HWND hWnd)
	����   : hInstance, hWnd
	�߂�l : E_FAIL, S_OK
	����   : Keyboard�̏�����, �f�o�C�X�̐ݒ�, �f�[�^�t�H�[�}�b�g�̐ݒ�, �������[�h�̐ݒ�, �A�N�Z�X�����擾
******************************************************************************/
HRESULT CInput::InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	int nKeyID;

	//Keyboard�̏�����
	if (FAILED(Init(hInstance, hWnd))) {
		nKeyID = MessageBox(hWnd, " �������Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�f�o�C�X�̐ݒ�(KeyBoard)
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevKeyBoard, NULL))) {
		nKeyID = MessageBox(hWnd, " �f�o�C�X�̐ݒ�Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�f�o�C�X�̐ݒ�(Mouse)
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevMouse, NULL))) {
		nKeyID = MessageBox(hWnd, " �f�o�C�X�̐ݒ�Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g�̐ݒ�(KeyBoard)
	if (FAILED(m_pDevKeyBoard->SetDataFormat(&c_dfDIKeyboard))) {
		nKeyID = MessageBox(hWnd, " �f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g�̐ݒ�(Mouse)
	if (FAILED(m_pDevMouse->SetDataFormat(&c_dfDIMouse2))) {
		nKeyID = MessageBox(hWnd, " �f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�������[�h�̐ݒ�(KeyBoard)
	if (FAILED(m_pDevKeyBoard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))) {
		nKeyID = MessageBox(hWnd, " �������[�h�̐ݒ�Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�������[�h�̐ݒ�(Mouse)
	if (FAILED(m_pDevMouse->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))) {
		nKeyID = MessageBox(hWnd, " �������[�h�̐ݒ�Ɏ��s���܂����B ", "  ", MB_YESNO + MB_ICONQUESTION);

		if (nKeyID == IDYES) {
			DestroyWindow(hWnd);//�E�B���h�E��j��
		}

		return E_FAIL;
	}

	//�A�N�Z�X�����擾
	m_pDevKeyBoard->Acquire();
	m_pDevMouse->Acquire();
	InitJoyStick();
	return S_OK;
}

/******************************************************************************
	�֐��� : void CInput::UninitKeyboard(void)
	����   : void
	�߂�l : �Ȃ�
	����   : �擾�����A�N�Z�X���̊J��
******************************************************************************/
void CInput::UninitKeyboard(void)
{
	//	�L�[�{�[�h
	if (m_pDevKeyBoard != NULL) {
		m_pDevKeyBoard->Unacquire();		//�A�N�Z�X���̊J��
		m_pDevKeyBoard->Release();
		m_pDevKeyBoard = NULL;
	}

	//	�}�E�X�̉��
	UninitMouse();
	UninitJoyStick();
	Uninit();
}

/******************************************************************************
	�֐��� : void CInput::UninitMouse(void)
	����   : void
	�߂�l : �Ȃ�
	����   : �擾�����A�N�Z�X���̊J��
******************************************************************************/
void CInput::UninitMouse(void)
{
	//	�}�E�X
	if (m_pDevMouse != NULL) {
		m_pDevMouse->Unacquire();		//�A�N�Z�X���̊J��
		m_pDevMouse->Release();
		m_pDevMouse = NULL;
	}
}

/******************************************************************************
	�֐��� : void CInput::UpdateKeyboard(void)
	����   : void
	�߂�l : �Ȃ�
	����   : Keyboard�̏�Ԃ��擾
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

			//Pless����
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
		//�A�N�Z�X�����擾
		m_pDevKeyBoard->Acquire();
	}
}

/******************************************************************************
	�֐��� : void CInput::UpdateMouse(void)
	����   : void
	�߂�l : �Ȃ�
	����   : Mouse�̏�Ԃ��擾
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

			//Pless����
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
		//�A�N�Z�X�����擾
		m_pDevMouse->Acquire();
	}
}

/******************************************************************************
	�֐��� : void CInput::InitJoyStick(void)
	����   : void
	�߂�l : �Ȃ�
	����   : JoyStick�̏������B
******************************************************************************/
void CInput::InitJoyStick(void)
{
	//	��Ɨp�ϐ�
	DWORD dwResult;

	for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &state);

		//	�f���@�C�X�����邩�ǂ���
		if (dwResult == ERROR_SUCCESS) {// Controller is connected
		 //	Press���̏�����
			m_PressState.Gamepad.bLeftTrigger = NULL;
			m_PressState.Gamepad.bRightTrigger = NULL;
			m_PressState.Gamepad.sThumbLX = NULL;
			m_PressState.Gamepad.sThumbLY = NULL;
			m_PressState.Gamepad.sThumbRX = NULL;
			m_PressState.Gamepad.sThumbRY = NULL;
			m_PressState.Gamepad.wButtons = NULL;

			// Trigger���̏�����
			m_TriggerState.Gamepad.bLeftTrigger = NULL;
			m_TriggerState.Gamepad.bRightTrigger = NULL;
			m_TriggerState.Gamepad.sThumbLX = NULL;
			m_TriggerState.Gamepad.sThumbLY = NULL;
			m_TriggerState.Gamepad.sThumbRX = NULL;
			m_TriggerState.Gamepad.sThumbRY = NULL;
			m_TriggerState.Gamepad.wButtons = NULL;

			//	Release���̏�����
			m_ReleaseState.Gamepad.bLeftTrigger = NULL;
			m_ReleaseState.Gamepad.bRightTrigger = NULL;
			m_ReleaseState.Gamepad.sThumbLX = NULL;
			m_ReleaseState.Gamepad.sThumbLY = NULL;
			m_ReleaseState.Gamepad.sThumbRX = NULL;
			m_ReleaseState.Gamepad.sThumbRY = NULL;
			m_ReleaseState.Gamepad.wButtons = NULL;

			//	Repeat���̏�����
			m_RepeatState.Gamepad.bLeftTrigger = NULL;
			m_RepeatState.Gamepad.bRightTrigger = NULL;
			m_RepeatState.Gamepad.sThumbLX = NULL;
			m_RepeatState.Gamepad.sThumbLY = NULL;
			m_RepeatState.Gamepad.sThumbRX = NULL;
			m_RepeatState.Gamepad.sThumbRY = NULL;
			m_RepeatState.Gamepad.wButtons = NULL;

			//	RepeatCnt�̏�����
			m_RepeatStateCnt = 0;

			m_Connected = true;
		}
		else {// Controller is not connected 

		}
	}
}

/******************************************************************************
	�֐��� : void CInput::UninitJoyStick(void)
	����   : void
	�߂�l : �Ȃ�
	����   : JoyStick�̏I�������B�B
******************************************************************************/
void CInput::UninitJoyStick(void)
{
}

/******************************************************************************
	�֐��� : void CInput::UpdateJoyStick(void)
	����   : void
	�߂�l : �Ȃ�
	����   : JoyStick�̍X�V�����B�B
******************************************************************************/
void CInput::UpdateJoyStick(void)
{
	//	��Ɨp�ϐ�
	DWORD dwResult;
	XINPUT_STATE state;

	for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {
		//	0�ŏ�����
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		//	�f���@�C�X���擾
		dwResult = XInputGetState(i, &state);

		//	�f���@�C�X����������
		if (dwResult == ERROR_SUCCESS) {
			//Trigger
			m_TriggerState.Gamepad.wButtons = (m_PressState.Gamepad.wButtons | state.Gamepad.wButtons) ^ m_PressState.Gamepad.wButtons;

			//Release
			m_ReleaseState.Gamepad.wButtons = (m_PressState.Gamepad.wButtons | state.Gamepad.wButtons) ^ state.Gamepad.wButtons;
			if (ReleaseJoyStick((WORD)i) == true) {
				m_RepeatStateCnt = 0;
			}

			//Pless����
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
		else {		//	�f�o�C�X���Ȃ��B
			m_Connected = false;
		}
	}
}