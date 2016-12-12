/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fbg.cpp
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
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( SCREEN_WIDTH )
#define HEIGHT			( SCREEN_HEIGHT )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCBg::CBg()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CBg::CBg( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene2D( DrawOrder, ObjType )
{
	pFileName = NULL;
}

/*******************************************************************************
* �֐����FCBg::~CBg()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CBg::~CBg()
{
}

/*******************************************************************************
* �֐����Fvoid CBg::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CBg::Init( Vector3 pos, int nIdx)
{
	m_Pos = pos;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(nIdx);

	// ���_�o�b�t�@�̐���
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_2D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_2D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	m_fLength = sqrtf( WIDTH * WIDTH + HEIGHT * HEIGHT ) * 0.5f;
	m_fAngle = atan2f( WIDTH, HEIGHT );

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( m_Pos.x + sinf( D3DX_PI + m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( D3DX_PI + m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( m_Pos.x + sinf( D3DX_PI - m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( D3DX_PI - m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( m_Pos.x + sinf( -m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( -m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( m_Pos.x + sinf( m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( m_fAngle + m_Rot.z ) * m_fLength,
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
* �֐����Fvoid CBg::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CBg::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CBg::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBg::Update( void )
{

}

/*******************************************************************************
* �֐����Fvoid CBg::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CBg::Draw( void )
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// Z�e�X�g��ON
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_2D ) );

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_2D );

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, *m_pTexture);

	// �|���S���̕`��
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
					0,							//�ŏ��̒��_�̃C���f�b�N�X
					2 );						//�`�悷��v���~�e�B�u��

	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

/*******************************************************************************
* �֐����FCBg *CBg::Create( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CBg *CBg::Create( Vector3 pos, int nIdx )
{
	CBg *bg;
	bg = new CBg;
	bg->Init( pos, nIdx );
	return bg;
}