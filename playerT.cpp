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
#include "mode.h"
#include "game.h"
#include "model.h"
#include "motionManager.h"
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "scene3D.h"
#include "billboard.h"
#include "bullet.h"
#include "effect.h"
#include "animationBoard.h"
#include "smoke.h"
#include "diffusionBullet.h"
#include "shadow.h"
#include "player.h"
#include "playerT.h"
#include "input.h"
#include "scene3D.h"
#include "meshField.h"
#include "camera.h"
#include "stencilShadow.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define HEIGHT					( 50 )			// �e������
#define MOVE_SPEED				( 0.5f )		// �ړ���
#define MOVE_SPEED_VEHICLE		( 1.0f )		// �ړ���
#define JUMP_SPEED				( 10.0f )		// �W�����v��
#define GRAVITY					( 0.5f )		// �d��
#define ROT_SPEED				( 0.05f )		// ��]��
#define ROT_AGREE				( 0.05f )

#define ROT_ATTEN				( 0.1f )		// ��]�ʌ����W��
#define MOVE_ATTEN				( 0.08f )		// �ړ��ʌ����W��

#define SHADOW_RADIUS			( 30 )			// �e�̔��a
#define SHADOW_HEIGHT			( 100 )			// �e�̍���

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCPlayerT::CPlayerT()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CPlayerT::CPlayerT(DRAWORDER DrawOrder, OBJTYPE ObjType) :CPlayer(DrawOrder, ObjType)
{
	m_State = PLAYERSTATE_WAIT;
	m_Mode = PLAYERMODE_HUMAN;
	m_Rot = Vector3(0.0f, D3DX_PI, 0.0f);
	m_bMove = false;
	m_bJump = false;
	m_Shadow = NULL;
	m_Scl = Vector3(2.0f, 2.0f, 2.0f);
}

/*******************************************************************************
* �֐����FCPlayerT::~CPlayerT()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CPlayerT::~CPlayerT()
{

}

/*******************************************************************************
* �֐����Fvoid CPlayerT::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CPlayerT::Init(Vector3 pos)
{
	m_Pos = pos;
	m_Shadow = CStencilShadow::Create(pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_01, &m_MtxWorld);
	m_MotionManager->SetMotion(PLAYERSTATE_WAIT);
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CPlayerT::Uninit(void)
{
	// ���
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	m_Shadow->Uninit();
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CPlayerT::Update(void)
{
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	CMeshField *meshField = game->GetMeshField();

	// �v���C���[�̑���
	Operate();

	// �d�́A�㏸�̉��Z
	m_Move.y -= GRAVITY;

	/* �ړ��ʁA�ʒu�̍X�V */
	// �ړ��ʂ̌���
	m_Move.x += (0.0f - m_Move.x) * MOVE_ATTEN;
	//m_Move.y += (0.0f - m_Move.y) * MOVE_ATTEN;
	m_Move.z += (0.0f - m_Move.z) * MOVE_ATTEN;

	// �ڐG���Ă���n�ʂ̖@�����擾
	Vector3 normal = meshField->GetNormal(&m_Pos);
	// �n�ʂ̌X�����ړ��ʂɉ��Z
	if (normal.x > 0.6f || normal.x < -0.6f)
	{
		m_Move.x += normal.x;
	}
	if (normal.z > 0.6f || normal.z < -0.6f)
	{
		m_Move.z += normal.z;
	}

	if (D3DXVec3Length(&normal) > 0 && m_Mode == PLAYERMODE_VEHICLE)
	{
		// �o�C�N���̓��f�������̌X���ɍ��킹�ăN�H�[�^�j�I���ŉ�]������
		D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

		D3DXVec3Cross(&axis, &Vector3(0.0f, 1.0f, 0.0f), &normal);				// ������Ə��̖@���ŊO�ς����߁A���̃x�N�g�������߂�

		float sita = asinf(D3DXVec3Length(&axis));								// ������p�x�����߂�
		D3DXQuaternionRotationAxis(&quat, &axis, sita);							// �������]������N�H�[�^�j�I�������߂�

		D3DXQuaternionSlerp(&m_Quaternion, &m_Quaternion, &quat, 0.1f);			// ���݂̃N�H�[�^�j�I���ƖړI�̃N�H�[�^�j�I���̕��
	}
	else
	{// �v���C���[�����̊O�ɂ����烊�Z�b�g
		m_Quaternion = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
		//m_bJump = true;
	}

	// �ʒu�X�V
	m_Pos += m_Move;

	// �t�B�[���h�̈ړ����� //
	float width, depth;
	width = meshField->GetWidthAll() * 0.5f;
	depth = meshField->GetDepthAll() * 0.5f;
	// �O�ɏo���牟���Ԃ�
	if (m_Pos.x < -width || m_Pos.x > width)
	{
		m_Pos.x -= m_Move.x;
	}
	if (m_Pos.z < -depth || m_Pos.z > depth)
	{
		m_Pos.z -= m_Move.z;
	}

	// �v���C���[�̍�����n�ʂɍ��킹��
	float field = meshField->GetHeight(m_Pos);
	if (m_Pos.y <= field)
	{
		m_Pos.y = field;
		m_bJump = false;
	}

	/* ��]�p�̍X�V */
	// �ړI�̊p�x�̕␳
	CManager::CheckRot(&m_RotN.y);
	// �����v�Z
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);
	// �������X�V
	m_Rot.y += m_Rad.y * ROT_ATTEN;
	CManager::CheckRot(&m_Rot.y);

	// ���[�h���Ƃ̏���
	UpdateMode();

	/* �e�̈ʒu�X�V */
	m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	/* ��Ԃ̍X�V�A���f */
	UpdateState();
	/* ���[�V�����̍X�V */
	m_MotionManager->Update();
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CPlayerT::Draw(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxRot, mtxTrans, mtxScl;				// �����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

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
	m_MotionManager->Draw();
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::SetState(PLAYERSTATE state)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̏�Ԑݒ菈��
*******************************************************************************/
void CPlayerT::SetState(PLAYERSTATE state)
{
	switch (state)
	{
	case PLAYERSTATE_WAIT:
		m_MotionManager->SetMotion(0);
		break;
	case PLAYERSTATE_WALK:
		m_MotionManager->SetMotion(1);
		break;
	case PLAYERSTATE_ATTACK:
		m_MotionManager->SetMotion(2);
		break;
	case PLAYERSTATE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(3);
		}
		break;
	case PLAYERSTATE_JUMP:
		m_MotionManager->SetMotion(1);
		break;
	case PLAYERSTATE_VEHICLE_WAIT:
		m_MotionManager->SetMotion(4);
		break;
	case PLAYERSTATE_VEHICLE_WALK:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(5);
		}
		break;
	case PLAYERSTATE_VEHICLE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(6);
		}
		break;
	default:
		break;
	}

	m_State = state;
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::SetMode(PLAYERMODE mode)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̃��[�h�ݒ菈��
*******************************************************************************/
void CPlayerT::SetMode(PLAYERMODE mode)
{
	m_Mode = mode;
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::UpdateState(void)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̏�ԍX�V����
*******************************************************************************/
void CPlayerT::UpdateState(void)
{
	if (m_MotionManager->EndMotion())
	{// ���[�V�������I��������
		switch (m_State)
		{
		case PLAYERSTATE_WAIT:

			break;
		case PLAYERSTATE_WALK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_ATTACK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_TRANSFORM:
			SetMode(PLAYERMODE_VEHICLE);
			SetState(PLAYERSTATE_VEHICLE_WAIT);
			break;
		case PLAYERSTATE_VEHICLE_WAIT:

			break;
		case PLAYERSTATE_VEHICLE_WALK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_VEHICLE_TRANSFORM:
			SetMode(PLAYERMODE_HUMAN);
			SetState(PLAYERSTATE_WAIT);
			break;
		default:
			break;
		}
	}

	if (!m_bMove)
	{// �����Ă��Ȃ���Αҋ@��Ԃɂ���
		switch (m_State)
		{
		case PLAYERSTATE_WALK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_VEHICLE_WALK:
			SetState(PLAYERSTATE_VEHICLE_WAIT);
			break;
		default:
			break;
		}
	}
	if (!m_bJump && !m_bMove)
	{// �W�����v���Ă��Ȃ���Αҋ@��Ԃɂ���
		switch (m_State)
		{
		case PLAYERSTATE_JUMP:
			SetState(PLAYERSTATE_DAMP);
			break;
		case PLAYERSTATE_DAMP:
			SetState(PLAYERSTATE_WAIT);
			break;
		default:
			break;
		}
	}
	if (m_State == PLAYERSTATE_ATTACK)
	{
		int nKeyFrame = m_MotionManager->GetKeyInfoId();
		int nFrame = m_MotionManager->GetFrame();
		if (nKeyFrame == 3 && nFrame == 0)
		{
			Vector3 vec;
			vec.x = sinf(m_Rot.y + D3DX_PI);
			vec.y = 0.0f;
			vec.z = cosf(m_Rot.y + D3DX_PI);
			vec.Normalize();

			CModel *model = m_MotionManager->GetModel(26);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));

			model = m_MotionManager->GetModel(27);
			matrix = model->GetMatrix();
			pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::Operate(void)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C�����쏈��
*******************************************************************************/
void CPlayerT::Operate(void)
{
	CInput *input = CManager::GetInput();
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	if (CManager::GetOperateMode() == CManager::OPMODE_PLAYER)
	{
		switch (m_Mode)
		{
		case PLAYERMODE_HUMAN:
		{// �l�^��
		 // ���s�ړ� //
			if (input->GetKeyboardPress(DIK_W) || input->GetKeyboardPress(DIK_D) ||
				input->GetKeyboardPress(DIK_A) || input->GetKeyboardPress(DIK_S))
			{// �ړ��L�[�������Ă�����
				float rad = 0.0f;
				// �O�ړ�
				if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// �΂߉E�O�ړ�
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// �΂ߍ��O�ړ�
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// ���ړ�
				if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// �΂߉E���ړ�
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// �΂ߍ����ړ�
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// �E�ړ�
				if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// ���ړ�
				if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.5f;
					rad = camera->m_Rot.y - D3DX_PI * 0.5f;
				}

				m_Move.x += sinf(rad) * MOVE_SPEED;
				m_Move.z += cosf(rad) * MOVE_SPEED;
				m_bMove = true;

				if (m_State != PLAYERSTATE_ATTACK)
				{
					SetState(PLAYERSTATE_WALK);
				}
			}
			else
			{// �ړ��L�[�������Ă��Ȃ�������
				m_bMove = false;
			}
			// �e����
			if (input->GetKeyboardTrigger(DIK_SPACE) && m_State != PLAYERSTATE_ATTACK)
			{
				SetState(PLAYERSTATE_ATTACK);
				
			}
			
			/* ��]�p�̍X�V */
			m_RotN.y = camera->m_Rot.y + D3DX_PI;	// TPS���_
			break;
		}
		case PLAYERMODE_VEHICLE:
		{// �g���b�N��
		 // ���s�ړ� //
			if (input->GetKeyboardPress(DIK_W) || input->GetKeyboardPress(DIK_D) ||
				input->GetKeyboardPress(DIK_A) || input->GetKeyboardPress(DIK_S))
			{// �ړ��L�[�������Ă�����
				float rad = 0.0f;
				// �O�ړ�
				if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// �΂߉E�O�ړ�
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// �΂ߍ��O�ړ�
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// ���ړ�
				if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// �΂߉E���ړ�
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// �΂ߍ����ړ�
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// �E�ړ�
				if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// ���ړ�
				if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.5f;
					rad = camera->m_Rot.y - D3DX_PI * 0.5f;
				}

				m_Move.x += sinf(rad) * MOVE_SPEED_VEHICLE;
				m_Move.z += cosf(rad) * MOVE_SPEED_VEHICLE;
				m_bMove = true;

				SetState(PLAYERSTATE_VEHICLE_WALK);
			}
			else
			{// �ړ��L�[�������Ă��Ȃ�������
				m_bMove = false;
			}
			// �e����
			if (input->GetKeyboardTrigger(DIK_SPACE))
			{
				CDiffusionBullet::Create(Vector3(m_Pos.x, m_Pos.y + HEIGHT, m_Pos.z), m_Rot);
			}
			break;
		}
		case PLAYERMODE_TRANSFORM:
		{// �ό`��
		 // ���s�ړ� //
			if (input->GetKeyboardPress(DIK_W) || input->GetKeyboardPress(DIK_D) ||
				input->GetKeyboardPress(DIK_A) || input->GetKeyboardPress(DIK_S))
			{// �ړ��L�[�������Ă�����
				float rad = 0.0f;
				// �O�ړ�
				if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// �΂߉E�O�ړ�
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// �΂ߍ��O�ړ�
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// ���ړ�
				if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// �΂߉E���ړ�
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// �΂ߍ����ړ�
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// �E�ړ�
				if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// ���ړ�
				if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.5f;
					rad = camera->m_Rot.y - D3DX_PI * 0.5f;
				}

				m_Move.x += sinf(rad) * MOVE_SPEED;
				m_Move.z += cosf(rad) * MOVE_SPEED;
				m_bMove = true;

			}
			else
			{// �ړ��L�[�������Ă��Ȃ�������
				m_bMove = false;
			}
			break;
		}
		default:
			break;
		}
		// �W�����v
		if (input->GetKeyboardTrigger(DIK_LSHIFT) && m_bJump == false
			&& (m_State == PLAYERSTATE_WAIT || m_State == PLAYERSTATE_WALK))
		{
			m_Move.y = JUMP_SPEED;
			m_bJump = true;
			SetState(PLAYERSTATE_JUMP);
		}

		// �ό`
		if (input->GetKeyboardTrigger(DIK_V))
		{
			switch (m_Mode)
			{// ���[�h���Ƃ̏���
			case PLAYERMODE_HUMAN:
				SetState(PLAYERSTATE_TRANSFORM);
				SetMode(PLAYERMODE_TRANSFORM);
				break;
			case PLAYERMODE_VEHICLE:
				SetState(PLAYERSTATE_VEHICLE_TRANSFORM);
				SetMode(PLAYERMODE_TRANSFORM);
				break;
			default:
				break;
			}
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CPlayerT::UpdateMode(void)
*
* ����	�F
* �߂�l�F
* ����	�F���[�h���Ƃ̏���
*******************************************************************************/
void CPlayerT::UpdateMode(void)
{
	switch (m_Mode)
	{
	case PLAYERMODE_HUMAN:
	{
		break;
	}
	case PLAYERMODE_VEHICLE:
	{
		if (m_bMove == true)
		{
			CModel *model = m_MotionManager->GetModel(30);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42 - 3.0f, matrix->_43);
			CSmoke::Create(pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);

			model = m_MotionManager->GetModel(31);
			matrix = model->GetMatrix();
			pos = Vector3(matrix->_41, matrix->_42 - 3.0f, matrix->_43);
			CSmoke::Create(pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);
		}
		break;
	}
	case PLAYERMODE_TRANSFORM:
	{
		break;
	}
	}
}

/*******************************************************************************
* �֐����FCPlayerT *CPlayerT::Create( Vector3 pos )
*
* ����	�F
* �߂�l�FCPlayerT�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CPlayerT *CPlayerT::Create(Vector3 pos)
{
	CPlayerT *player;
	player = new CPlayerT;
	player->Init(pos);
	return player;
}