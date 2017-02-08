/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fbillboard.cpp
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
#include "mode.h"
#include "game.h"
#include "camera.h"
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "billboard.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define TEXTFILENAME	( "BillboardData.txt" )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCBillboard::CBillboard()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CBillboard::CBillboard( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene3D( DrawOrder, ObjType )
{
}

/*******************************************************************************
* �֐����FCBillboard::~CBillboard()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CBillboard::~CBillboard()
{
}

/*******************************************************************************
* �֐����Fvoid CBillboard::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CBillboard::Init( Vector3 pos )
{
	m_Pos = pos;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
								 HEIGHT,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 HEIGHT,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( -WIDTH * 0.5f,
								 0.0f,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 0.0f,
								 0.0f );

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
* �֐����Fvoid CBillboard::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CBillboard::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CBillboard::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBillboard::Update( void )
{
}

/*******************************************************************************
* �֐����FCBillboard *CBillboard::Create( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CBillboard *CBillboard::Create( Vector3 pos )
{
	CBillboard *billboard;
	billboard = new CBillboard;

	CGame *game = ( CGame* )CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();
	pos.y = meshField->GetHeight( pos );

	billboard->Init( pos );
	return billboard;
}

/*******************************************************************************
* �֐����Fvoid CBillboard::SetRenderStateBegin( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�J�n����
*******************************************************************************/
void CBillboard::SetRenderStateBegin(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e��ݒ� /////
	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// Z�e�X�g��ON
	/*pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );*/
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
}

/*******************************************************************************
* �֐����Fvoid CBillboard::SetRenderStateEnd( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�I������
*******************************************************************************/
void CBillboard::SetRenderStateEnd(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �ݒ�����ɖ߂� /////
	// ���C�g��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �A���t�@�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// Z�e�X�g��OFF
	//pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
}

/*******************************************************************************
* �֐����Fvoid CBillboard::SetWorldMatrix( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[���h�}�g���b�N�X�ݒ菈��
*******************************************************************************/
void CBillboard::SetWorldMatrix(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �r���[�}�g���b�N�X�̋t�s������߂�
	/*D3DXMatrixInverse( &m_MtxWorld,
	NULL,
	&mtxView);*/

	// �}�g���N�X�̈ړ��Ɋւ���ϐ��N���A
	m_MtxWorld._41 = 0.0f;
	m_MtxWorld._42 = 0.0f;
	m_MtxWorld._43 = 0.0f;

	D3DXMATRIX matBillboard;
	D3DXVECTOR3 vDir = camera->GetVecRV();

	if (vDir.x >= 0.0f)
	{
		D3DXMatrixRotationY(&matBillboard, -atanf(vDir.z / vDir.x) + D3DX_PI / 2.0f);
	}
	else
	{
		D3DXMatrixRotationY(&matBillboard, -atanf(vDir.z / vDir.x) - D3DX_PI / 2.0f);
	}
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&matBillboard);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);
	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);
}