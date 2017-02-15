/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fmanager.cpp
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
#include "renderer.h"
#include "sound.h"
#include "input.h"
#include "fade.h"
#include "mode.h"
#include "title.h"
#include "game.h"
#include "result.h"
#include "editor.h"
#include "load.h"
#include "texture.h"
#include "manager.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
CRenderer *CManager::m_Renderer;
CManager::OPMODE CManager::m_Opmode;
HWND *CManager::m_hWnd;
CMode *CManager::m_Mode;
CInput *CManager::m_Input;

/*******************************************************************************
* �֐����FCManager::CManager()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CManager::CManager()
{
	m_Renderer = NULL;
	m_Opmode = OPMODE_PLAYER;
}

/*******************************************************************************
* �֐����FCManager::~CManager()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CManager::~CManager()
{
}

/*******************************************************************************
* �֐����Fvoid CManager::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CManager::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
{
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	m_hWnd = &hWnd;

	m_Renderer = new CRenderer;
	m_Renderer->Init( hInstance, hWnd, bWindow );

	m_Input = new CInput;
	m_Input->InitKeyboard( hInstance, hWnd );
	m_Input->InitJoyStick();

	CSound::Init( hWnd );

	CTexture::Load();

	CFade::Init();
#ifdef _DEBUG
	CDebugProc::Init();
#endif

	SetMode(new CTitle);
}

/*******************************************************************************
* �֐����Fvoid CManager::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CManager::Uninit( void )
{
	m_Input->UninitKeyboard();
	SAFE_DELETE(m_Input);
	CFade::Uninit();

	m_Mode->Uninit();
	CTexture::Unload();
	m_Renderer->Uninit();
#ifdef _DEBUG
	CDebugProc::Uninit();
#endif
	// �������
	SAFE_DELETE(m_Renderer);
	SAFE_DELETE(m_Mode);
	// �T�E���h���
	CSound::Uninit();
}

/*******************************************************************************
* �֐����Fvoid CManager::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CManager::Update( void )
{
	m_Input->UpdateKeyboard();
	m_Input->UpdateMouse();
	m_Input->UpdateJoyStick();

	m_Renderer->Update();
	m_Mode->Update();
	
	CFade::Update();
#ifdef _DEBUG
	CDebugProc::Update();
#endif
}

/*******************************************************************************
* �֐����Fvoid CManager::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CManager::Draw( void )
{
	// �`��J�n
	m_Renderer->Begin();
	m_Mode->Draw();
	CFade::Draw();
#ifdef _DEBUG
	CDebugProc::Draw();
#endif
	m_Renderer->End();
	// �`��I��
}

/*******************************************************************************
* �֐����FCRenderer CManager::*GetRenderer( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�����_���[�擾����
*******************************************************************************/
CRenderer *CManager::GetRenderer( void )
{
	return m_Renderer;
}

/*******************************************************************************
* �֐����FHWND *CManager::GetHWND( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�E�C���h�E�n���h���擾����
*******************************************************************************/
HWND *CManager::GetHWND( void )
{
	return m_hWnd;
}

/*******************************************************************************
* �֐����FD3DXVECTOR3* CManager::CalcScreenToWorld( D3DXVECTOR3* pout,int Sx, int Sy, float fZ, int Screen_w, int Screen_h, D3DXMATRIX* View, D3DXMATRIX* Prj )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�N���[�������[���h�ϊ�����
*******************************************************************************/
D3DXVECTOR3* CManager::CalcScreenToWorld(
   D3DXVECTOR3* pout,
   int Sx,				// �X�N���[��X���W
   int Sy,				// �X�N���[��Y���W
   float fZ,			// �ˉe��Ԃł�Z�l�i0�`1�j
   int Screen_w,		// �X�N���[����
   int Screen_h,		// �X�N���[������
   D3DXMATRIX* View,	// �r���[�}�g���N�X
   D3DXMATRIX* Prj )	// �v���W�F�N�V�����}�g���N�X
{
	// �e�s��̋t�s����Z�o
	D3DXMATRIX InvView, InvPrj, VP, InvViewport;
	D3DXMatrixInverse( &InvView, NULL, View );
	D3DXMatrixInverse( &InvPrj, NULL, Prj );
	D3DXMatrixIdentity( &VP );
	VP._11 = Screen_w/2.0f; VP._22 = -Screen_h/2.0f;
	VP._41 = Screen_w/2.0f; VP._42 = Screen_h/2.0f;
	D3DXMatrixInverse( &InvViewport, NULL, &VP );
	
	// �t�ϊ�
	D3DXMATRIX tmp = InvViewport * InvPrj * InvView;
	D3DXVec3TransformCoord( pout, &D3DXVECTOR3( ( float )Sx, ( float )Sy, ( float )fZ ), &tmp );
	
	return pout;
}


/*******************************************************************************
* �֐����FD3DXVECTOR3* CManager::CalcScreenToXZ( D3DXVECTOR3* pout, int Sx, int Sy, int Screen_w, int Screen_h, D3DXMATRIX* View, D3DXMATRIX* Prj )
* 
* ����	�F
* �߂�l�F
* ����	�FXZ���ʂƃX�N���[�����W�̌�_�Z�o�֐�
*******************************************************************************/
D3DXVECTOR3* CManager::CalcScreenToXZ(
   D3DXVECTOR3* pout,
   int Sx,				// �X�N���[��X���W
   int Sy,				// �X�N���[��Y���W
   int Screen_w,		// �X�N���[����
   int Screen_h,		// �X�N���[������
   D3DXMATRIX* View,	// �r���[�}�g���N�X
   D3DXMATRIX* Prj)		// �v���W�F�N�V�����}�g���N�X
{
   D3DXVECTOR3 nearpos;
   D3DXVECTOR3 farpos;
   D3DXVECTOR3 ray;
   CalcScreenToWorld( &nearpos, Sx, Sy, 0.0f, Screen_w, Screen_h, View, Prj );
   CalcScreenToWorld( &farpos, Sx, Sy, 1.0f, Screen_w, Screen_h, View, Prj );
   ray = farpos - nearpos;
   D3DXVec3Normalize( &ray, &ray );

   // ���Ƃ̌������N���Ă���ꍇ�͌�_��
   // �N���Ă��Ȃ��ꍇ�͉����̕ǂƂ̌�_���o��
   if( ray.y <= 0 ) {
      // ����_
      float Lray = D3DXVec3Dot( &ray, &D3DXVECTOR3( 0, 1, 0 ) );
      float LP0 = D3DXVec3Dot( &( -nearpos ), &D3DXVECTOR3( 0, 1, 0 ) );
      *pout = nearpos + ( LP0 / Lray ) * ray;
   }
   else {
      *pout = farpos;
   }

   return pout;
}

/*******************************************************************************
* �֐����Fvoid CManager::CheckRot( float f )
* 
* ����	�F
* �߂�l�F
* ����	�F��]�p�C������
*******************************************************************************/
void CManager::CheckRot( float *f )
{
	if( *f > D3DX_PI )
	{
		*f -= D3DX_PI * 2;
	}
	if( *f < -D3DX_PI )
	{
		*f += D3DX_PI * 2;
	}
}

/*******************************************************************************
* �֐����Fvoid CManager::CheckRot( Vector3 *rot )
* 
* ����	�F
* �߂�l�F
* ����	�F��]�p(XYZ)�C������
*******************************************************************************/
void CManager::CheckRot( Vector3 *rot )
{
	// x
	if( rot->x > D3DX_PI )
	{
		rot->x -= D3DX_PI * 2;
	}
	if( rot->x < -D3DX_PI )
	{
		rot->x += D3DX_PI * 2;
	}
	// y
	if( rot->y > D3DX_PI )
	{
		rot->y -= D3DX_PI * 2;
	}
	if( rot->y < -D3DX_PI )
	{
		rot->y += D3DX_PI * 2;
	}
	// z
	if( rot->z > D3DX_PI )
	{
		rot->z -= D3DX_PI * 2;
	}
	if( rot->z < -D3DX_PI )
	{
		rot->z += D3DX_PI * 2;
	}
}

/*******************************************************************************
* �֐����Fvoid CManager::SetMode( CMode *mode )
* 
* ����	�F
* �߂�l�F
* ����	�F���[�h�ݒ菈��
*******************************************************************************/
void CManager::SetMode( CMode *mode )
{
	if( m_Mode != NULL )
	{
		m_Mode->Uninit();
		delete m_Mode;
		m_Mode = NULL;
	}
	m_Mode = mode;
	if (m_Mode->GetMultithreadFlg() == false)
	{
		m_Mode->Init();
	}
}

/*******************************************************************************
* �֐����Fvoid CManager::SetMode( CMode *mode )
* 
* ����	�F
* �߂�l�F
* ����	�F���[�h�ݒ菈��
*******************************************************************************/
CMode *CManager::GetMode( void )
{
	return m_Mode;
}