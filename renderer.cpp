/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Frenderer.cpp
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "renderer.h"
#include "main.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define STARTPOS	( 300 )
#define ENDPOS		( 3000 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCRenderer::CRenderer()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CRenderer::CRenderer()
{
	m_Wireframe = WIREFRAME_NONE;
	m_bFog = true;
}

/*******************************************************************************
* �֐����FCRenderer::~CRenderer()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CRenderer::~CRenderer()
{
}

/*******************************************************************************
* �֐����Fvoid CRenderer::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
HRESULT CRenderer::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
{
	/*D3DXCreateFont( m_D3DDevice, 18, 0, 0, 0, FALSE,
					SHIFTJIS_CHARSET,
					OUT_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH,
					"Terminal",
					&m_Font );*/

	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;


	m_D3D = Direct3DCreate9( D3D_SDK_VERSION );					//Direct3D�I�u�W�F�N�g�̐���

	if( m_D3D == NULL )
	{
		return E_FAIL;
	}
	if( FAILED( m_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		return E_FAIL;
	};

	ZeroMemory( &d3dpp, sizeof( d3dpp ) );							//�[���N���A
	
	d3dpp.BackBufferWidth = SCREEN_WIDTH;							//��ʂ̕�
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;							//����
	d3dpp.BackBufferFormat = d3ddm.Format;							//�o�b�N�o�b�t�@�`��
	d3dpp.BackBufferCount = 1;										//�o�b�N�o�b�t�@�̐�

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;							//�f�v�X�o�b�t�@�ƃX�e���V���o�b�t�@�̐���
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					//�f�v�X�o�b�t�@�Ƃ���24�r�b�g���g�p
	d3dpp.Windowed = bWindow;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	if( FAILED ( m_D3D->CreateDevice(
					D3DADAPTER_DEFAULT,
					D3DDEVTYPE_HAL,
					hWnd,
					D3DCREATE_HARDWARE_VERTEXPROCESSING/* | D3DCREATE_MULTITHREADED*/,
					&d3dpp,
					&m_D3DDevice ) ) )
	{
		if( FAILED ( m_D3D->CreateDevice(
						D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						hWnd,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						&d3dpp,
						&m_D3DDevice ) ) )
		{
			if( FAILED ( m_D3D->CreateDevice(
							D3DADAPTER_DEFAULT,
							D3DDEVTYPE_REF,
							hWnd,
							D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							&d3dpp,
							&m_D3DDevice ) ) )
			{
				return E_FAIL;
			}
		}
	}

	// �����_�[�X�e�[�g�̐ݒ�
	m_D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// �T���v���[�X�e�[�g�̐ݒ�
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );	// �e�N�X�`��(U�l),�J��Ԃ��ɐݒ�
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );	// �e�N�X�`��(U�l),�J��Ԃ��ɐݒ�

	// �e�N�X�`���X�e�[�W�X�e�[�g�̐ݒ�
	m_D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );	// ��2�̃e�N�X�`���̃A���t�@�l��������
	m_D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	// �e�N�X�`���̃A���t�@�l
	m_D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );	// ���݂̃A���t�@�l

	// Z�e�X�g��ON
	m_D3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// �t�H�O�̐ݒ�
	FLOAT StartPos   = STARTPOS;	//�J�n�ʒu
	FLOAT EndPos     = ENDPOS;		//�I���ʒu
	m_D3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	m_D3DDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0x77, 0x88, 0x99, 0xff));
	m_D3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);      //���_���[�h
	m_D3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);     //�e�[�u�����[�h
	m_D3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&StartPos));	//�J�n�ʒu
	m_D3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&EndPos));		//�I���ʒu

	return S_OK;
}

/*******************************************************************************
* �֐����Fvoid CRenderer::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CRenderer::Uninit( void )
{
	SAFE_RELEASE( m_D3DDevice );
	SAFE_RELEASE( m_D3D );
}

/*******************************************************************************
* �֐����Fvoid CRenderer::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CRenderer::Update( void )
{
#ifdef _DEBUG
	// ���C���[�t���[���ؑ�
	if( CInput::GetKeyboardTrigger( DIK_F8 ) )
	{
		m_Wireframe == WIREFRAME_NONE? m_Wireframe = WIREFRAME_ON: m_Wireframe = WIREFRAME_NONE;
	}
	if (CInput::GetKeyboardTrigger(DIK_F9))
	{
		m_bFog = m_bFog == false ? true : false;
	}
#endif
}

/*******************************************************************************
* �֐����Fvoid CRenderer::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CRenderer::Draw( void )
{
	
}

/*******************************************************************************
* �֐����Fvoid CRenderer::Begin( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`��J�n����
*******************************************************************************/
void CRenderer::Begin( void )
{
	// ��ʃN���A
	m_D3DDevice->Clear( 0, NULL, ( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL ), D3DCOLOR_RGBA( 50, 50, 50, 255 ), 1.0f, 0 );

	m_D3DDevice->BeginScene();							// �`��J�n

	// ���C���[�t���[���`��
	m_Wireframe == WIREFRAME_ON ?
	m_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) :
	m_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	// �t�H�O�`��
	m_bFog == true ?
	m_D3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE) :
	m_D3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

/*******************************************************************************
* �֐����Fvoid CRenderer::End( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`��I������
*******************************************************************************/
void CRenderer::End( void )
{
	m_D3DDevice->EndScene();							// �`��I��
	m_D3DDevice->Present( NULL, NULL, NULL, NULL );		// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̐؂�ւ�
}

/*******************************************************************************
* �֐����FLPDIRECT3DDEVICE9 GetDevice( void )
* 
* ����	�F
* �߂�l�F
* ����	�F
*******************************************************************************/
LPDIRECT3DDEVICE9 CRenderer::GetDevice( void )
{
	// �f�o�C�X��Ԃ�
	return m_D3DDevice;
}