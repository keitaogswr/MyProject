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
#include "missile.h"
#include "smoke.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH				( 10 )			// �|���S����
#define HEIGHT				( 10 )			// �|���S������
#define TEXTURE_ROW			( 1 )			// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN		( 1 )			// �e�N�X�`���s������
#define MOVE_SPEED			( 20.0f )		// �e�̈ړ����x
#define MOVE_SPEED_ENEMY	( 25.0f )		// �e�̈ړ����x
#define MOVE_SPEED_BOSS		( 50.0f )		// �e�̈ړ����x
#define ROT_SPEED			( 0.01f )		// ��]���x
#define LIFE_MAX			( 120 )			// �e�̎���
#define LIFE_MAX_ENEMY		( 50 )			// �e�̎���(�G�l�~�[)
#define LIFE_MAX_BOSS		( 100 )			// �e�̎���(�{�X)
#define NUM_UPDATE			( 3 )			// �X�V��
#define EXPLOSION_RAND		( 50 )			// �����͈�
const int PLAYER_BULLET_DAMAGE = 1;			// �e�̃_���[�W(�v���C���[)
const int ENEMY_BULLET_DAMAGE = 20;			// �e�̃_���[�W(�G�l�~�[)
const int BOSS_BULLET_DAMAGE = 10;			// �e�̃_���[�W(�{�X)
const int PLAYER_BULLET_LENGTH = 50;		// �e�̃_���[�W(�v���C���[)
const int ENEMY_BULLET_LENGTH = 20;			// �e�̓����蔻��(�G�l�~�[)
const int BOSS_BULLET_LENGTH = 50;			// �e�̓����蔻��(�{�X)
const int SMOKE_UPDATE = 1;					// ���̔����Ԋu
const float ROT_ANGLE = 0.05f;				// ��]�p�x

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
CMissile::CMissile(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nSmokeCnt = 0;
	m_nSearchCnt = 0;
	m_nTargetId = -1;
	D3DXQuaternionIdentity(&m_Quaternion);
	m_nLife = LIFE_MAX;
}

/*******************************************************************************
* �֐����FCBullet::~CBullet()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CMissile::~CMissile()
{
}

/*******************************************************************************
* �֐����FCMissile::Init(Vector3 pos, Vector3 vec, int id)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CMissile::Init(Vector3 pos, Vector3 vec, int id)
{
	m_Pos = pos;
	m_Vec = vec;
	m_Vec.Normalize();
	m_nTargetId = id;

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
	pVtx[3].col = D3DCOLOR_RGBA(255, 120, 0, 255);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����FCMissile::Update(void)
*
* ����	�Fvoid
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CMissile::Update(void)
{
	if (m_nTargetId < 0)
	{// �^�[�Q�b�g���ĂȂ�������
		return;
	}
	m_nSmokeCnt++;
	if ((m_nSmokeCnt % SMOKE_UPDATE) == 0)
	{// ���̐���
		CSmoke::Create(m_Pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f), 70.0f, 70.0f);
	}

	// �N�H�[�^�j�I���̍X�V
	UpdateQuaternion();

	// �ʒu�X�V
	m_Pos += m_Vec * MOVE_SPEED;

	// �����蔻��X�V
	UpdateCollision();

	// ��������
	DeleteCheak();
}

/*******************************************************************************
* �֐����Fvoid CMissile::Update(void)
*
* ����	�Fvoid
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CMissile::Draw(void)
{
	CBillboard::Draw();
}

/*******************************************************************************
* �֐����FCMissile *CMissile::Create(Vector3 pos, Vector3 vec, int id)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CMissile *CMissile::Create(Vector3 pos, Vector3 vec, int id)
{
	CMissile *missile;
	missile = new CMissile;

	missile->Init(pos, vec, id);
	return missile;
}

/*******************************************************************************
* �֐����FCMissile::UpdateCollision(void)
*
* ����	�F
* �߂�l�F
* ����	�F�����蔻��X�V����
*******************************************************************************/
void CMissile::UpdateCollision(void)
{
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
			float search = ((CEnemy*)scene)->GetCollisionLength();
			if (length < search + PLAYER_BULLET_LENGTH)
			{
				CEnemy *enemy = (CEnemy*)scene;
				if (enemy->GetState() != CEnemy::STATE_GUARD)
				{
					enemy->SetDamage(PLAYER_BULLET_DAMAGE);
					CGame *game = (CGame*)CManager::GetMode();
					CCamera *camera = game->GetCamera();
					camera->SetShake(30.0f, 10);
				}
				Uninit();
			}
		}
		scene = next;
	}
}

/*******************************************************************************
* �֐����FCMissile::UpdateQuaternion(void)
*
* ����	�F
* �߂�l�F
* ����	�F�N�H�[�^�j�I���X�V����
*******************************************************************************/
void CMissile::UpdateQuaternion(void)
{
	Vector3 axis = Vector3(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

	/* �G�̍��G */
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	// �^�[�Q�b�g���Ă��Ȃ����
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
		if (scene->GetObjType() == OBJTYPE_ENEMY && !scene->GetDeleteFlg())
		{
			CEnemy *enemy = dynamic_cast<CEnemy*>(scene);
			if (m_nTargetId == enemy->GetId())
			{
				break;
			}
		}
		scene = next;
	}

	if (!scene)
	{// �^�[�Q�b�g���Ă���G�����Ȃ�������X�V���Ȃ�
		return;
	}

	// ��������G�ւ̃x�N�g�������߂�
	Vector3 vecTarget = scene->GetTargetPos() - m_Pos;

	////�x�N�g��A��B�̒������v�Z����
	float lengthA;

	// �����̌����Ă�������ƓG�Ƃ̃x�N�g���ŊO�ς����߁A���̃x�N�g�������߂�
	D3DXVec3Cross(&axis, &m_Vec, &vecTarget);

	D3DXQuaternionRotationAxis(&quat, &axis, ROT_ANGLE);	// �������]������N�H�[�^�j�I�������߂�

	D3DXQUATERNION R, Qv;
	D3DXQuaternionConjugate(&R, &quat);

	// �x�N�g����]
	Qv.x = m_Vec.x;
	Qv.y = m_Vec.y;
	Qv.z = m_Vec.z;
	Qv.w = 0.0f;
	D3DXQuaternionMultiply(&Qv, &R, &Qv);
	D3DXQuaternionMultiply(&Qv, &Qv, &quat);

	m_Vec.x = Qv.x;
	m_Vec.y = Qv.y;
	m_Vec.z = Qv.z;
	lengthA = m_Vec.Length();
}