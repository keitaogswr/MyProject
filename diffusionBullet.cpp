/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fbullet.cpp
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
#include "camera.h"
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "billboard.h"
#include "effect.h"
#include "bullet.h"
#include "diffusionBullet.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 20 )			// �|���S����
#define HEIGHT			( 20 )			// �|���S������
#define TEXTURE_ROW		( 1 )			// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN	( 1 )			// �e�N�X�`���s������
#define MOVE_SPEED		( 5.0f )		// �e�̈ړ����x
#define LIFE_MAX		( 50 )			// �e�̎���
#define NUM_UPDATE		( 10 )			// �X�V��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCDiffusionBullet::CDiffusionBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CDiffusionBullet::CDiffusionBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;
}

/*******************************************************************************
* �֐����FCDiffusionBullet::~CDiffusionBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CDiffusionBullet::~CDiffusionBullet()
{
}

/*******************************************************************************
* �֐����Fvoid CDiffusionBullet::Init( Vector3 pos, Vector3 rot )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CDiffusionBullet::Init(Vector3 pos, Vector3 rot)
{
	CBullet::Init(pos, rot, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
}

/*******************************************************************************
* �֐����Fvoid CDiffusionBullet::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CDiffusionBullet::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CDiffusionBullet::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CDiffusionBullet::Update(void)
{
	// �ʒu�X�V
	m_Pos.x += sinf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
	m_Pos.z += cosf(m_Rot.y + D3DX_PI) * MOVE_SPEED;

	// �G�t�F�N�g�̐���
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 30, 30);

	// �t�B�[���h�̎擾
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();

	float width, depth;
	width = mesh->GetWidthAll() * 0.5f;
	depth = mesh->GetDepthAll() * 0.5f;

	// ���������炷
	m_nLife--;
	// �����`�F�b�N
	if (m_nLife <= 0
		|| m_Pos.x < -width || m_Pos.x > width
		|| m_Pos.z < -depth || m_Pos.z > depth)
	{// �������s������j��
		CBullet *bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x + 0.3f, m_Rot.y + 0.5f, m_Rot.z));
		bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x - 0.3f, m_Rot.y + 0.5f, m_Rot.z));
		bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x + 0.3f, m_Rot.y - 0.5f, m_Rot.z));
		bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x - 0.3f, m_Rot.y - 0.5f, m_Rot.z));

		Uninit();
	}
}

/*******************************************************************************
* �֐����FCDiffusionBullet *CDiffusionBullet::Create( Vector3 pos, Vector3 rot )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CDiffusionBullet *CDiffusionBullet::Create(Vector3 pos, Vector3 rot)
{
	CDiffusionBullet *bullet;
	bullet = new CDiffusionBullet;

	bullet->Init(pos, rot);
	return bullet;
}

/*******************************************************************************
* �֐����FCDiffusionBullet::CDiffusionBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CDiffusionBulletChild::CDiffusionBulletChild(DRAWORDER DrawOrder, OBJTYPE ObjType) :CDiffusionBullet(DrawOrder, ObjType)
{
	m_nLife = 120;
}


/*******************************************************************************
* �֐����Fvoid CDiffusionBulletChild::Init( Vector3 pos, Vector3 rot )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CDiffusionBulletChild::Init(Vector3 pos, Vector3 rot)
{
	CBullet::Init(pos, rot, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
}

/*******************************************************************************
* �֐����Fvoid CDiffusionBulletChild::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CDiffusionBulletChild::Update(void)
{
	// �ʒu�X�V
	m_Pos.x += sinf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
	m_Pos.y += sinf(m_Rot.x + D3DX_PI) * MOVE_SPEED;
	m_Pos.z += cosf(m_Rot.y + D3DX_PI) * MOVE_SPEED;


	// �G�t�F�N�g�̐���
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 30, 30);

	// �t�B�[���h�̎擾
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();

	float width, depth;
	width = mesh->GetWidthAll() * 0.5f;
	depth = mesh->GetDepthAll() * 0.5f;

	// ���������炷
	m_nLife--;
	// �����`�F�b�N
	if (m_nLife <= 0
		|| m_Pos.x < -width || m_Pos.x > width
		|| m_Pos.z < -depth || m_Pos.z > depth)
	{// �������s������j��
		Uninit();
	}
}

/*******************************************************************************
* �֐����FCDiffusionBulletChild *CDiffusionBulletChild::Create(Vector3 pos, Vector3 rot)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CDiffusionBulletChild *CDiffusionBulletChild::Create(Vector3 pos, Vector3 rot)
{
	CDiffusionBulletChild *bullet;
	bullet = new CDiffusionBulletChild;

	bullet->Init(pos, rot);
	return bullet;
}