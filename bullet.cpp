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
#include "sceneX.h"
#include "dynamicModel.h"
#include "player.h"
#include "playerT.h"
#include "enemy.h"
#include "effect.h"
#include "animationBoard.h"
#include "explosion.h"
#include "bullet.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH				( 10 )			// �|���S����
#define HEIGHT				( 10 )			// �|���S������
#define TEXTURE_ROW			( 1 )			// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN		( 1 )			// �e�N�X�`���s������
#define MOVE_SPEED			( 10.0f )		// �e�̈ړ����x
#define MOVE_SPEED_ENEMY	( 25.0f )		// �e�̈ړ����x
#define MOVE_SPEED_BOSS		( 40.0f )		// �e�̈ړ����x
#define LIFE_MAX			( 50 )			// �e�̎���
#define LIFE_MAX_ENEMY		( 50 )			// �e�̎���(�G�l�~�[)
#define LIFE_MAX_BOSS		( 100 )			// �e�̎���(�{�X)
#define NUM_UPDATE			( 3 )			// �X�V��
#define EXPLOSION_RAND		( 50 )			// �����͈�
const int PLAYER_BULLET_DAMAGE = 100;		// �e�̃_���[�W(�v���C���[)
const int ENEMY_BULLET_DAMAGE = 20;			// �e�̃_���[�W(�G�l�~�[)
const int BOSS_BULLET_DAMAGE = 30;			// �e�̃_���[�W(�{�X)

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCBullet::CBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CBullet::CBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBillboard(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;
	m_ObjType = OBJTYPE_NONE;
	m_Col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_Vec = Vector3(0.0f, 0.0f, 0.0f);
}

/*******************************************************************************
* �֐����FCBullet::~CBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CBullet::~CBullet()
{
}

/*******************************************************************************
* �֐����Fvoid CBullet::Init( Vector3 pos, Vector3 vec, D3DXCOLOR col )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CBullet::Init(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	m_Pos = pos;
	//m_Rot = vec;
	m_Vec = vec;
	m_Col = col;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_BULLET_000);

	// ���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-WIDTH * 0.5f,
		HEIGHT * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(WIDTH * 0.5f,
		HEIGHT * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(-WIDTH * 0.5f,
		-HEIGHT * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(WIDTH * 0.5f,
		-HEIGHT * 0.5f,
		0.0f);

	pVtx[0].nor =
	pVtx[1].nor =
	pVtx[2].nor =
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CBullet::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CBullet::Uninit(void)
{
	// �G�t�F�N�g�̐���
	CExplosion::Create(m_Pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 100.0f, 100.0f);
	// ���
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CBullet::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBullet::Update(void)
{
	// �G�t�F�N�g�̐���
	for (int i = 0; i < NUM_UPDATE; i++)
	{
		// �ʒu�X�V
		//m_Pos.x += sinf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
		//m_Pos.z += cosf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
		m_Pos += m_Vec * MOVE_SPEED;
		CEffect *effect = CEffect::Create(m_Pos, m_Col, 30, 30);
	}

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
		|| m_Pos.z < -depth || m_Pos.z > depth
		|| mesh->GetHeight(m_Pos) > m_Pos.y)
	{// �������s������A�܂��̓t�B�[���h�̊O�ɏo����A�������̓t�B�[���h�̒��ɓ�������j��
		Uninit();
	}
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_ENEMY)
		{
			Vector3 pos = ((CEnemy*)scene)->GetTargetPos();
			Vector3 diff = pos - m_Pos;
			float length = diff.Length();
			int search = ((CEnemy*)scene)->GetCollisionLength();
			if (length < search)
			{
				CEnemy *enemy = (CEnemy*)scene;
				if (enemy->GetState() != CEnemy::STATE_GUARD)
				{
					enemy->SetDamage(PLAYER_BULLET_DAMAGE);
					CGame *game = (CGame*)CManager::GetMode();
					CCamera *camera = game->GetCamera();
					camera->SetShake();
				}
				Uninit();
			}
		}
		scene = next;
	}
}

/*******************************************************************************
* �֐����Fvoid CBullet::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CBullet::Draw(void)
{

}

/*******************************************************************************
* �֐����FCBullet *CBullet::Create( Vector3 pos, Vector3 vec )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CBullet *CBullet::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	CBullet *bullet;
	bullet = new CBullet;

	bullet->Init(pos, vec, col);
	return bullet;
}

/*******************************************************************************
* �֐����FCEnemyBullet::CBossBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CEnemyBullet::CEnemyBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX_ENEMY;
}

/*******************************************************************************
* �֐����Fvoid CEnemyBullet::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CEnemyBullet::Update(void)
{
	// �G�t�F�N�g�̐���
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 20, 20);
	m_Pos += m_Vec * MOVE_SPEED_ENEMY;
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
		|| m_Pos.z < -depth || m_Pos.z > depth
		|| mesh->GetHeight(m_Pos) > m_Pos.y)
	{// �������s������A�܂��̓t�B�[���h�̊O�ɏo����A�������̓t�B�[���h�̒��ɓ�������j��
		Uninit();
	}
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_PLAYER)
		{
			Vector3 pos = scene->GetPosition();
			Vector3 diff = pos - m_Pos;
			float length = diff.Length();
			if (length < 40)
			{
				CPlayer *player = (CPlayer*)scene;
				player->SetDamage(ENEMY_BULLET_DAMAGE);
				CGame *game = (CGame*)CManager::GetMode();
				CCamera *camera = game->GetCamera();
				camera->SetShake();
				Uninit();
			}
		}
		scene = next;
	}
}

/*******************************************************************************
* �֐����FCEnemyBullet *CEnemyBullet::Create( Vector3 pos, Vector3 vec )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CEnemyBullet *CEnemyBullet::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	CEnemyBullet *bullet;
	bullet = new CEnemyBullet;

	bullet->Init(pos, vec, col);
	return bullet;
}


/*******************************************************************************
* �֐����FCBossBullet::CBossBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CBossBullet::CBossBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX_BOSS;
}

/*******************************************************************************
* �֐����Fvoid CBossBullet::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBossBullet::Update(void)
{
	// �G�t�F�N�g�̐���
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 100, 100);
	m_Pos += m_Vec * MOVE_SPEED_BOSS;
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
		|| m_Pos.z < -depth || m_Pos.z > depth
		|| mesh->GetHeight(m_Pos) > m_Pos.y)
	{// �������s������A�܂��̓t�B�[���h�̊O�ɏo����A�������̓t�B�[���h�̒��ɓ�������j��
		Uninit();
	}
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_PLAYER)
		{
			Vector3 pos = scene->GetPosition();
			Vector3 diff = pos - m_Pos;
			float length = diff.Length();
			if (length < 100)
			{
				CPlayer *player = (CPlayer*)scene;
				player->SetDamage(BOSS_BULLET_DAMAGE);
				CGame *game = (CGame*)CManager::GetMode();
				CCamera *camera = game->GetCamera();
				camera->SetShake();
				Uninit();
			}
		}
		scene = next;
	}
}

/*******************************************************************************
* �֐����FCBossBullet *CBossBullet::Create( Vector3 pos, Vector3 vec )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CBossBullet *CBossBullet::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	CBossBullet *bullet;
	bullet = new CBossBullet;

	bullet->Init(pos, vec, col);
	return bullet;
}