/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Finput.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#define DIRECTINPUT_VERSION ( 0x0800 )
#include "dinput.h"

/*******************************************************************************
* ���C�u�����̃����N
*******************************************************************************/
#pragma comment( lib, "dinput8.lib" )	// ���͏����ɕK�v
#pragma comment( lib, "dxguid.lib" )	// DirectX�R���|�[�l���g�g�p�ɕK�v

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/
/* �}�E�X�{�^���̗񋓌^ */
typedef enum
{
	DIM_LEFT			= 0,				// �}�E�X���{�^��
	DIM_RIGHT,								// �}�E�X�E�{�^��
	DIM_CENTER,								// �}�E�X�z�C�[���{�^��
	DIM_BUTTON3,							// �}�E�X�{�^��3
	DIM_BUTTON4,							// �}�E�X�{�^��4
	DIM_BUTTON5,							// �}�E�X�{�^��5
	DIM_BUTTON6,							// �}�E�X�{�^��6
	DIM_BUTTON7,							// �}�E�X�{�^��7
	DIM_MAX
}MOUSESTATE_BUTTONS;

/* �L�[�{�[�h�\���� */
typedef struct
{
	BYTE press;							// �L�[�v���X���
	BYTE trigger;						// �L�[�g���K�[���
	BYTE release;						// �L�[�����[�X���
	BYTE repeat;						// �L�[���s�[�g���
	unsigned repeatCnt;					// �L�[���s�[�g�J�E���^
}KEYSTATE;

/* �}�E�X�\���� */
typedef struct
{
	DIMOUSESTATE2 state;				// �}�E�X���\����
	BYTE trigBtns[ DIM_MAX ];			// �}�E�X�{�^���g���K�[���
	BYTE releaseBtns[ DIM_MAX ];		// �}�E�X�{�^�������[�X���

	bool bWindow;						// �E�B���h�E���[�h�t���O
	HWND hWnd;							// �}�E�X���W�擾�p�E�B���h�E�n���h��
	POINT localPos;						// �}�E�X���[�J�����W
}MOUSESTATE;

/*******************************************************************************
* �O���錾
*******************************************************************************/

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
// �C���v�b�g�N���X
class CInput
{
	private:
		static LPDIRECTINPUT8		m_pInput;					// IDirectInput8�C���^�[�t�F�[�X�ւ̃|�C���^
		static LPDIRECTINPUTDEVICE8 m_pDevKeyboard;				// ���̓f�o�C�X�ւ̃|�C���^
		static LPDIRECTINPUTDEVICE8 m_pDevMouse;				// ���̓f�o�C�X�ւ̃|�C���^

		static KEYSTATE m_aKeyState[ 256 ];						// �L�[�̏��
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