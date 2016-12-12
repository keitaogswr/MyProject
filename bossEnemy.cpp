/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fenemy.cpp
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
#include "shadow.h"
#include "stencilShadow.h"
#include "enemy.h"
#include "bossEnemy.h"
#include "billboard.h"
#include "bullet.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define MOVE_SPEED		( 1.0f )		// �ړ���
#define ROT_SPEED		( 0.05f )		// ��]��

#define ROT_ATEEN		( 0.1f )		// ��]�ʌ����W��
#define MOVE_ATEEN		( 0.15f )		// �ړ��ʌ����W��

#define LIFE_MAX		( 400 )			// �ő僉�C�t
#define SEARCH_LENG		( 2000 )		// ���G�͈�
#define ATTACK_CNT		( 60 )			// �U���J�E���^

#define GRAVITY			( 0.5f )		// �d��

const float SHADOW_RADIUS = 400.0f;		// �e�̔��a
const float  SHADOW_HEIGHT = 100.0f;	// �e�̍���

const int COLLISION_LENGTH = 400;

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCBossEnemy::CBossEnemy(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CBossEnemy::CBossEnemy(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;
	m_nCollisionLength = COLLISION_LENGTH;
}

/*******************************************************************************
* �֐����FCBossEnemy::~CBossEnemy()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CBossEnemy::~CBossEnemy()
{
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CBossEnemy::Init(Vector3 pos)
{
	m_Pos = pos;
	m_TargetPos = m_Pos + Vector3(0.0f, 200.0f, 0.0f);
	m_Shadow = CStencilShadow::Create(m_Pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_BOSS_00, &m_MtxWorld);
	m_MotionManager->SetMotion(0);
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CBossEnemy::Uninit(void)
{
	// ���
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	// �폜�t���O
	SetDeleteFlg(true);
	m_Shadow->SetDeleteFlg(true);
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBossEnemy::Update(void)
{
	D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();

	m_Move.y -= GRAVITY;

	// �ʒu�X�V
	m_Pos += m_Move;
	// �v���C���[�̍�����n�ʂɍ��킹��
	float field = meshField->GetHeight(m_Pos);
	if (m_Pos.y <= field)
	{
		m_Pos.y = field;
	}
	m_TargetPos = m_Pos + Vector3(0.0f, 250.0f, 0.0f);

	/* ��]�p�̍X�V */
	//m_RotN.y = camera->m_Rot.y + D3DX_PI;	// TPS���_

	// ���G����
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_PLAYER)
		{
			Vector3 pos = scene->GetPosition();
			Vector3 diff = m_Pos - pos;
			float length = diff.Length();
			if (length < SEARCH_LENG)
			{
				m_RotN.y = atan2f(m_Pos.x - pos.x, m_Pos.z - pos.z);

				m_bSearch = true;
				m_MotionManager->SetMotion(1);
			}
			else
			{
				m_bSearch = false;
				m_nAttCnt = 0;
				m_MotionManager->SetMotion(0);
			}
		}
		scene = next;
	}

	// �ړI�̊p�x�̕␳
	CManager::CheckRot(&m_RotN.y);

	// �����v�Z
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);

	// �������X�V
	m_Rot.y += m_Rad.y * ROT_ATEEN;
	CManager::CheckRot(&m_Rot.y);

	if (m_bSearch)
	{
		m_nAttCnt++;
		if (m_nAttCnt > ATTACK_CNT)
		{
			m_nAttCnt = 0;
			scene = CScene::GetList(DRAWORDER_3D);
			next = NULL;
			while (scene != NULL)
			{
				next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
				if (scene->GetObjType() == OBJTYPE_PLAYER)
				{
					// ���f���̃}�g���N�X�v�Z
					SetModelMatrix();
					// �e�̃Z�b�g
					SetBullet(scene);
				}
				scene = next;
			}
		}
	}

	// ��ԑJ��
	UpdateState();
	// �e�̈ʒu�X�V
	m_Shadow->Set(Vector3(m_Pos.x, m_Pos.y, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	// ���[�V�����X�V
	m_MotionManager->Update();

	// �����`�F�b�N
	if (m_nLife <= 0)
	{
		Uninit();
	}
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::Uninit( void )
*
* ����	�F
* �߂�l�FCBossEnemy�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CBossEnemy *CBossEnemy::Create(Vector3 pos)
{
	CBossEnemy *model;
	model = new CBossEnemy;
	model->Init(pos);
	return model;
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::SetBullet(CScene *scene)
*
* ����	�F
* �߂�l�F
* ����	�F�e�ݒ菈��
*******************************************************************************/
void CBossEnemy::SetBullet(CScene *scene)
{
	/* ���r */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	CModel *model = m_MotionManager->GetModel(5);
	D3DXMATRIX *matrix = model->GetMatrix();
	Vector3 pos = Vector3(250.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// �^�[�Q�b�g�ƃ��f���̍��W����x�N�g�����v�Z���A���K��
	Vector3 target = scene->GetPosition();
	Vector3 diff = target - pos;
	diff.Normalize();
	// �e�̐���
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));

	/* �E�r */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	model = m_MotionManager->GetModel(6);
	matrix = model->GetMatrix();
	pos = Vector3(-250.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// �^�[�Q�b�g�ƃ��f���̍��W����x�N�g�����v�Z���A���K��
	target = scene->GetPosition();
	diff = target - pos;
	diff.Normalize();
	// �e�̐���
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));

	/* �������o */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	model = m_MotionManager->GetModel(9);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, -250.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// �^�[�Q�b�g�ƃ��f���̍��W����x�N�g�����v�Z���A���K��
	target = scene->GetPosition();
	diff = target - pos;
	diff.Normalize();
	// �e�̐���
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));

	/* �E�����o */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	model = m_MotionManager->GetModel(10);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, -250.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// �^�[�Q�b�g�ƃ��f���̍��W����x�N�g�����v�Z���A���K��
	target = scene->GetPosition();
	diff = target - pos;
	diff.Normalize();
	// �e�̐���
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
}