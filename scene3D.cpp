/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene3D.cpp
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
#include "scene3D.h"
#include "input.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCScene3D::CScene3D()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CScene3D::CScene3D( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene( DrawOrder, ObjType )
{
	m_Pos = Vector3( 0.0f, 0.0f, 0.0f );
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Trans = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_pTexture = NULL;
	m_VtxBuff = NULL;
}

/*******************************************************************************
* �֐����FCScene3D::~CScene3D()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CScene3D::~CScene3D()
{
}

/*******************************************************************************
* �֐����Fvoid CScene3D::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CScene3D::Init( Vector3 pos )
{
	m_Pos = pos;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(
		pDevice,
		"data\\TEXTURE\\ore.jpg",
		m_pTexture);

	// ���_�o�b�t�@�̐���
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( -WIDTH * 0.5f,
								 m_Pos.y,
								 DEPTH * 0.5f );
	pVtx[ 1 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 m_Pos.y,
								 DEPTH * 0.5f );
	pVtx[ 2 ].pos = D3DXVECTOR3( -WIDTH * 0.5f,
								 m_Pos.y,
								 -DEPTH * 0.5f );
	pVtx[ 3 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 m_Pos.y,
								 -DEPTH * 0.5f );

	pVtx[ 0 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 1 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 2 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 3 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

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
* �֐����Fvoid CScene3D::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CScene3D::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CScene3D::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CScene3D::Update( void )
{
}

/*******************************************************************************
* �֐����Fvoid CScene3D::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CScene3D::Draw( void )
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �����_���[�X�e�[�g�̐ݒ�
	SetRenderStateBegin();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxWorld );
	// �X�P�[���𔽉f
	D3DXMatrixScaling( &mtxScl, m_Scl.x, m_Scl.y, m_Scl.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxScl );
	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll( &mtxRot, m_Rot.y, m_Rot.x, m_Rot.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxRot );
	// �ʒu�𔽉f
	D3DXMatrixTranslation( &mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxTrans );
	
	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform( D3DTS_WORLD, &m_MtxWorld );

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_3D ) );

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_3D );

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, *m_pTexture);

	//�|���S���̕`��
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
					0,							//�ŏ��̒��_�̃C���f�b�N�X
					2 );						//�`�悷��v���~�e�B�u��

	// �����_���[�X�e�[�g�̐ݒ�
	SetRenderStateEnd();
}

/*******************************************************************************
* �֐����FCScene3D *CScene3D::Create( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CScene3D *CScene3D::Create( Vector3 pos )
{
	CScene3D *scene3D;
	scene3D = new CScene3D;
	scene3D->Init( pos );
	return scene3D;
}