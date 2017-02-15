/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fcamera.cpp
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
#include "scene.h"
#include "camera.h"
#include "input.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "player.h"
#include "enemy.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define NEARZ		( 5.0f )				// Near�l
#define FARZ		( 10000.0f )			// Far�l
#define ANGLE		( 40.0f )				// �J�����̎���p
#define MOVE_SPEED	( 3.0f )				// �J�����̎������x
#define ROT_SPEED	( 0.05f )				// �J�����̎��]���x
#define DISTANCE_R	( 1000.0f )				// �J���������_�̏�������
#define DISTANCE_V	( 220.0f )				// �J�������_�Ə�������
#define HEIGHT		( 50.0f )				// �J�������_�̏�������

#define ROT_ATTEN			( 0.1f )		// �J�����̉�]�����W��
#define MOVE_ATTEN_V		( 0.05f )		// ���_�̑��x�����W��
#define MOVE_ATTEN_R		( 0.05f )		// �����_�̑��x�����W��
#define MOVE_COEFF_V		( 15.0f )		// ���_�ړ������̔{���W��
#define MOVE_COEFF_R		( 8.0f )		// �����_�ړ������̔{���W��
#define ROT_ATTEN_ROCKON	( 0.5f )		// �J�����̉�]�����W��(���b�N�I�����[�h��)
#define MOVE_ATTEN_R_ROCKON	( 0.4f )		// �����_�̑��x�����W��(���b�N�I�����[�h��)

#define TPS_ANGLE			( 0.3f )		// TPS���[�h���̃J�����̈ʒu�p�x
#define ROT_TPS_ATTEN		( 0.1f )		// �J�����̉�]�����W��(TPS)
//#define MOVE_TPS_ATTEN_V	( 0.15f )		// ���_�̑��x�����W��(TPS)
//#define MOVE_TPS_ATTEN_R	( 0.15f )		// �����_�̑��x�����W��(TPS)
//#define MOVE_TPS_COEFF_V	( 8.0f )		// ���_�ړ������̔{���W��(TPS)
//#define MOVE_TPS_COEFF_R	( 4.0f )		// �����_�ړ������̔{���W��(TPS)

#define SHAKE_CNT			( 10 )			// ��ʗh�ꎞ��
#define AMPLITUDE			( 30.0f )		// ��ʂ̗h�ꕝ

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCCamera::CCamera()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CCamera::CCamera()
{
	m_Pos = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rot = Vector3( 0.0f, 0.0f, 0.0f );
	m_PosR = Vector3( 0.0f, 0.0f, 0.0f );
	m_VecU = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rad = Vector3( 0.0f, 0.0f, 0.0f );
	m_RotN = Vector3( 0.0f, 0.0f, 0.0f );
	m_bShake = false;
	m_nCnt = 0;
	m_nMode = CAMERAMODE_SNEAK;

	m_MoveV = Vector3(0.0f, 0.0f, 0.0f);
	m_MoveR = Vector3(0.0f, 0.0f, 0.0f);
	m_fAmplitude = AMPLITUDE;
	m_nShakeTime = SHAKE_CNT;
}

/*******************************************************************************
* �֐����FCCamera::~CCamera()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CCamera::~CCamera()
{
}

/*******************************************************************************
* �֐����Fvoid CCamera::Init( void )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CCamera::Init( void )
{
	// �J���� ///
	m_Pos = Vector3( 0.0f, HEIGHT, -DISTANCE_V );
	m_PosR = Vector3( 0.0f, 0.0f, 0.0f );
	m_VecU = Vector3( 0.0f, 1.0f, 0.0f );

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxProjection );

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH( &m_MtxProjection,
								ANGLE * ( D3DX_PI / 180 ),
								( float )SCREEN_WIDTH / SCREEN_HEIGHT,
								NEARZ,
								FARZ );

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform( D3DTS_PROJECTION, &m_MtxProjection );

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxView );
	D3DXMatrixLookAtLH( &m_MtxView,
						&m_Pos,
						&m_PosR,
						&m_VecU );
	pDevice->SetTransform( D3DTS_VIEW, &m_MtxView );
}

/*******************************************************************************
* �֐����Fvoid CCamera::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CCamera::Uninit( void )
{

}

/*******************************************************************************
* �֐����Fvoid CCamera::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CCamera::Update( void )
{
	// ���f���\���̂̎擾
	CGame *game = ( CGame* )CManager::GetMode();
	CPlayer *player = game->GetPlayer();
	CInput *input = CManager::GetInput();

	Operate();
	
	// ��ʂ�h�炷
	if (m_bShake)
	{
		m_nCnt++;

		m_PosR.x += m_fAmplitude * sinf(m_nCnt * D3DX_PI * 0.5f) * sinf(m_Rot.y + D3DX_PI);
		m_PosR.y += m_fAmplitude * sinf(m_nCnt * D3DX_PI * 0.5f);
		m_PosR.z += m_fAmplitude * sinf(m_nCnt * D3DX_PI * 0.5f) * cosf(m_Rot.y + D3DX_PI);
	}
	if (m_nCnt == m_nShakeTime)
	{
		m_bShake = false;
		m_nCnt = 0;
	}

	Vector3 pos = player->GetPosition();
	Vector3 move = player->GetMove();
	Vector3 rad;
	float radTps;
	CEnemy *target;
	Vector3 targetPos;
	float x, z;
	switch (m_nMode)
	{// �J�������[�h���Ƃ̏���
	case CAMERAMODE_SNEAK:
		// ��]�p�̍X�V
		rad = m_RotN - m_Rot;
		CManager::CheckRot(&rad);	// �~�����`�F�b�N
		m_Rot += rad * ROT_ATTEN;
		CManager::CheckRot(&m_Rot); // �~�����`�F�b�N

		// �����_�ړ��ʂ̍X�V
		// ���x�ɂ���Ď��̖ړI�n��L�΂�
		m_MoveR.x = (move.x * MOVE_COEFF_V + pos.x
			- sinf(m_Rot.y + D3DX_PI) * DISTANCE_R - m_PosR.x) * MOVE_ATTEN_R;
		m_MoveR.y = (pos.y + HEIGHT - m_PosR.y) * MOVE_ATTEN_R;
		m_MoveR.z = (move.z * MOVE_COEFF_V + pos.z
			- cosf(m_Rot.y + D3DX_PI) * DISTANCE_R - m_PosR.z) * MOVE_ATTEN_R;

		// �����_�̈ʒu�X�V
		m_PosR.x += m_MoveR.x;
		m_PosR.y += m_MoveR.y;
		m_PosR.z += m_MoveR.z;

		radTps = m_Rot.y - TPS_ANGLE;
		CManager::CheckRot(&radTps); // �~�����`�F�b�N

		// ���_�ړ��ʂ̍X�V
		m_MoveV.x = ((move.x * MOVE_COEFF_R + pos.x
			- sinf(radTps) * DISTANCE_V) - m_Pos.x) * MOVE_ATTEN_V;
		m_MoveV.y = (pos.y + HEIGHT - m_Pos.y) * MOVE_ATTEN_R;
		m_MoveV.z = ((move.z * MOVE_COEFF_R + pos.z
			- cosf(radTps) * DISTANCE_V) - m_Pos.z) * MOVE_ATTEN_V;

		// ���_�ʒu�̍X�V
		m_Pos.x += m_MoveV.x;
		m_Pos.y += m_MoveV.y;
		m_Pos.z += m_MoveV.z;
		break;
	case CAMERAMODE_ROCKON:
		// ��]�p�̍X�V
		x = m_PosR.x - pos.x;
		z = m_PosR.z - pos.z;
		m_RotN.y = atan2f(x, z);
		CManager::CheckRot(&m_RotN);	// �~�����`�F�b�N
		rad = m_RotN - m_Rot;
		CManager::CheckRot(&rad);		// �~�����`�F�b�N
		m_Rot += rad * ROT_ATTEN;
		CManager::CheckRot(&m_Rot);		// �~�����`�F�b�N

		// �^�[�Q�b�g�̈ʒu�擾
		target = CEnemy::Get(player->GetTargetId());
		if (target)
		{
			targetPos = target->GetTargetPos();
		}
		else
		{
			return;
		}
		
		// �����_�ړ��ʂ̍X�V
		// ���x�ɂ���Ď��̖ړI�n��L�΂�
		m_MoveR.x = (targetPos.x - m_PosR.x) * MOVE_ATTEN_R_ROCKON;
 		m_MoveR.y = (targetPos.y - m_PosR.y) * MOVE_ATTEN_R_ROCKON;
		m_MoveR.z = (targetPos.z - m_PosR.z) * MOVE_ATTEN_R_ROCKON;

		// �����_�̈ʒu�X�V
		m_PosR.x += m_MoveR.x;
		m_PosR.y += m_MoveR.y;
		m_PosR.z += m_MoveR.z;

		radTps = m_Rot.y - TPS_ANGLE;
		CManager::CheckRot(&radTps); // �~�����`�F�b�N

		// ���_�ړ��ʂ̍X�V
		m_MoveV.x = ((pos.x - DISTANCE_V * sinf(radTps)) - m_Pos.x) * ROT_ATTEN_ROCKON;
		m_MoveV.y = (pos.y + HEIGHT - m_Pos.y) * ROT_ATTEN_ROCKON;
		m_MoveV.z = ((pos.z - DISTANCE_V * cosf(radTps)) - m_Pos.z) * ROT_ATTEN_ROCKON;

		// ���_�ʒu�̍X�V
		m_Pos.x += m_MoveV.x;
		m_Pos.y += m_MoveV.y;
		m_Pos.z += m_MoveV.z;
		break;
	default:
		break;
	}
}

/*******************************************************************************
* �֐����Fvoid CCamera::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CCamera::Draw( void )
{

}

/*******************************************************************************
* �֐����Fvoid CCamera::SetCamera( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�J�����Z�b�g����
*******************************************************************************/
void CCamera::Set( void )
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxProjection );

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH( &m_MtxProjection,
								ANGLE * ( D3DX_PI / 180 ),
								( float )SCREEN_WIDTH / SCREEN_HEIGHT,
								NEARZ,
								FARZ );

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform( D3DTS_PROJECTION, &m_MtxProjection );

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxView );
	D3DXMatrixLookAtLH( &m_MtxView,
						&m_Pos,
						&m_PosR,
						&m_VecU );
	pDevice->SetTransform( D3DTS_VIEW, &m_MtxView );
}

/*******************************************************************************
* �֐����FCCamera *CCamera::Create( void )
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CCamera *CCamera::Create( void )
{
	CCamera *camera;
	camera = new CCamera;
	camera->Init();
	return camera;
}

/*******************************************************************************
* �֐����FVector3 CCamera::GetPosition( void )
* 
* ����	�F
* �߂�l�F
* ����	�F���W�ʒu��Ԃ�
*******************************************************************************/
Vector3 CCamera::GetPosition( void )
{
	return m_Pos;
}

/*******************************************************************************
* �֐����FD3DXMATRIX *CCamera::GetMtxProj( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�v���W�F�N�V�����}�g���N�X��Ԃ�
*******************************************************************************/
D3DXMATRIX *CCamera::GetMtxProj( void )
{
	return &m_MtxProjection;
}

/*******************************************************************************
* �֐����FD3DXMATRIX *CCamera::GetMtxProj( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�r���[�}�g���N�X��Ԃ�
*******************************************************************************/
D3DXMATRIX *CCamera::GetMtxView( void )
{
	return &m_MtxView;
}

/*******************************************************************************
* �֐����FD3DXVECTOR3 CCamera::GetVecRV( void )
* 
* ����	�F
* �߂�l�F
* ����	�F���_���璍���_�̃x�N�g���Ԃ�
*******************************************************************************/
D3DXVECTOR3 CCamera::GetVecRV( void )
{
	D3DXVECTOR3 vector;
	vector = m_PosR - m_Pos;

	return vector;
}

/*******************************************************************************
* �֐����Fvoid CCamera::SetShake(float amplitudeint, int time)
*
* ����	�F
* �߂�l�F
* ����	�F�J������h�炷
*******************************************************************************/
void CCamera::SetShake(float amplitudeint, int time)
{
	m_nCnt = 0;
	m_bShake = true;
	m_fAmplitude = amplitudeint;
	m_nShakeTime = time;
}

/*******************************************************************************
* �֐����Fvoid CCamera::SetCameraMode(int id)
*
* ����	�F
* �߂�l�F
* ����	�F�J�����̃��[�h�ݒ�
*******************************************************************************/
void CCamera::SetCameraMode(int id)
{
	m_nMode = id;
}

/*******************************************************************************
* �֐����Fvoid CCamera::UpdateMode(void)
*
* ����	�F
* �߂�l�F
* ����	�F�J�����̃��[�h�ݒ�
*******************************************************************************/
void CCamera::UpdateMode(void)
{

}

/*******************************************************************************
* �֐����Fvoid CCamera::Operate(void)
*
* ����	�F
* �߂�l�F
* ����	�F�J�����̑���ݒ�
*******************************************************************************/
void CCamera::Operate(void)
{
	CInput *input = CManager::GetInput();

	if (input->GetJoyStickConnected())
	{
		OperateJoyStick();
		return;
	}

	// ���_����
	if (input->GetKeyboardPress(DIK_LEFT))
	{
		m_RotN.y -= ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // �~�����`�F�b�N
	}
	// ���_����
	if (input->GetKeyboardPress(DIK_RIGHT))
	{
		m_RotN.y += ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // �~�����`�F�b�N
	}

	// �����_�㉺
	if (input->GetKeyboardPress(DIK_T))
	{
		m_PosR.y += 30.0f;
	}
	if (input->GetKeyboardPress(DIK_G))
	{
		m_PosR.y -= 30.0f;
	}
}

/*******************************************************************************
* �֐����Fvoid CCamera::Operate(void)
*
* ����	�F
* �߂�l�F
* ����	�F�J�����̑���ݒ�
*******************************************************************************/
void CCamera::OperateJoyStick(void)
{
	CInput *input = CManager::GetInput();
	XINPUT_STATE *stateX = input->GetPressState();

	// ���_����
	if (stateX->Gamepad.sThumbRX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		m_RotN.y -= ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // �~�����`�F�b�N
	}
	// ���_����
	if (stateX->Gamepad.sThumbRX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		m_RotN.y += ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // �~�����`�F�b�N
	}
}