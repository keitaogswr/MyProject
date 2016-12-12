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
#include "editorCamera.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define NEARZ				( 5.0f )		// Near�l
#define FARZ				( 10000.0f )	// Far�l
#define ANGLE				( 40.0f )		// �J�����̎���p
#define MOVE_SPEED			( 10.0f )		// �J�����̎������x
#define ROT_SPEED			( 0.1f )		// �J�����̎��]���x
#define DISTANCE			( 1000.0f )		// �J�������_�ƒ����_�̏�������
#define HEIGHT				( 500.0f )		// �J�������_�̏�������

#define ROT_ATEEN			( 0.1f )		// �J�����̉�]�����W��
#define MOVE_ATEEN_V		( 0.5f )		// ���_�̑��x�����W��
#define MOVE_ATEEN_R		( 0.05f )		// �����_�̑��x�����W��
#define MOVE_COEFF_V		( 15.0f )		// ���_�ړ������̔{���W��
#define MOVE_COEFF_R		( 8.0f )		// �����_�ړ������̔{���W��

#define TPS_ANGLE			( 0.3f )		// TPS���[�h���̃J�����̈ʒu�p�x
#define ROT_TPS_ATEEN		( 0.1f )		// �J�����̉�]�����W��(TPS)

#define SHAKE_CNT			( 10 )			// �U���J�E���^

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCEditorCamera::CEditorCamera()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CEditorCamera::CEditorCamera()
{
}

/*******************************************************************************
* �֐����FCEditorCamera::~CEditorCamera()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CEditorCamera::~CEditorCamera()
{
}

/*******************************************************************************
* �֐����Fvoid CEditorCamera::Init( void )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CEditorCamera::Init(void)
{
	// �J���� ///
	m_Pos = Vector3(0.0f, HEIGHT, -DISTANCE);
	m_PosR = Vector3(0.0f, 0.0f, 0.0f);
	m_VecU = Vector3(0.0f, 1.0f, 0.0f);

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&m_MtxProjection,
		ANGLE * (D3DX_PI / 180),
		(float)SCREEN_WIDTH / SCREEN_HEIGHT,
		NEARZ,
		FARZ);

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &m_MtxProjection);

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxView);
	D3DXMatrixLookAtLH(&m_MtxView,
		&m_Pos,
		&m_PosR,
		&m_VecU);
	pDevice->SetTransform(D3DTS_VIEW, &m_MtxView);
}

/*******************************************************************************
* �֐����Fvoid CEditorCamera::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CEditorCamera::Uninit(void)
{

}

/*******************************************************************************
* �֐����Fvoid CEditorCamera::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CEditorCamera::Update(void)
{
	float rad = 0.0f;
	if (CInput::GetKeyboardPress(DIK_Q))
	{
		m_Rot.y -= ROT_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_E))
	{
		m_Rot.y += ROT_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_R))
	{
		m_MoveV.y += MOVE_SPEED;
		m_MoveR.y += MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_F))
	{
		m_MoveV.y -= MOVE_SPEED;
		m_MoveR.y -= MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_T))
	{
		m_MoveR.y += MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_G))
	{
		m_MoveR.y -= MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_W) || CInput::GetKeyboardPress(DIK_D) ||
		CInput::GetKeyboardPress(DIK_A) || CInput::GetKeyboardPress(DIK_S))
	{// �ړ��L�[�������Ă�����
		// �O�ړ�
		if (CInput::GetKeyboardPress(DIK_W) && (!CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A)))
		{
			rad = m_Rot.y;
		}
		// �΂߉E�O�ړ�
		else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_D))
		{
			rad = m_Rot.y + D3DX_PI * 0.25f;
		}
		// �΂ߍ��O�ړ�
		else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_A))
		{
			rad = m_Rot.y - D3DX_PI * 0.25f;
		}
		// ���ړ�
		if (CInput::GetKeyboardPress(DIK_S) && !CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A))
		{
			rad = m_Rot.y + D3DX_PI;
		}
		// �΂߉E���ړ�
		else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_D))
		{
			rad = m_Rot.y + D3DX_PI * 0.75f;
		}
		// �΂ߍ����ړ�
		else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_A))
		{
			rad = m_Rot.y - D3DX_PI * 0.75f;
		}
		// �E�ړ�
		if (CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
		{
			rad = m_Rot.y + D3DX_PI * 0.5f;
		}
		// ���ړ�
		if (CInput::GetKeyboardPress(DIK_A) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
		{
			rad = m_Rot.y - D3DX_PI * 0.5f;
		}
		CManager::CheckRot(&rad); // �~�����`�F�b�N

		// ���_�ړ��ʂ̍X�V
		m_MoveV.x += sinf(rad) * MOVE_SPEED;
		m_MoveV.z += cosf(rad) * MOVE_SPEED;
	}

	/* �ړ��ʁA�ʒu�̍X�V */
	// �ړ��ʂ̌���
	m_MoveV += (Vector3(0.0f, 0.0f, 0.0f) - m_MoveV) * MOVE_ATEEN_V;
	m_MoveR += (Vector3(0.0f, 0.0f, 0.0f) - m_MoveR) * MOVE_ATEEN_V;

	// ���_�A�����_�ʒu�̍X�V
	m_Pos += m_MoveV;
	m_PosR.x = m_Pos.x + sinf(m_Rot.y) * DISTANCE;
	m_PosR.y += m_MoveR.y;
	m_PosR.z = m_Pos.z + cosf(m_Rot.y) * DISTANCE;
}

/*******************************************************************************
* �֐����FCEditorCamera *CEditorCamera::Create( void )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CEditorCamera *CEditorCamera::Create(void)
{
	CEditorCamera *camera;
	camera = new CEditorCamera;
	camera->Init();
	return camera;
}