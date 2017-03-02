/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fplayer.cpp
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
#include "scene2D.h"
#include "scene3D.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "scene3D.h"
#include "billboard.h"
#include "bullet.h"
#include "diffusionBullet.h"
#include "shadow.h"
#include "effect.h"
#include "animationBoard.h"
#include "smoke.h"
#include "player.h"
#include "enemy.h"
#include "input.h"
#include "meshField.h"
#include "camera.h"
#include "orbit.h"
#include "reticle.h"
#include "stencilShadow.h"
#include "playerLifeGauge.h"
#include "explosion.h"
#include "sound.h"
#include "afterBurner.h"
#include "missile.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define HEIGHT					( 50 )			// �e������
#define MOVE_SPEED				( 0.8f )		// �ړ���
#define MOVE_SPEED_VEHICLE		( 1.2f )		// �ړ���
#define JUMP_SPEED				( 13.0f )		// �W�����v��
#define GRAVITY					( 0.5f )		// �d��
#define ROT_SPEED				( 0.05f )		// ��]��
#define ROT_AGREE				( 0.05f )

#define ROT_ATTEN				( 0.3f )		// ��]�ʌ����W��
#define MOVE_ATTEN				( 0.08f )		// �ړ��ʌ����W��

#define SEARCH_LENG				( 1500 )		// ���b�N�I������

#define DISTANCE_R				( 1000.0f )		// �J���������_�̏�������

#define SHADOW_RADIUS			( 20 )			// �e�̔��a
#define SHADOW_RADIUS_TRANSFORM	( 17 )			// �e�̔��a
#define SHADOW_RADIUS_VEHICLE	( 15 )			// �e�̔��a
#define SHADOW_HEIGHT			( 100 )			// �e�̍���

const int LIFE_MAX = 1000;
const int EXPLOSION_CNT = 30;
const int KEEP_MAX = 30;

const float COLLISION = 40.0f;

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCPlayer::CPlayer()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CPlayer::CPlayer(DRAWORDER DrawOrder, OBJTYPE ObjType) :CDynamicModel(DrawOrder, ObjType)
{
	m_State = PLAYERSTATE_WAIT;
	m_Mode = PLAYERMODE_HUMAN;
	m_Rot = Vector3(0.0f, D3DX_PI, 0.0f);
	m_RotN = Vector3(0.0f, D3DX_PI, 0.0f);
	m_bMove = false;
	m_bJump = false;
	m_bRockOn = false;
	m_bBullet = false;
	m_Shadow = NULL;
	m_Orbit[0] = m_Orbit[1] = NULL;
	m_nLife = LIFE_MAX;
	m_nTarget = -1;
	m_nExplosionCnt = 0;
	m_nKeep = KEEP_MAX;
	m_fCollision = COLLISION;
}

/*******************************************************************************
* �֐����FCPlayer::~CPlayer()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CPlayer::~CPlayer()
{

}

/*******************************************************************************
* �֐����Fvoid CPlayer::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CPlayer::Init(Vector3 pos)
{
	m_Pos = pos;
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_00, &m_MtxWorld);
	m_MotionManager->SetMotion(PLAYERSTATE_WAIT);
	m_Shadow = CStencilShadow::Create(pos);

	m_Orbit[0] = COrbit::Create();
	m_Orbit[1] = COrbit::Create();
}

/*******************************************************************************
* �֐����Fvoid CPlayer::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CPlayer::Uninit(void)
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
* �֐����Fvoid CPlayer::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CPlayer::Update(void)
{
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	CMeshField *meshField = game->GetMeshField();
	if (meshField == NULL)
	{
		return;
	}

	if (game->GetState() != CGame::STATE_START && game->GetState() != CGame::STATE_END)
	{
		// �v���C���[�̑���
		Operate();
	}

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

	// ���b�N�I���̍X�V
	UpdateRockOn();

	// �ʒu�X�V
	m_Pos += m_Move;

	// �t�B�[���h�̈ړ����� //
	float width, depth;
	width = meshField->GetWidthAll() * 0.5f - 1;
	depth = meshField->GetDepthAll() * 0.5f - 1;
	// �O�ɏo���牟���Ԃ�
	if (m_Pos.x <= -width || m_Pos.x >= width)
	{
		m_Pos.x -= m_Move.x;
	}
	if (m_Pos.z <= -depth || m_Pos.z >= depth)
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

	if (m_bRockOn == true && m_bBullet == true && m_Mode == PLAYERMODE_HUMAN)
	{// �^�[�Q�b�g���Ă���Ƃ�x����G�Ɍ�������
		CEnemy *target = CEnemy::Get(m_nTarget);
		if (target)
		{
			Vector3 pos = target->GetTargetPos();
			Vector2 vec2 = Vector2(pos.z - m_Pos.z, pos.x - m_Pos.x);
			m_RotN.x = atan2f(pos.y - m_Pos.y, vec2.Length());
			CManager::CheckRot(&m_RotN.x);
		}
	}
	else
	{// �^�[�Q�b�g���Ă��Ȃ��Ƃ��͐��ʂ���������
		m_RotN.x = 0.0f;
	}

	/* ��]�p�̍X�V */
	// �ړI�̊p�x�̕␳
	CManager::CheckRot(&m_RotN.x);
	CManager::CheckRot(&m_RotN.y);
	// �����v�Z
	m_Rad.x = m_RotN.x - m_Rot.x;
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad);
	// �������X�V
	m_Rot.x += m_Rad.x * ROT_ATTEN;
	m_Rot.y += m_Rad.y * ROT_ATTEN;
	CManager::CheckRot(&m_Rot);

	/* ������ϋv�l�̍X�V */
	UpdateKeep();

	// ���[�h���Ƃ̏���
	UpdateMode();

	/* ��Ԃ̍X�V�A���f */
	UpdateState();
	/* ���[�V�����̍X�V */
	m_MotionManager->Update();
	/* �I�[�r�b�g�̍X�V */
	SetOrbit();
	/* ���C�t�o�[�̍X�V */
	game->GetPlayerLifeGauge()->SetLife((float)m_nLife, (float)LIFE_MAX);
	if (m_nLife <= 0)
	{
		if (m_nExplosionCnt == 0)
		{
			game->SetState(CGame::STATE_END);
		}
		m_nLife = 0;
		m_nExplosionCnt++;
		if (m_nExplosionCnt % EXPLOSION_CNT == 0)
		{
			// ���f���̍��W���󂯎��A�����G�t�F�N�g�𐶐�
			CModel *model = m_MotionManager->GetModel(0);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			CExplosion::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 100.0f, 100.0f);
			CSmoke::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 70.0f, 70.0f);
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CPlayer::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CPlayer::Draw(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxRot, mtxTrans;				// �����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
	m_MotionManager->Draw();
}

/*******************************************************************************
* �֐����Fvoid CPlayer::Load( void )
*
* ����	�F
* �߂�l�F
* ����	�F�ǂݍ��ݏ���
*******************************************************************************/
void CPlayer::Load(void)
{
}

/*******************************************************************************
* �֐����Fvoid CPlayer::Unload( void )
*
* ����	�F
* �߂�l�F
* ����	�F�������
*******************************************************************************/
void CPlayer::Unload(void)
{
}

/*******************************************************************************
* �֐����Fvoid CPlayer::SetState(PLAYERSTATE state)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̏�Ԑݒ菈��
*******************************************************************************/
void CPlayer::SetState(PLAYERSTATE state)
{
	CInput *input = CManager::GetInput();
	if (input->GetJoyStickConnected())
	{
		SetStateJoyStick(state);
		return;
	}
	switch (state)
	{
	case PLAYERSTATE_WAIT:
		m_MotionManager->SetMotion(0);
		break;
	case PLAYERSTATE_WALK:
		// �O�ړ�
		if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
		{
			m_MotionManager->SetMotion(1);
		}
		// �΂߉E�O�ړ�
		else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
		{
			m_MotionManager->SetMotion(1);
		}
		// �΂ߍ��O�ړ�
		else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
		{
			m_MotionManager->SetMotion(1);
		}
		// ���ړ�
		if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
		{
			m_MotionManager->SetMotion(6);
		}
		// �΂߉E���ړ�
		else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
		{
			m_MotionManager->SetMotion(6);
		}
		// �΂ߍ����ړ�
		else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
		{
			m_MotionManager->SetMotion(6);
		}
		// �E�ړ�
		if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
		{
			m_MotionManager->SetMotion(4);
		}
		// ���ړ�
		if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
		{
			m_MotionManager->SetMotion(5);
		}
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
		m_MotionManager->SetMotion(10);
		break;
	case PLAYERSTATE_BOMBED:
		m_MotionManager->SetMotion(11);
		break;
	case PLAYERSTATE_VEHICLE_WAIT:
		m_MotionManager->SetMotion(9);
		break;
	case PLAYERSTATE_VEHICLE_WALK:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(7);
		}
		break;
	case PLAYERSTATE_VEHICLE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(8);
		}
		break;
	default:
		break;
	}

	m_State = state;
}

/*******************************************************************************
* �֐����Fvoid CPlayer::SetState(PLAYERSTATE state)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̏�Ԑݒ菈��
*******************************************************************************/
void CPlayer::SetStateJoyStick(PLAYERSTATE state)
{
	CInput *input = CManager::GetInput();
	XINPUT_STATE *stateX = input->GetPressState();
	switch (state)
	{
	case PLAYERSTATE_WAIT:
		m_MotionManager->SetMotion(0);
		break;
	case PLAYERSTATE_WALK:
		// �O�ړ�
		if (stateX->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(1);
		}
		// �΂߉E�O�ړ�
		else if (stateX->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(1);
		}
		// �΂ߍ��O�ړ�
		else if (stateX->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(1);
		}
		// ���ړ�
		if (stateX->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(6);
		}
		// �΂߉E���ړ�
		else if (stateX->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(6);
		}
		// �΂ߍ����ړ�
		else if (stateX->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(6);
		}
		// �E�ړ�
		if (stateX->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(4);
		}
		// ���ړ�
		if (stateX->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			stateX->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_MotionManager->SetMotion(5);
		}
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
		m_MotionManager->SetMotion(10);
		break;
	case PLAYERSTATE_BOMBED:
		m_MotionManager->SetMotion(11);
		break;
	case PLAYERSTATE_VEHICLE_WAIT:
		m_MotionManager->SetMotion(9);
		break;
	case PLAYERSTATE_VEHICLE_WALK:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(7);
		}
		break;
	case PLAYERSTATE_VEHICLE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(8);
		}
		break;
	default:
		break;
	}

	m_State = state;
}

/*******************************************************************************
* �֐����Fvoid CPlayer::SetMode(PLAYERMODE mode)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̃��[�h�ݒ菈��
*******************************************************************************/
void CPlayer::SetMode(PLAYERMODE mode)
{
	m_Mode = mode;
}

/*******************************************************************************
* �֐����Fvoid CPlayer::UpdateState(void)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�̏�ԍX�V����
*******************************************************************************/
void CPlayer::UpdateState(void)
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
			m_bBullet = false;
			break;
		case PLAYERSTATE_TRANSFORM:
			SetMode(PLAYERMODE_VEHICLE);
			SetState(PLAYERSTATE_VEHICLE_WAIT);
			break;
		case PLAYERSTATE_BOMBED:
			SetState(PLAYERSTATE_WAIT);
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
		if (nKeyFrame == 2 && nFrame == 0)
		{// ���[�V�����̎w��t���[���ɒB������
			Vector3 vec;

			// ���f���̍��W���󂯎��A�r����e�𔭎˂���
			CModel *model = m_MotionManager->GetModel(8);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			m_bRockOn ? vec = CEnemy::Get(m_nTarget)->GetTargetPos() - pos : vec = Vector3(sinf(D3DX_PI + m_Rot.y), 0.0f, cosf(D3DX_PI + m_Rot.y));
			vec.Normalize();
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));

			model = m_MotionManager->GetModel(7);
			matrix = model->GetMatrix();
			pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			m_bRockOn ? vec = CEnemy::Get(m_nTarget)->GetTargetPos() - pos : vec = Vector3(sinf(D3DX_PI + m_Rot.y), 0.0f, cosf(D3DX_PI + m_Rot.y));
			vec.Normalize();
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));

			CSound::Play(CSound::SOUND_LABEL_SE_SHOT_000);
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CPlayer::Operate(void)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C�����쏈��
*******************************************************************************/
void CPlayer::Operate(void)
{
	CInput *input = CManager::GetInput();
	/*if (input->GetJoyStickConnected())
	{
		OperateJoyStick();
		return;
	}*/
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
				m_bBullet = true;
			}
			/* ��]�p�̍X�V */
			m_RotN.y = camera->m_Rot.y + D3DX_PI;	// TPS���_
			break;
		}
		case PLAYERMODE_VEHICLE:
		{// �o�C�N��
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
				if (CEnemy::Get(m_nTarget))
				{
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y + D3DX_PI * 0.5f), 1.0f, cosf(m_Rot.y + D3DX_PI * 0.5f)), m_nTarget);
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y - D3DX_PI * 0.5f), 1.0f, cosf(m_Rot.y - D3DX_PI * 0.5f)), m_nTarget);
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y + D3DX_PI * 0.25f), 0.3f, cosf(m_Rot.y + D3DX_PI * 0.25f)), m_nTarget);
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y - D3DX_PI * 0.25f), 0.3f, cosf(m_Rot.y - D3DX_PI * 0.25f)), m_nTarget);
					m_bBullet = true;
				}
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
		if (input->GetKeyboardTrigger(DIK_UP) && m_bJump == false
			&& (m_State == PLAYERSTATE_WAIT || m_State == PLAYERSTATE_WALK))
		{
			m_Move.y = JUMP_SPEED;
			m_bJump = true;
			SetState(PLAYERSTATE_JUMP);
		}

		// �ό`
		if (input->GetKeyboardTrigger(DIK_DOWN))
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
* �֐����Fvoid CPlayer::OperateJoyStick(void)
*
* ����	�F
* �߂�l�F
* ����	�F�v���C�����쏈��
*******************************************************************************/
void CPlayer::OperateJoyStick(void)
{
	CInput *input = CManager::GetInput();

	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();

	XINPUT_STATE *state = input->GetPressState();
	if (CManager::GetOperateMode() == CManager::OPMODE_PLAYER)
	{
		switch (m_Mode)
		{
		case PLAYERMODE_HUMAN:
		{// �l�^��
		 // ���s�ړ� //
			if (state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
				state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{// �ړ��L�[�������Ă�����
				float rad = 0.0f;
				// �O�ړ�
				if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// �΂߉E�O�ړ�
				else if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// �΂ߍ��O�ړ�
				else if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// ���ړ�
				if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// �΂߉E���ړ�
				else if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// �΂ߍ����ړ�
				else if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// �E�ړ�
				if (state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// ���ړ�
				if (state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
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
			if (input->TriggerJoyStick(XINPUT_GAMEPAD_B) && m_State != PLAYERSTATE_ATTACK)
			{
				SetState(PLAYERSTATE_ATTACK);
				m_bBullet = true;
			}
			/* ��]�p�̍X�V */
			m_RotN.y = camera->m_Rot.y + D3DX_PI;	// TPS���_
			break;
		}
		case PLAYERMODE_VEHICLE:
		{// �o�C�N��
		 // ���s�ړ� //
			if (state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
				state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{// �ړ��L�[�������Ă�����
				float rad = 0.0f;
				// �O�ړ�
				if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// �΂߉E�O�ړ�
				else if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// �΂ߍ��O�ړ�
				else if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// ���ړ�
				if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// �΂߉E���ړ�
				else if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// �΂ߍ����ړ�
				else if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// �E�ړ�
				if (state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// ���ړ�
				if (state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
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
			if (input->TriggerJoyStick(XINPUT_GAMEPAD_B))
			{
				if (CEnemy::Get(m_nTarget))
				{
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y + D3DX_PI * 0.5f), 1.0f, cosf(m_Rot.y + D3DX_PI * 0.5f)), m_nTarget);
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y - D3DX_PI * 0.5f), 1.0f, cosf(m_Rot.y - D3DX_PI * 0.5f)), m_nTarget);
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y + D3DX_PI * 0.25f), 0.3f, cosf(m_Rot.y + D3DX_PI * 0.25f)), m_nTarget);
					CMissile::Create(m_Pos, Vector3(sinf(m_Rot.y - D3DX_PI * 0.25f), 0.3f, cosf(m_Rot.y - D3DX_PI * 0.25f)), m_nTarget);
					m_bBullet = true;
				}
			}
			break;
		}
		case PLAYERMODE_TRANSFORM:
		{// �ό`��
		 // ���s�ړ� //
			if (state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
				state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{// �ړ��L�[�������Ă�����
				float rad = 0.0f;
				// �O�ړ�
				if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// �΂߉E�O�ړ�
				else if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// �΂ߍ��O�ړ�
				else if (state->Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// ���ړ�
				if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// �΂߉E���ړ�
				else if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// �΂ߍ����ړ�
				else if (state->Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// �E�ړ�
				if (state->Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// ���ړ�
				if (state->Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state->Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
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
		if (input->TriggerJoyStick(XINPUT_GAMEPAD_A) && m_bJump == false
			&& (m_State == PLAYERSTATE_WAIT || m_State == PLAYERSTATE_WALK))
		{
			m_Move.y = JUMP_SPEED;
			m_bJump = true;
			SetState(PLAYERSTATE_JUMP);
		}

		// �ό`
		if (input->TriggerJoyStick(XINPUT_GAMEPAD_X))
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
* �֐����Fvoid CPlayer::UpdateMode(void)
*
* ����	�F
* �߂�l�F
* ����	�F���[�h���Ƃ̏���
*******************************************************************************/
void CPlayer::UpdateMode(void)
{
	CGame *game = dynamic_cast<CGame*>(CManager::GetMode());
	CCamera *camera = game->GetCamera();
	CMeshField *meshField = game->GetMeshField();
	float field = meshField->GetHeight(m_Pos);
	switch (m_Mode)
	{
	case PLAYERMODE_HUMAN:
	{
		/* �e�̈ʒu�X�V */
		m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);
		if (m_bMove == true)
		{
			if (m_bJump == false)
			{
				CSmoke::Create(m_Pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);
			}
		}
		break;
	}
	case PLAYERMODE_VEHICLE:
	{
		/* �e�̈ʒu�X�V */
		m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS_VEHICLE, SHADOW_HEIGHT);
		if (m_bMove == true)
		{
			CModel *model = m_MotionManager->GetModel(28);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42 - 3.0f, matrix->_43);
			CSmoke::Create(pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);
		}
		break;
	}
	case PLAYERMODE_TRANSFORM:
	{
		/* �e�̈ʒu�X�V */
		m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS_TRANSFORM, SHADOW_HEIGHT);
		break;
	}
	}
}

/*******************************************************************************
* �֐����Fvoid CPlayer::SetOrbit(void)
*
* ����	�F
* �߂�l�F
* ����	�F�I�[�r�b�g�̐ݒ菈��
*******************************************************************************/
void CPlayer::SetOrbit(void)
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

	/* �I�[�r�b�g�̍X�V */
	// ���f���̃}�g���N�X���擾
	model = m_MotionManager->GetModel(33);
	D3DXMATRIX *matrix = model->GetMatrix();
	m_Orbit[0]->Set(Vector3(-1.0f, 2.0f, -1.0f), Vector3(-16.0f, -30.0f, -1.0f), *matrix);
	model = m_MotionManager->GetModel(34);
	matrix = model->GetMatrix();
	m_Orbit[1]->Set(Vector3(1.0f, 2.0f, -1.0f), Vector3(16.0f, -30.0f, -1.0f), *matrix);
}

/*******************************************************************************
* �֐����Fvoid CPlayer::UpdateRockOn(void)
*
* ����	�F
* �߂�l�F
* ����	�F���b�N�I������
*******************************************************************************/
void CPlayer::UpdateRockOn(void)
{
	CInput *input = CManager::GetInput();
	CCamera *camera = ((CGame*)CManager::GetMode())->GetCamera();
	Vector3 vec;
	float nearLeng = SEARCH_LENG * SEARCH_LENG;
	float length;

	/* �G�̍��G */
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	int id = -1;
	// �^�[�Q�b�g���Ă��Ȃ����
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_ENEMY && !scene->GetDeleteFlg())
		{
			vec = scene->GetPosition();
			vec = m_Pos - vec;
			length = vec.LengthSq();
			if (length < SEARCH_LENG * SEARCH_LENG && length < nearLeng)
			{// ��ԋ߂��G���X�V
				nearLeng = length;
				id = dynamic_cast<CEnemy*>(scene)->GetId();
			}
		}
		scene = next;
	}
	if (m_nTarget < 0 && m_bRockOn == true && id >= 0)
	{
		m_nTarget = id;
		// �^�[�Q�b�g���Ă���G�Ƀt���O��n��
		CEnemy::Get(m_nTarget)->SetTarget(true);
	}

	if (input->GetKeyboardTrigger(DIK_R) /*|| input->TriggerJoyStick(XINPUT_GAMEPAD_Y)*/)
	{// ���b�N�I���̐؂�ւ�
		m_bRockOn = m_bRockOn == true ? false : true;

		if (m_bRockOn)
		{
			if (m_nTarget >= 0)
			{// �^�[�Q�b�g���Ă���G�Ƀt���O��n��
				CEnemy::Get(m_nTarget)->SetTarget(false);
			}
			m_nTarget = id;
			if (m_nTarget >= 0)
			{// �^�[�Q�b�g���Ă���G�Ƀt���O��n��
				CEnemy::Get(m_nTarget)->SetTarget(true);
			}
			else
			{
				m_bRockOn = false;
			}
		}
		else
		{
			m_nTarget = -1;
		}
	}
	if (m_nTarget < 0 && id < 0)
	{
		// ���b�N�I���t���O���I�t
		m_bRockOn = false;
		// �J������Ǐ]���[�h�ɕύX
		camera->SetCameraMode(CAMERAMODE_SNEAK);
		// �Ə���ʏ탂�[�h�ɂ���
		CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
		reticle->SetRockOn(false);
	}

	if (m_bRockOn)
	{// ���b�N�I���t���O��true�̂Ƃ�
		// �J���������b�N�I�����[�h�ɕύX
		camera->SetCameraMode(CAMERAMODE_ROCKON);
		CEnemy *enemy = CEnemy::Get(m_nTarget);
		vec = enemy->GetPosition();
		vec = m_Pos - vec;
		length = vec.LengthSq();
		if (length > SEARCH_LENG * SEARCH_LENG)
		{
			m_bRockOn = false;
		}
		if (m_nTarget >= 0)
		{// �G���͈͓��ɂ�����
			// �Ə������b�N�I�����[�h�ɂ���
			CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
			reticle->SetRockOn(true);
		}
		else if (m_nTarget < 0)
		{// �G���͈͓��ɂ��Ȃ�������
			// ���b�N�I���t���O���I�t
			m_bRockOn = false;
			// �J������Ǐ]���[�h�ɕύX
			camera->SetCameraMode(CAMERAMODE_SNEAK);
			// �Ə���ʏ탂�[�h�ɂ���
			CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
			reticle->SetRockOn(false);
		}
	}
	else
	{// ���b�N�I���t���O��false�̂Ƃ�
		// �J������Ǐ]���[�h�ɕύX
		camera->SetCameraMode(CAMERAMODE_SNEAK);
		// �Ə���ʏ탂�[�h�ɂ���
		CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
		reticle->SetRockOn(false);
	}
}

/*******************************************************************************
* �֐����Fvoid CPlayer::UpdateKeep(void)
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����ϋv�l�X�V����
*******************************************************************************/
void CPlayer::UpdateKeep(void)
{
	if (m_Mode == PLAYERMODE_HUMAN)
	{
		if (m_nKeep <= 0)
		{
			SetState(PLAYERSTATE_BOMBED);
			m_nKeep = KEEP_MAX;
			m_bBullet = false;
		}
	}
}

/*******************************************************************************
* �֐����FCPlayer *CPlayer::Create( Vector3 pos )
*
* ����	�F
* �߂�l�FCPlayer�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CPlayer *CPlayer::Create(Vector3 pos)
{
	CPlayer *player;
	player = new CPlayer;
	player->Init(pos);
	return player;
}