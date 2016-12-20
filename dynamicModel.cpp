/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FdynamicModel.cpp
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
#include "motionManager.h"
#include "model.h"
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "scene3D.h"
#include "meshField.h"
#include "dynamicModel.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 50 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MOVE_SPEED		( 1.0f )		// �ړ���
#define ROT_SPEED		( 0.05f )		// ��]��

#define ROT_ATEEN		( 0.1f )		// ��]�ʌ����W��
#define MOVE_ATEEN		( 0.15f )		// �ړ��ʌ����W��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCDynamicModel::CDynamicModel()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CDynamicModel::CDynamicModel( DRAWORDER DrawOrder, OBJTYPE ObjType ):CSceneX( DrawOrder, ObjType )
{
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Move = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rot = Vector3( 0.0f, 0.0f, 0.0f );
	m_RotN = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rad = Vector3( 0.0f, 0.0f, 0.0f );
	m_Texture = NULL;
	D3DXQuaternionIdentity( &m_Quaternion );
	D3DXQuaternionIdentity( &m_QuaterN );

	m_MotionManager = NULL;
}

/*******************************************************************************
* �֐����FCDynamicModel::~CDynamicModel()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CDynamicModel::~CDynamicModel()
{
}

/*******************************************************************************
* �֐����Fvoid CDynamicModel::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CDynamicModel::Init( Vector3 pos )
{
	m_Pos = pos;
	int num = rand() % 2;
	if (num == 0)
	{
		m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_00, &m_MtxWorld);
	}
	else if (num == 1)
	{
		m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_01, &m_MtxWorld);
	}

	m_MotionManager->SetMotion(0);
}

/*******************************************************************************
* �֐����Fvoid CDynamicModel::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CDynamicModel::Uninit( void )
{
	// ���
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CDynamicModel::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CDynamicModel::Update( void )
{

	m_MotionManager->Update();
}

/*******************************************************************************
* �֐����Fvoid CDynamicModel::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CDynamicModel::Draw( void )
{
	/* �ϐ���` */
	D3DXMATRIX mtxRot, mtxTrans, mtxScl;				// �����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxWorld );

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
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
	m_MotionManager->Draw();
}

/*******************************************************************************
* �֐����FCDynamicModel *CDynamicModel::Create( Vector3 pos )
* 
* ����	�F
* �߂�l�FCDynamicModel�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CDynamicModel *CDynamicModel::Create( Vector3 pos )
{
	CDynamicModel *model;
	model = new CDynamicModel;
	model->Init( pos );
	return model;
}

/*******************************************************************************
* �֐����Fvoid CDynamicModel::SetModelMatrix(void)
*
* ����	�F
* �߂�l�F
* ����	�F���f���̃}�g���N�X�̐ݒ菈��
*******************************************************************************/
void CDynamicModel::SetModelMatrix(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxRot, mtxTrans;				// �����A�|�W�V����

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// �N�H�[�^�j�I���𔽉f
	D3DXMatrixRotationQuaternion(&mtxRot, &m_Quaternion);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	// ���f���̃}�g���N�X�Z�b�g
	CModel *model;
	for (int i = 0; i < CMotionManager::GetDynamicModelInfo(m_MotionManager->GetModelType())->nNumModel; i++)
	{
		model = m_MotionManager->GetModel(i);
		model->SetMatrix();
	}
}