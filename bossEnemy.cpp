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
#include "effect.h"
#include "bullet.h"
#include "skyDome.h"
#include "skySphere.h"
#include "barrier.h"
#include "animationBoard.h"
#include "explosion.h"
#include "input.h"
#include "chargeEffect.h"
#include "meshCylinder.h"
#include "lazer.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float  MOVE_SPEED = 1.0f;			// �ړ���
const float  ROT_SPEED = 0.05f;			// ��]��

const float  ROT_ATEEN = 0.1f;			// ��]�ʌ����W��
const float  MOVE_ATEEN = 0.15f;		// �ړ��ʌ����W��

const int LIFE_MAX = 10000;				// �ő僉�C�t
const float SEARCH_LENG = 2000.0f;		// ���G�͈�
const int ATTACK_CNT = 30;				// �U���J�E���^

const float GRAVITY = 0.5f;				// �d��

const float SHADOW_RADIUS = 400.0f;		// �e�̔��a
const float SHADOW_HEIGHT = 100.0f;		// �e�̍���

const int COLLISION_LENGTH = 400;		// �����蔻��
const int DAMAGE_CNT = 60;				// ��e�J�E���^
const int STATE_CHANGE = 150;			// ��ԕω��J�E���^
const int STATE_CHANGE_ATTACK_0 = 300;	// ��ԕω��J�E���^(�U����)
const int STATE_CHANGE_ATTACK_1 = 300;	// ��ԕω��J�E���^(�U����)

const int LIFE_PHASE_1 = 8000;			// ���C�t�i�K1

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
	m_fCollisionLength = COLLISION_LENGTH;
	m_pBarrier = NULL;
	m_pCharge = NULL;
	m_State = STATE_NORMAL;
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
	m_pBarrier = CBarrier::Create(m_TargetPos, 300.0f, 8, 16);
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
	m_Shadow = NULL;
	m_pBarrier->SetDeleteFlg(true);
	m_pBarrier = NULL;

	// �G�t�F�N�g�̐���
	CExplosion::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 500.0f, 500.0f);

	CGame *game = (CGame*)CManager::GetMode();
	game->SetState(CGame::STATE_END);
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
			}
			else
			{
				m_bSearch = false;
				m_nAttCnt = 0;
				SetState(STATE_NORMAL);
			}
		}
		scene = next;
	}
#ifdef _DEBUG
	if (CInput::GetKeyboardTrigger(DIK_B))
	{
		// �ڍs
		m_pCharge = CChargeEffect::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f);
		SetState(STATE_ATTACK_1);
	}
#endif

	// �ړI�̊p�x�̕␳
	CManager::CheckRot(&m_RotN.y);

	// �����v�Z
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);

	// �������X�V
	m_Rot.y += m_Rad.y * ROT_ATEEN;
	CManager::CheckRot(&m_Rot.y);

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

/*******************************************************************************
* �֐����Fvoid CEnemy::UpdateState(void)
*
* ����	�F
* �߂�l�F
* ����	�F��ԍX�V����
*******************************************************************************/
void CBossEnemy::UpdateState(void)
{
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	// ��ԃJ�E���^�̍X�V
	m_nStateCnt++;
	switch (m_State)
	{
	case STATE_NORMAL:
		m_MotionManager->SetMotion(0);
		// �o���A(����)���Z�b�g
		m_pBarrier->Set(m_TargetPos, 0.0f);
		if (m_nStateCnt > STATE_CHANGE)
		{
			if (m_bSearch)
			{// ���G�t���O��true
				SetState(STATE_ATTACK_0);
			}
			else
			{// ���G�t���O��false
				SetState(STATE_GUARD);
			}
		}
		break;
	case STATE_DAMAGE:
		m_MotionManager->SetMotion(0);
		if (m_nStateCnt >= DAMAGE_CNT)
		{
			SetState(STATE_NORMAL);
		}
		break;
	case STATE_ATTACK_0:
		m_MotionManager->SetMotion(1);
		m_nAttCnt++;
		if ((m_nAttCnt % ATTACK_CNT) == 0)
		{
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
		if (m_nStateCnt >= STATE_CHANGE_ATTACK_0)
		{// �K��̎��Ԃ��o�߂�����s���ω�
			if (m_nLife <= LIFE_PHASE_1)
			{// ���C�t�����ȉ���������
				m_pCharge = CChargeEffect::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f);
				SetState(STATE_ATTACK_1);
			}
			else
			{
				// �K�[�h��ԂɈڍs
				SetState(STATE_GUARD);
			}
		}
		break;
	case STATE_ATTACK_1:
		m_MotionManager->SetMotion(1);
		m_nAttCnt++;
		if (m_pCharge)
		{// �`���[�W���Ă�����
			if ((m_nAttCnt % 140) == 0)
			{
				SetWorldMatrix();
				CLazer::Create(m_TargetPos, Vector3(-D3DX_PI * 0.8f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 100.0f, &m_MtxWorld);
				m_pCharge = NULL;
			}
		}
			
		if (m_nStateCnt >= STATE_CHANGE_ATTACK_1)
		{
			// �K�[�h��ԂɈڍs
			SetState(STATE_GUARD);
		}
		break;
	case STATE_GUARD:
		// �K�[�h��
		m_MotionManager->SetMotion(0);
		// �o���A�̃Z�b�g
		m_pBarrier->Set(m_TargetPos, 1.0f);
		if (m_nStateCnt >= STATE_CHANGE)
		{
			SetState(STATE_NORMAL);
		}
		break;
	default:
		break;
	}
}