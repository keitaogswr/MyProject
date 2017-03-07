/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FsceneX.cpp
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
#include "sceneX.h"
#include "input.h"
#include "scene3D.h"
#include "meshField.h"
#include "camera.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MOVE_SPEED		( 2.0f )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCSceneX::CSceneX()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CSceneX::CSceneX( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene( DrawOrder, ObjType )
{
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Texture = NULL;
	m_fCollision = 0.0f;
}

/*******************************************************************************
* �֐����FCSceneX::~CSceneX()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CSceneX::~CSceneX()
{
}

/*******************************************************************************
* �֐����Fvoid CSceneX::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CSceneX::Init( Vector3 pos )
{
	m_Pos = pos;

	/* �ϐ���` */
	int nNumVertices;	// ���_��
	DWORD sizeFVF;		// ���_�t�H�[�}�b�g�̃T�C�Y
	BYTE *pVtxBuff;		// ���_���ւ̃|�C���^

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// X�t�@�C���̓ǂݍ���
	D3DXLoadMeshFromX(	"data\\MODEL\\boss_TypeFortress\\All.x",		// �ǂݍ��݃t�@�C����
						D3DXMESH_SYSTEMMEM,
						pDevice,
						NULL,
						&m_pBuffMat,		// �}�e���A�����
						NULL,
						&m_numMat,			// �}�e���A���̐�
						&m_pMesh );			// ���b�V�����

	nNumVertices = m_pMesh->GetNumVertices();
	sizeFVF = D3DXGetFVFVertexSize( m_pMesh->GetFVF() );

	// ���_���̎擾
	m_pMesh->LockVertexBuffer( D3DLOCK_READONLY, ( void** )&pVtxBuff );
}

/*******************************************************************************
* �֐����Fvoid CSceneX::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CSceneX::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_pMesh );
	SAFE_RELEASE( m_pBuffMat );
}

/*******************************************************************************
* �֐����Fvoid CSceneX::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CSceneX::Update( void )
{

}

/*******************************************************************************
* �֐����Fvoid CSceneX::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CSceneX::Draw( void )
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;		// �X�P�[���A�����A�|�W�V����

	D3DXMATERIAL	*pMat;						// �}�e���A���|�C���^
	D3DMATERIAL9	matDef;						// �f�t�H���g�̃}�e���A��

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	SetWorldMatrix();
	
	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform( D3DTS_WORLD, &m_MtxWorld );

	pDevice->GetMaterial( &matDef );							// ���݂̃}�e���A�����擾

	pMat = ( D3DXMATERIAL* )m_pBuffMat->GetBufferPointer();

	for( int nCntMat = 0; nCntMat < ( int )m_numMat; nCntMat++ )
	{
		// �}�e���A���̐ݒ�
		pDevice->SetMaterial( &pMat[ nCntMat ].MatD3D );

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture( 0, m_Texture );

		// ���b�V���̕`��
		m_pMesh->DrawSubset( nCntMat );
	}
	// �}�e���A�������ɖ߂�
	pDevice->SetMaterial( &matDef );
}

/*******************************************************************************
* �֐����Fvoid CSceneX::Uninit( void )
* 
* ����	�F
* �߂�l�FCSceneX�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CSceneX *CSceneX::Create( Vector3 pos )
{
	CSceneX *sceneX;
	sceneX = new CSceneX;
	sceneX->Init( pos );
	return sceneX;
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::SetWorldMatrix( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[���h�}�g���b�N�X�ݒ菈��
*******************************************************************************/
void CSceneX::SetWorldMatrix(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl,
		m_Scl.x,
		m_Scl.y,
		m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_Rot.y,
		m_Rot.x,
		m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans,
		m_Pos.x,
		m_Pos.y,
		m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);
}