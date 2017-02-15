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
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "scene3D.h"
#include "meshField.h"
#include "dynamicModel.h"
#include "shadow.h"
#include "stencilShadow.h"
#include "enemy.h"
#include "billboard.h"
#include "bullet.h"
#include "input.h"
#include "effect.h"
#include "animationBoard.h"
#include "explosion.h"
#include "enemySpeed.h"
#include "player.h"
#include "afterBurner.h"
#include "camera.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define MOVE_SPEED		( 2.0f )		// �ړ���
#define ROT_SPEED		( 0.05f )		// ��]��

#define ROT_ATTEN		( 0.1f )		// ��]�ʌ����W��
#define MOVE_ATTEN		( 0.01f )		// �ړ��ʌ����W��

const float SHADOW_RADIUS = 20.0f;		// �e�̔��a
const float SHADOW_HEIGHT = 1000.0f;	// �e�̍���
const int LIFE_MAX = 100;				// �ő僉�C�t
const int SEARCH_LENG = 1600;			// ���G�͈�
const int ATTACK_CNT = 60;				// �U���J�E���^
const int COLLISION_LENGTH = 10;		// �����蔻��
const int DAMAGE_CNT = 60;				// ��e�J�E���^
const int SEARCH_CNT = 10;				// ���G�J�E���^
const int BURNER_CNT = 15;				// �o�[�i�[�J�E���^

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCEnemySpeed::CEnemySpeed(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CEnemySpeed::CEnemySpeed(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
{
	m_Scl = Vector3(1.5f, 1.5f, 1.5f);
}

/*******************************************************************************
* �֐����FCEnemySpeed::~CEnemy()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CEnemySpeed::~CEnemySpeed()
{
}

/*******************************************************************************
* �֐����Fvoid CEnemySpeed::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CEnemySpeed::Init(Vector3 pos)
{
	m_Pos = pos;
	m_TargetPos = m_Pos;
	m_Shadow = CStencilShadow::Create(m_Pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_ENEMY_01, &m_MtxWorld);
	m_MotionManager->SetMotion(0);
	m_pAfterBurner = CAfterBurner::Create(m_Pos, Vector3(0.0f, 0.0f, 0.0f), &m_MtxWorld);
}

/*******************************************************************************
* �֐����Fvoid CEnemySpeed::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CEnemySpeed::Uninit(void)
{
	// ���
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	m_Shadow->SetDeleteFlg(true);
	// �G�t�F�N�g�̐���
	CExplosion::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 500.0f, 500.0f);
	SAFE_DELETE(m_pAfterBurner);
}

/*******************************************************************************
* �֐����Fvoid CEnemySpeed::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CEnemySpeed::Update(void)
{
	// �ʒu�X�V
	UpdateMove();

	// ���G�X�V
	UpdateSearch();

	// �ړI�̊p�x�̕␳
	CManager::CheckRot(&m_RotN.y);

	// �����v�Z
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);

	// �������X�V
	m_Rot.y += m_Rad.y * ROT_ATTEN;
	CManager::CheckRot(&m_Rot.y);

	// �U���X�V
	UpdateAttack();

	// ��ԑJ��
	UpdateState();

	// �e�̈ʒu�X�V
	m_Shadow->Set(Vector3(m_Pos.x, m_Pos.y, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	// ���[�V�����X�V
	m_MotionManager->Update();

	// �����`�F�b�N
	if (m_nLife <= 0)
	{
		// �폜�t���O
		SetDeleteFlg(true);
		DeleteTarget();
	}
}

/*******************************************************************************
* �֐����FCEnemySpeed *CEnemySpeed::Create(Vector3 pos)
*
* ����	�F
* �߂�l�FCEnemy�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CEnemySpeed *CEnemySpeed::Create(Vector3 pos)
{
	CEnemySpeed *model;
	model = new CEnemySpeed;
	model->Init(pos);
	return model;
}

/*******************************************************************************
* �֐����Fvoid CEnemySpeed::UpdateState(void)
*
* ����	�F
* �߂�l�F
* ����	�F��ԍX�V����
*******************************************************************************/
void CEnemySpeed::UpdateState(void)
{
	m_nStateCnt++;
	switch (m_State)
	{
	case STATE_NORMAL:
		if (m_bSearch)
		{
			SetState(STATE_ATTACK);
		}
		if ((m_nStateCnt % BURNER_CNT) == 0)
		{
			m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, -0.5f, 0.0f));
		}
		break;
	case STATE_MOVE:
		if ((m_nStateCnt % BURNER_CNT) == 0)
		{
			m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
		}
		break;
	case STATE_ATTACK:
		if ((m_nStateCnt % BURNER_CNT) == 0)
		{
			m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
		}
		break;
	case STATE_DAMAGE:
		m_MotionManager->SetModelColorAll(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		if (m_nStateCnt >= DAMAGE_CNT)
		{
			SetState(STATE_NORMAL);
		}
		break;
	default:
		break;
	}
}

/*******************************************************************************
* �֐����Fvoid CEnemySpeed::SetState(int state)
*
* ����	�F
* �߂�l�F
* ����	�F��Ԑݒ菈��
*******************************************************************************/
void CEnemySpeed::SetState(int state)
{
	switch (state)
	{
	case STATE_NORMAL:
		m_MotionManager->SetMotion(0);
		break;
	case STATE_MOVE:
		m_MotionManager->SetMotion(1);
		break;
	case STATE_ATTACK:
		m_MotionManager->SetMotion(2);
		break;
	default:
		break;
	}
	m_State = state;
	m_nStateCnt = 0;
}

/*******************************************************************************
* �֐����Fvoid CEnemySpeed::UpdateMove(void)
*
* ����	�F
* �߂�l�F
* ����	�F�ړ��X�V����
*******************************************************************************/
void CEnemySpeed::UpdateMove(void)
{
	if (m_State == STATE_MOVE || m_State == STATE_ATTACK)
	{
		m_nSearchCnt++;
		if ((m_nSearchCnt % SEARCH_CNT) == 0)
		{
			CGame *game = (CGame*)CManager::GetMode();
			CPlayer *player = game->GetPlayer();
			Vector3 pos = player->GetPosition();
			Vector3 diff = Vector3(pos.x - m_Pos.x, 0.0f, pos.z - m_Pos.z);
			diff.Normalize();
			m_Move += diff * MOVE_SPEED;
		}
	}
	// �ړ��ʂ̌���
	m_Move += (Vector3(0.0f, 0.0f, 0.0f) - m_Move) * MOVE_ATTEN;
	// �ʒu�̍X�V
	m_Pos += m_Move;

	// �G���m�̓����蔻��
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_ENEMY && scene != this)
		{
			Vector3 pos = scene->GetPosition();
			CEnemy *enemy = (CEnemy*)scene;
			float collision = enemy->GetCollisionLength();
			if (CheckCollision(pos, collision))
			{
				m_Pos -= m_Move;
			}
		}
		scene = next;
	}

	// �t�B�[���h�̓����蔻��
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();
	if (mesh->GetHeight(m_Pos) > m_Pos.y)
	{// �������s������A�܂��̓t�B�[���h�̊O�ɏo����A�������̓t�B�[���h�̒��ɓ�������j��
		m_Pos -= m_Move;
	}
	
	// �^�[�Q�b�g�ʒu���X�V
	m_TargetPos = m_Pos;
}

/*******************************************************************************
* �֐����Fbool CEnemySpeed::CheckCollision(Vector3 pos, float collision)
*
* ����	�F
* �߂�l�F
* ����	�F�����蔻�菈��
*******************************************************************************/
bool CEnemySpeed::CheckCollision(Vector3 pos, float collision)
{
	if (m_Pos.x + m_fCollisionLength > pos.x - collision && m_Pos.x - m_fCollisionLength < pos.x + collision &&
		m_Pos.y + m_fCollisionLength > pos.y - collision && m_Pos.y - m_fCollisionLength < pos.y + collision &&
		m_Pos.z + m_fCollisionLength > pos.z - collision && m_Pos.z - m_fCollisionLength < pos.z + collision)
	{
		Vector3 diff = pos - m_Pos;
		float length = diff.Length();
		if (length < (collision + m_fCollisionLength))
		{
			return true;
		}
	}
	
	return false;
}