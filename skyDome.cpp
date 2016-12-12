/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FskyDome.cpp
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
#include "mode.h"
#include "game.h"
#include "renderer.h"
#include "scene.h"
#include "scene3D.h"
#include "skyDome.h"
#include "input.h"
#include "camera.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MESHDOME_ANGLE	( 50.0f )			// �h�[�������̊p�x
#define ROT_SPEED		( 0.01f )			// 

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCSkyDome::CSkyDome()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CSkyDome::CSkyDome( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene3D( DrawOrder, ObjType )
{
	m_Pos = Vector3( 0.0f, 0.0f, 0.0f );
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Trans = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_pTexture = NULL;
	m_VtxBuff = NULL;

	ZeroMemory( &m_MeshFan, sizeof( m_MeshFan ) );
	ZeroMemory( &m_MeshDome, sizeof( m_MeshDome ) );
}

/*******************************************************************************
* �֐����FCSkyDome::~CSkyDome()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CSkyDome::~CSkyDome()
{
}

/*******************************************************************************
* �֐����Fvoid CSkyDome::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CSkyDome::Init( Vector3 pos, float rad, int row, int column )
{
	m_Pos = pos;

	// �h�[������
	m_MeshDome.nRowBlock = row;
	m_MeshDome.nColumnBlock = column;
	m_MeshDome.nRowNumVtx = m_MeshDome.nRowBlock + 1;
	m_MeshDome.nColumnNumVtx =m_MeshDome.nColumnBlock + 1;
	m_MeshDome.fRadius = rad;
	m_MeshDome.nNumVtxMax = m_MeshDome.nRowNumVtx * m_MeshDome.nColumnNumVtx;
	m_MeshDome.nNumIdx = m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 + ( m_MeshDome.nRowBlock - 1 ) * 2;
	m_MeshDome.nNumPrim = ( m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 ) + ( m_MeshDome.nRowBlock - 1 ) * 4;

	// �t�@������
	m_MeshFan.nNumVtxMax = m_MeshDome.nColumnNumVtx;
	m_MeshFan.nNumIdx = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan.nNumPrim = m_MeshDome.nColumnBlock;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_SKY_000);

	// ���_�o�b�t�@�̊m�� /////
	// �h�[������
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * m_MeshDome.nNumVtxMax,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_MeshDome.pVtxBuff,
							NULL ) ) )
	{
		return;
	}
	// �t�@������
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * m_MeshFan.nNumVtxMax,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_MeshFan.pVtxBuff,
							NULL ) ) )
	{
		return;
	}

	// �C���f�b�N�X�o�b�t�@�̊m�� /////
	// �h�[������
	pDevice->CreateIndexBuffer( sizeof( WORD ) * m_MeshDome.nNumIdx,		// �m�ۂ���o�b�t�@�T�C�Y
								D3DUSAGE_WRITEONLY,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&m_MeshDome.pIdxBuff,
								NULL );
	// �t�@������
	pDevice->CreateIndexBuffer( sizeof( WORD ) * m_MeshFan.nNumIdx,		// �m�ۂ���o�b�t�@�T�C�Y
								D3DUSAGE_WRITEONLY,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&m_MeshFan.pIdxBuff,
								NULL );

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;
	// �h�[������
	m_MeshDome.pVtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	for (int nCntRow = m_MeshDome.nRowNumVtx - 1, nIdxCnt = 0; nCntRow >= 0; nCntRow--)
	{
		for( int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			pVtx[ nIdxCnt ].pos.x = cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
									sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ) * m_MeshDome.fRadius;
			pVtx[ nIdxCnt ].pos.y = sinf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) * m_MeshDome.fRadius;
			pVtx[ nIdxCnt ].pos.z = cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
									cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ) * m_MeshDome.fRadius;

			pVtx[ nIdxCnt ].nor = D3DXVECTOR3( cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
												sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ), 
												sinf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ),
												cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
												cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ) );

			pVtx[ nIdxCnt ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

			// �e�N�X�`���u���b�N�\��
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// �e�N�X�`���S�ʕ\��
			pVtx[ nIdxCnt ].tex.x = ( 1.0f / m_MeshDome.nColumnBlock ) * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = ( 1.0f / m_MeshDome.nRowBlock ) * (m_MeshDome.nRowNumVtx - 1 - nCntRow);
		}
	}

	m_MeshDome.pVtxBuff->Unlock();

	// �t�@������
	m_MeshFan.pVtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos.x = 0.0f;
	pVtx[ 0 ].pos.y = m_MeshDome.fRadius;
	pVtx[ 0 ].pos.z = 0.0f;

	pVtx[ 0 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 0 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

	pVtx[ 0 ].tex.x = 0.0f;
	pVtx[ 0 ].tex.y = 0.0f;

	for( int i = 1, nIdxCnt = 0; i < m_MeshFan.nNumVtxMax; i++ )
	{
		pVtx[ i ].pos.x = cosf( MESHDOME_ANGLE * RAD ) *
								sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ) * m_MeshDome.fRadius;

		pVtx[ i ].pos.y = sinf( MESHDOME_ANGLE * RAD ) * m_MeshDome.fRadius;
		pVtx[ i ].pos.z = cosf( MESHDOME_ANGLE * RAD ) *
								cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ) * m_MeshDome.fRadius;

		pVtx[ i ].nor = D3DXVECTOR3( cosf( MESHDOME_ANGLE * RAD ) *
										sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ), 
										sinf( MESHDOME_ANGLE * RAD ),
										cosf( MESHDOME_ANGLE * RAD ) *
										cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ) );
		pVtx[ i ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

		pVtx[ i ].tex.x = 0.0f;
		pVtx[ i ].tex.y = 1.0f;
	}

	m_MeshFan.pVtxBuff->Unlock();


	// �C���f�b�N�X���ݒ� /////
	WORD *pIdx;

	m_MeshDome.pIdxBuff->Lock( 0, 0, ( void** )&pIdx, 0 );

	// �ϐ���`
	int x, y, cnt;

	for( y = 0, cnt = 0; y < m_MeshDome.nRowBlock; y++ )
	{
		for( x = 0; x < m_MeshDome.nColumnNumVtx; x++, cnt++, pIdx += 2 )
		{
			pIdx[ 0 ] = x + ( y + 1 ) * m_MeshDome.nColumnNumVtx;
			pIdx[ 1 ] = x + ( y + 0 ) * m_MeshDome.nColumnNumVtx;
			if( x == m_MeshDome.nColumnNumVtx - 1 && cnt * 2 < m_MeshDome.nNumIdx - 2 )
			{
				cnt++;
				pIdx += 2;
				pIdx[ 0 ] = x + ( y + 0 ) * m_MeshDome.nColumnNumVtx;
				pIdx[ 1 ] = ( y + 1 + 1 ) * m_MeshDome.nColumnNumVtx;
			}
		}
	}

	m_MeshDome.pIdxBuff->Unlock();

	m_MeshFan.pIdxBuff->Lock( 0, 0, ( void** )&pIdx, 0 );

	for( int i = 0; i < m_MeshFan.nNumIdx; i++ )
	{
		pIdx[ i ] = i;
	}
	pIdx[ m_MeshFan.nNumIdx - 1 ] = 1;

	m_MeshFan.pIdxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CSkyDome::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CSkyDome::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_MeshDome.pVtxBuff );
	SAFE_RELEASE( m_MeshDome.pIdxBuff );
	SAFE_RELEASE( m_MeshFan.pVtxBuff );
	SAFE_RELEASE( m_MeshFan.pIdxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CSkyDome::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CSkyDome::Update( void )
{
	// �J������Ǐ]
	CGame *game = ( CGame* )CManager::GetMode();
	CCamera *camera = game->GetCamera();
	m_Pos.x = camera->GetPosition().x;
	m_Pos.z = camera->GetPosition().z;
	m_Rot.y += ROT_SPEED;
}

/*******************************************************************************
* �֐����Fvoid CSkyDome::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CSkyDome::Draw( void )
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e��ݒ� /////
	// ���C�g��OFF
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
	pDevice->SetRenderState( D3DRS_ALPHAREF, 50 );
	
	// �t�H�O��OFF
	pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// �J�����O�̐ݒ�
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

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

	// �f�[�^�X�g���[���Ƀo�C���h����
	pDevice->SetStreamSource( 0, m_MeshDome.pVtxBuff, 0, sizeof( VERTEX_3D ) );
	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices( m_MeshDome.pIdxBuff );

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_3D );

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, *m_pTexture );

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLESTRIP,	// �v���~�e�B�u�̎��
					0,
					0,							
					m_MeshDome.nNumVtxMax,	// ���_��
					0,
					m_MeshDome.nNumPrim );	// �`�悷��v���~�e�B�u��

	// �f�[�^�X�g���[���Ƀo�C���h����
	pDevice->SetStreamSource( 0, m_MeshFan.pVtxBuff, 0, sizeof( VERTEX_3D ) );
	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices( m_MeshFan.pIdxBuff );

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_3D );

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, *m_pTexture );

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLEFAN,		// �v���~�e�B�u�̎��
					0,
					0,							
					m_MeshFan.nNumVtxMax,	// ���_��
					0,
					m_MeshFan.nNumPrim );	// �`�悷��v���~�e�B�u��

	// ���C�g��ON
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	// �A���t�@�e�X�g��OFF
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	// �t�H�O��ON
	pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	// �J�����O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/*******************************************************************************
* �֐����FCSkyDome *CSkyDome::Create( Vector3 pos, float rad, int row, int column )
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CSkyDome *CSkyDome::Create( Vector3 pos, float rad, int row, int column )
{
	CSkyDome *scene3D;
	scene3D = new CSkyDome;
	scene3D->Init( pos, rad, row, column );
	return scene3D;
}