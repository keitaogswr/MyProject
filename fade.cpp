/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Ffade.cpp
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
#include "manager.h"
#include "renderer.h"
#include "fade.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( SCREEN_WIDTH )
#define HEIGHT			( SCREEN_HEIGHT )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define FADECOUNT		( 30 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
CMode *CFade::m_NextMode;
FADE CFade::m_State = FADE_IN;
int CFade::m_Count;
LPDIRECT3DVERTEXBUFFER9 CFade::m_VtxBuff;
float CFade::m_Alfa;

/*******************************************************************************
* �֐����FCFade::CFade()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CFade::CFade()
{
}

/*******************************************************************************
* �֐����FCFade::~CFade()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CFade::~CFade()
{
}

/*******************************************************************************
* �֐����Fvoid CFade::Init( void )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CFade::Init( void )
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	//D3DXCreateTextureFromFile(
	//	pDevice,
	//	"data\\TEXTURE\\reticle001.png",
	//	&m_Texture );

	// ���_�o�b�t�@�̐���
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_2D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_2D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( 0.0f,
								 0.0f,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( SCREEN_WIDTH,
								 0.0f,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( 0.0f,
								 SCREEN_HEIGHT,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( SCREEN_WIDTH,
								 SCREEN_HEIGHT,
								 0.0f );

	pVtx[ 0 ].rhw =
	pVtx[ 1 ].rhw =
	pVtx[ 2 ].rhw =
	pVtx[ 3 ].rhw = 1.0f;

	pVtx[ 0 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );
	pVtx[ 1 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );
	pVtx[ 2 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );
	pVtx[ 3 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

	pVtx[ 0 ].tex = D3DXVECTOR2( 0.0f, 0.0f );
	pVtx[ 1 ].tex = D3DXVECTOR2( 1.0f / TEXTURE_ROW, 0.0f );
	pVtx[ 2 ].tex = D3DXVECTOR2( 0.0f, 1.0f / TEXTURE_COLUMN );
	pVtx[ 3 ].tex = D3DXVECTOR2( 1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN );

	m_VtxBuff->Unlock();

}

/*******************************************************************************
* �֐����Fvoid CFade::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CFade::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_VtxBuff );
}

/*******************************************************************************
* �֐����Fvoid CFade::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CFade::Update( void )
{

	if( m_State == FADE_IN )
	{
		m_Alfa -= 1.0f / FADECOUNT;
		if( m_Count > FADECOUNT )
		{
			m_State = FADE_NONE;
			m_Count = 0;
			m_Alfa = 0.0f;
		}
	}
	else if( m_State == FADE_OUT )
	{
		m_Alfa += 1.0f / FADECOUNT;
		if( m_Count > FADECOUNT )
		{
			CManager::SetMode( m_NextMode );
			m_State = FADE_IN;
			m_Count = 0;
			m_Alfa = 1.0f;
		}
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].col = 
	pVtx[ 1 ].col = 
	pVtx[ 2 ].col = 
	pVtx[ 3 ].col = D3DXCOLOR( 0.0f, 0.0f, 0.0f, m_Alfa );

	m_VtxBuff->Unlock();

	m_Count++;
}

/*******************************************************************************
* �֐����Fvoid CFade::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CFade::Draw( void )
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_2D ) );

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_2D );

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, NULL );

	// �|���S���̕`��
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
					0,							//�ŏ��̒��_�̃C���f�b�N�X
					2 );						//�`�悷��v���~�e�B�u��
}

/*******************************************************************************
* �֐����Fvoid CFade::Start( CMode *NextMode )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�F�[�h�J�n����
*******************************************************************************/
void CFade::Start( CMode *NextMode )
{
	if( m_State == FADE_NONE )
	{
		m_NextMode = NextMode;
		m_State = FADE_OUT;
		m_Count = 0;
		m_Alfa = 0.0f;
	}
}