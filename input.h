/******************************************************************************
	�^�C�g���� :�C���v�b�g�N���X
	�t�@�C���� : input.h
	�쐬��     : AT-13C-284 36 �[�V�C��
	�쐬��     : 2016/04/26
	�X�V��     :
******************************************************************************/
/******************************************************************************
	�C���N���[�h�K�[�h
******************************************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

/******************************************************************************
	�C���N���[�h�t�@�C��
******************************************************************************/
#define DIRECTINPUT_VERSION ( 0x0800 )
#include "dinput.h"

#include "XInput.h"
#pragma comment(lib,"XInput.lib")

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )


/******************************************************************************
	�}�N����`
******************************************************************************/
const int MAX_KEY = 256;		//	�L�[�ő吔�B
const int MAX_MOUSE_KEY = 8;	//	�L�[�ő吔�B
const int MAX_CONTROLLERS = 1;	//	�R���g���[���[�̍ő吔�B
const int REPEAT_STATE_CNT = 40;//	���s�[�g�̃X�e�[�g�J�E���g�B

/******************************************************************************
	�\���̐錾
******************************************************************************/
typedef enum {
	MOUSE_INPUT_LEFT = 0,		//	���N���b�N�B
	MOUSE_INPUT_RIGHT,			//	�E�N���b�N�B
	MOUSE_INPUT_CENTER,			//	���N���b�N�B
	MOUSE_INPUT_MAX
}MOUSE_INPUT;

/******************************************************************************
	�N���X�錾
******************************************************************************/
class CInput
{
public:
	CInput();		//	�R���X�g���N�^�B
	~CInput();		//	�f�X�g���N�^�B
	
	//	�L�[�{�[�h�֘A///////////////////////////////////////////////////////////////////////////////////
	HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);		//	�L�[�{�[�h�̏������֐�(�����ł̓f�o�C�X�̐ݒ�����Ă���̂ŁA�}�E�X�̏������������ł���Ă܂��B)
	void UninitKeyboard(void);									//	�L�[�{�[�h�̏I���֐��B
	void UpdateKeyboard(void);									//	�L�[�{�[�h�̍X�V�֐��B

	bool GetKeyboardPress(int nKey) { return (m_aKeyState[nKey] & 0x80) ? true : false; };				//	�L�[�{�[�h�̃v���X���擾�B
	bool GetKeyboardTrigger(int nKey) { return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false; };		//	�L�[�{�[�h�̃g���K�[���擾�B
	bool GetKeyboardRelease(int nKey) { return (m_aKeyStateRelease[nKey] & 0x80) ? true : false; };		//	�L�[�{�[�h�̃����[�X���擾�B
	bool GetKeyboardRepeat(int nKey) { return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false; };		//	�L�[�{�[�h�̃��s�[�g���擾�B
	//	�L�[�{�[�h�֘A///////////////////////////////////////////////////////////////////////////////////
	//	�}�E�X�֘A///////////////////////////////////////////////////////////////////////////////////////
	void UninitMouse(void);		//	�}�E�X�̏I������
	void UpdateMouse(void);		//	�}�E�X�̍X�V����

	//	�}�E�X���̎擾�֐�
	DIMOUSESTATE2 GetMouse(void) { return m_Mouse; }

	bool GetMousePress(int nKey) { return (m_aMouseKeyState.rgbButtons[nKey] & 0x80) ? true : false; }	//	�}�E�X�̃v���X���擾�B
	bool GetMouseTrigger(int nKey) { return (m_aMouseKeyStateTrigger[nKey] & 0x80) ? true : false; }	//	�}�E�X�̃g���K�[���擾�B
	bool GetMouseRelease(int nKey) { return (m_aMouseKeyStateRelease[nKey] & 0x80) ? true : false; }	//	�}�E�X�̃����[�X���擾�B
	bool GetMouseRepeat(int nKey) { return (m_aMouseKeyStateRepeat[nKey] & 0x80) ? true : false; }		//	�}�E�X�̃��s�[�g���擾�B
	//	�}�E�X�֘A///////////////////////////////////////////////////////////////////////////////////////
	//	JoyStick/////////////////////////////////////////////////////////////////////////////////////////
	void InitJoyStick(void);		//	JoyStick�̏������֐��B
	void UninitJoyStick(void);		//	JoyStick�̏I�������B
	void UpdateJoyStick(void);		//	JoyStick�̍X�V�����B

	//	JoyStick�̓��͂��󂯎��֐�
	bool PressJoyStick(WORD nKey) { return (m_PressState.Gamepad.wButtons & nKey) ? true : false; };		//	JoyStick�̏��擾�B
	bool TriggerJoyStick(WORD nKey) { return (m_TriggerState.Gamepad.wButtons & nKey) ? true : false; };	//	JoyStick�̏��擾�B
	bool ReleaseJoyStick(WORD nKey) { return (m_ReleaseState.Gamepad.wButtons & nKey) ? true : false; };	//	JoyStick�̏��擾�B
	bool RepeatJoyStick(WORD nKey) { return (m_RepeatState.Gamepad.wButtons & nKey) ? true : false; };		//	JoyStick�̏��擾�B
	bool GetJoyStickConnected(void) { return m_Connected; }			//	JoyStick�̐ڑ��󋵎擾�B
	XINPUT_STATE *GetPressState(void) { return &m_PressState; }
	//	JoyStick/////////////////////////////////////////////////////////////////////////////////////////
private:
	//	�L�[�{�[�h�֘A///////////////////////////////////////////////////////////////////////////////////
	LPDIRECTINPUT8       m_pInput;
	LPDIRECTINPUTDEVICE8 m_pDevKeyBoard;
	BYTE m_aKeyState[MAX_KEY];			//�L�[�{�[�h�̓��͏�񃏁[�N(������/�����ĂȂ��A�̏�񂪓���)�B
	BYTE m_aKeyStateTrigger[MAX_KEY];	//�L�[�g���K�[���B
	BYTE m_aKeyStateRelease[MAX_KEY];	//�L�[�����[�X���B
	BYTE m_aKeyStateRepeat[MAX_KEY];	//�L�[���s�[�g���B
	int  m_aKeystateRepeatCnt[MAX_KEY];	//�L�[���s�[�g�J�E���^�B
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//	�}�E�X�֘A///////////////////////////////////////////////////////////////////////////////////////
	LPDIRECTINPUTDEVICE8 m_pDevMouse;
	DIMOUSESTATE2		 m_Mouse;
	DIMOUSESTATE2 m_aMouseKeyState;					//	�}�E�X�̓��͏�񃏁[�N(������/�����ĂȂ��A�̏�񂪓���)�B
	BYTE m_aMouseKeyStateTrigger[MAX_MOUSE_KEY];	//	�}�E�X�̃g���K�[���B
	BYTE m_aMouseKeyStateRelease[MAX_MOUSE_KEY];	//	�}�E�X�̃����[�X���B
	BYTE m_aMouseKeyStateRepeat[MAX_MOUSE_KEY];		//	�}�E�X�̃��s�[�g���B
	int  m_aMouseKeyStateRepeatCnt[MAX_MOUSE_KEY];	//	�}�E�X�̃��s�[�g�J�E���^�B
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//	JoyStick/////////////////////////////////////////////////////////////////////////////////////////
	XINPUT_STATE m_PressState;			//	JoyStick�̃v���X���B
	XINPUT_STATE m_TriggerState;		//	JoyStick�̃g���K�[���B
	XINPUT_STATE m_ReleaseState;		//	JoyStick�̃����[�X���B
	XINPUT_STATE m_RepeatState;			//	JoyStick�̃��s�[�g���B
	int			 m_RepeatStateCnt;		//	JoyStick�̃��s�[�g�̃J�E���^�B
	bool         m_Connected;			//	�ڑ�����Ă��邩�ǂ����B
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);		//	�������֐��B
	void Uninit(void);		//	�I���֐��B
	void Update(void);		//	�X�V�֐��B
};

#endif