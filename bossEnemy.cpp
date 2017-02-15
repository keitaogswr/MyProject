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
#include "enemySpeed.h"
#include "camera.h"
#include "player.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float  MOVE_SPEED = 1.0f;			// �ړ���
const float  ROT_SPEED = 0.05f;			// ��]��

const float  ROT_ATTEN = 0.1f;			// ��]�ʌ����W��
const float  ROT_ATTEN_1 = 0.03f;		// ��]�ʌ����W��
const float  MOVE_ATTEN = 0.15f;		// �ړ��ʌ����W��

const int LIFE_MAX = 10000;				// �ő僉�C�t
const float SEARCH_LENG = 2000.0f;		// ���G�͈�

const float GRAVITY = 0.5f;				// �d��

const float SHADOW_RADIUS = 400.0f;		// �e�̔��a
const float SHADOW_HEIGHT = 100.0f;		// �e�̍���

const int COLLISION_LENGTH = 400;		// �����蔻��
const int ATTACK_CNT = 10;				// �U���J�E���^
const int DAMAGE_CNT = 60;				// ��e�J�E���^
const int STATE_CHANGE = 150;			// ��ԕω��J�E���^
const int STATE_CHANGE_ATTACK_0 = 300;	// ��ԕω��J�E���^(�U��0��)
const int STATE_CHANGE_ATTACK_1 = 450;	// ��ԕω��J�E���^(�U��1��)

const int LIFE_PHASE_1 = 8000;			// ���C�t�i�K1

const int SUMMON_MAX = 5;				// �����ő吔

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
	CInput *input = CManager::GetInput();
	if (input->GetKeyboardTrigger(DIK_B))
	{
		SetCharge();
		SetState(STATE_ATTACK_1);
	}
	if (input->GetKeyboardTrigger(DIK_N))
	{
		SetState(STATE_SUMMON);
	}
#endif

	// ��]�p�̍X�V
	UpdateRot();

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
				SetCharge();
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
		m_MotionManager->SetMotion(0);
		m_nAttCnt++;
		if (m_nStateCnt == 140)
		{// ���[�U�[�̔���
			SetWorldMatrix();
			SetLazer();
			CGame *game = (CGame*)CManager::GetMode();
			CCamera *camera = game->GetCamera();
			camera->SetShake(10.0f, 150);
		}
			
		if (m_nStateCnt >= STATE_CHANGE_ATTACK_1)
		{
			// ������ԂɈڍs
			SetState(STATE_SUMMON);
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
	case STATE_SUMMON:
		SetSummon();
		SetState(STATE_GUARD);
		break;
	default:
		break;
	}
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::UpdateRot( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBossEnemy::UpdateRot(void)
{
	// �����W���̕���
	float rotAtten = 0.0f;

	if (m_State == STATE_ATTACK_1)
	{
		rotAtten = ROT_ATTEN_1;
	}
	else
	{
		rotAtten = ROT_ATTEN;
	}

	// �ړI�̊p�x�̕␳
	CManager::CheckRot(&m_RotN.y);

	// �����v�Z
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);

	// �������X�V
	m_Rot.y += m_Rad.y * rotAtten;
	CManager::CheckRot(&m_Rot.y);
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::SetCharge( void )
*
* ����	�F
* �߂�l�F
* ����	�F�G�t�F�N�g�ݒ菈��
*******************************************************************************/
void CBossEnemy::SetCharge(void)
{
	// �G�t�F�N�g�̐���
	CChargeEffect::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f, NULL);

	/* ���r */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	CModel *model = m_MotionManager->GetModel(7);
	D3DXMATRIX *matrix = model->GetMatrix();
	Vector3	pos = Vector3(0.0f, 0.0f, 0.0f);
	// �G�t�F�N�g�̐���
	CChargeEffect::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f, matrix);

	/* �E�r */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	model = m_MotionManager->GetModel(8);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, 0.0f);
	// �G�t�F�N�g�̐���
	CChargeEffect::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f, matrix);
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::SetLazer( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[�U�[�ݒ菈��
*******************************************************************************/
void CBossEnemy::SetLazer(void)
{
	// ���[�U�[�̐���
	CLazer::Create(m_TargetPos, Vector3(-D3DX_PI * 0.8f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 80.0f, &m_MtxWorld);

	/* ���r */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	CModel *model = m_MotionManager->GetModel(9);
	D3DXMATRIX *matrix = model->GetMatrix();
	Vector3	pos = Vector3(0.0f, 0.0f, -250.0f);
	// ���[�U�[�̐���
	CLazer::Create(pos, Vector3(-D3DX_PI * 0.5f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 40.0f, matrix);

	/* �E�r */
	// ���f���̃}�g���N�X���󂯎��A���W�ϊ�
	model = m_MotionManager->GetModel(10);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, -250.0f);
	// ���[�U�[�̐���
	CLazer::Create(pos, Vector3(-D3DX_PI * 0.5f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 40.0f, matrix);
}

/*******************************************************************************
* �֐����Fvoid CBossEnemy::SetSummon( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����ݒ菈��
*******************************************************************************/
void CBossEnemy::SetSummon(void)
{
	// �ϐ���`
	int cnt = 0;
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	// �G�l�~�[�̃J�E���g
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_ENEMY)
		{
			cnt++;
		}
		scene = next;
	}
	// �G�l�~�[�̏���
	if (cnt <= 1)
	{
		Vector3 pos;
		D3DXCOLOR col = D3DXCOLOR(0.1f, 0.8f, 0.8f, 0.9f);
		
		D3DXVec3TransformCoord(&pos, &Vector3(500.0f, 250.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);

		D3DXVec3TransformCoord(&pos, &Vector3(-500.0f, 250.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);

		D3DXVec3TransformCoord(&pos, &Vector3(700.0f, 150.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);

		D3DXVec3TransformCoord(&pos, &Vector3(-700.0f, 150.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);
	}
}