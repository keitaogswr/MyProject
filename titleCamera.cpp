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
#include "titleCamera.h"
#include "input.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "player.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define NEARZ		( 5.0f )				// Near�l
#define FARZ		( 10000.0f )			// Far�l
#define ANGLE		( 40.0f )				// �J�����̎���p
#define MOVE_SPEED	( 3.0f )				// �J�����̎������x
#define ROT_SPEED	( 0.01f )				// �J�����̎��]���x
#define DISTANCE	( 150.0f )				// �J�������_�ƒ����_�̏�������
#define HEIGHT		( 35.0f )				// �J�������_�̏�������

#define ROT_ATTEN			( 0.1f )		// �J�����̉�]�����W��
#define MOVE_ATTEN_V		( 0.05f )		// ���_�̑��x�����W��
#define MOVE_ATTEN_R		( 0.05f )		// �����_�̑��x�����W��
#define MOVE_COEFF_V		( 15.0f )		// ���_�ړ������̔{���W��
#define MOVE_COEFF_R		( 8.0f )		// �����_�ړ������̔{���W��

#define TPS_ANGLE			( 0.3f )		// TPS���[�h���̃J�����̈ʒu�p�x
#define ROT_TPS_ATTEN		( 0.1f )		// �J�����̉�]�����W��(TPS)
//#define MOVE_TPS_ATTEN_V	( 0.15f )		// ���_�̑��x�����W��(TPS)
//#define MOVE_TPS_ATTEN_R	( 0.15f )		// �����_�̑��x�����W��(TPS)
//#define MOVE_TPS_COEFF_V	( 8.0f )		// ���_�ړ������̔{���W��(TPS)
//#define MOVE_TPS_COEFF_R	( 4.0f )		// �����_�ړ������̔{���W��(TPS)

#define SHAKE_CNT			( 10 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCTitleCamera::CTitleCamera()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CTitleCamera::CTitleCamera()
{
}

/*******************************************************************************
* �֐����FCTitleCamera::~CTitleCamera()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CTitleCamera::~CTitleCamera()
{
}

/*******************************************************************************
* �֐����Fvoid CTitleCamera::Init( void )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CTitleCamera::Init(void)
{
	// �J���� ///
	m_Pos = Vector3(0.0f, HEIGHT, -DISTANCE);
	m_PosR = Vector3(0.0f, HEIGHT, 0.0f);
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
* �֐����Fvoid CTitleCamera::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CTitleCamera::Uninit(void)
{

}

/*******************************************************************************
* �֐����Fvoid CTitleCamera::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CTitleCamera::Update(void)
{
	m_Rot.y += ROT_SPEED;

	CManager::CheckRot(&m_Rot); // �~�����`�F�b�N

	// ���_�ړ��ʂ̍X�V
	m_MoveV.x = sinf(m_Rot.y) * DISTANCE;
	m_MoveV.z = cosf(m_Rot.y) * DISTANCE;

	// ���_�ʒu�̍X�V
	m_Pos.x = m_MoveV.x;
	m_Pos.z = m_MoveV.z;
}

/*******************************************************************************
* �֐����Fvoid CTitleCamera::SetCamera( void )
*
* ����	�F
* �߂�l�F
* ����	�F�J�����Z�b�g����
*******************************************************************************/
void CTitleCamera::Set(void)
{
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
* �֐����FCTitleCamera *CTitleCamera::Create( void )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CTitleCamera *CTitleCamera::Create(void)
{
	CTitleCamera *camera;
	camera = new CTitleCamera;
	camera->Init();
	return camera;
}