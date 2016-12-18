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
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "scene3D.h"
#include "meshField.h"
#include "dynamicModel.h"
#include "shadow.h"
#include "stencilShadow.h"
#include "enemy.h"
#include "billboard.h"
#include "bullet.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define MOVE_SPEED		( 1.0f )		// �ړ���
#define ROT_SPEED		( 0.05f )		// ��]��

#define ROT_ATEEN		( 0.1f )		// ��]�ʌ����W��
#define MOVE_ATEEN		( 0.15f )		// �ړ��ʌ����W��

const float SHADOW_RADIUS = 20.0f;		// �e�̔��a
const float  SHADOW_HEIGHT = 1000.0f;	// �e�̍���
#define LIFE_MAX		( 100 )			// �ő僉�C�t
#define SEARCH_LENG		( 500 )
#define ATTACK_CNT		( 60 )
const int COLLISION_LENGTH = 40;
const int DAMAGE_CNT = 60;

#define TEXTFILENAME	( "data\\EDITDATA\\EnemyData.txt" )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCEnemy::CEnemy( DRAWORDER DrawOrder, OBJTYPE ObjType ):CDynamicModel( DrawOrder, ObjType )
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CEnemy::CEnemy( DRAWORDER DrawOrder, OBJTYPE ObjType ):CDynamicModel( DrawOrder, ObjType )
{
	m_Shadow = NULL;
	m_nLife = LIFE_MAX;
	m_Scl = Vector3(1.0f, 1.0f, 1.0f);
	m_bSearch = false;
	m_nAttCnt = 0;
	m_TargetPos = Vector3(0.0f, 0.0f, 0.0f);
	m_nCollisionLength = COLLISION_LENGTH;
	m_State = STATE_NORMAL;
	m_nStateCnt = 0;
}

/*******************************************************************************
* �֐����FCEnemy::~CEnemy()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CEnemy::~CEnemy()
{
}

/*******************************************************************************
* �֐����Fvoid CEnemy::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CEnemy::Init( Vector3 pos )
{
	m_Pos = pos;
	m_TargetPos = m_Pos;
	m_Shadow = CStencilShadow::Create(m_Pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_ENEMY_00, &m_MtxWorld);
	m_MotionManager->SetMotion(0);
}

/*******************************************************************************
* �֐����Fvoid CEnemy::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CEnemy::Uninit( void )
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
}

/*******************************************************************************
* �֐����Fvoid CEnemy::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CEnemy::Update( void )
{
	D3DXVECTOR3 axis = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXQUATERNION quat = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 0.0f );
	CGame *game = ( CGame* )CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();

	// �ʒu�X�V
	m_Pos.x += m_Move.x;
	m_Pos.z += m_Move.z;

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
				m_RotN.y = atan2f( m_Pos.x - pos.x, m_Pos.z - pos.z );

				m_bSearch = true;
			}
			else
			{
				m_bSearch = false;
				m_nAttCnt = 0;
			}
		}
		scene = next;
	}

	// �ړI�̊p�x�̕␳
	CManager::CheckRot( &m_RotN.y );

	// �����v�Z
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot( &m_Rad.y );

	// �������X�V
	m_Rot.y += m_Rad.y * ROT_ATEEN;
	CManager::CheckRot( &m_Rot.y );

	if (m_bSearch)
	{
		m_nAttCnt++;
		if (m_nAttCnt > ATTACK_CNT)
		{
			m_nAttCnt = 0;
			scene = CScene::GetList(DRAWORDER_3D);
			next = NULL;
			while (scene != NULL)
			{
				next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
				if (scene->GetObjType() == OBJTYPE_PLAYER)
				{
					Vector3 pos = scene->GetPosition();
					Vector3 diff = pos - m_Pos;
					diff.Normalize();
					CEnemyBullet::Create(m_Pos, diff, D3DXCOLOR( 1.0f, 0.0f, 1.0f, 1.0f ));
				}
				scene = next;
			}
		}
	}

	// ��ԑJ��
	UpdateState();
	// �t�B�[���h�̈ʒu�擾
	//float field = meshField->GetHeight(m_Pos);
	// �e�̈ʒu�X�V
	m_Shadow->Set(Vector3(m_Pos.x, m_Pos.y, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	// ���[�V�����X�V
	m_MotionManager->Update();

	// �����`�F�b�N
	if (m_nLife <= 0)
	{
		Uninit();
	}
}

/*******************************************************************************
* �֐����Fvoid CEnemy::Uninit( void )
* 
* ����	�F
* �߂�l�FCEnemy�^�̃|�C���^
* ����	�F��������
*******************************************************************************/
CEnemy *CEnemy::Create( Vector3 pos )
{
	CEnemy *model;
	model = new CEnemy;
	model->Init( pos );
	return model;
}

/*******************************************************************************
* �֐����Fvoid CEnemy::Load( void )
*
* ����	�F
* �߂�l�F
* ����	�F�ǂݍ��ݏ���
*******************************************************************************/
int CEnemy::Load(void)
{
	FILE *fp;

	// �t�@�C���ǂݍ��݊J�n
	if ((fp = fopen(TEXTFILENAME, "r")) == NULL)
	{
		return EXIT_FAILURE;
	}

	CEnemy *enemy = NULL;
	Vector3 vector;
	char str[256];

	while (fscanf(fp, "%s", str) != EOF)
	{
		if (strcmp(str, "POSXYZ") == 0)
		{
			fscanf(fp, "%f %f %f", &vector.x, &vector.y, &vector.z);
			enemy = Create(vector);
		}
	}

	// �t�@�C���ǂݍ��ݏI��
	fclose(fp);

	return 0;
}

/*******************************************************************************
* �֐����Fvoid CEnemy::UpdateState(void)
*
* ����	�F
* �߂�l�F
* ����	�F��ԍX�V����
*******************************************************************************/
void CEnemy::UpdateState(void)
{
	switch (m_State)
	{
	case STATE_NORMAL:
		m_MotionManager->SetModelColorAll(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		m_MotionManager->SetModelColFlgAll(false);
		break;
	case STATE_DAMAGE:
		m_MotionManager->SetModelColorAll(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		m_nStateCnt++;
		if (m_nStateCnt >= DAMAGE_CNT)
		{
			m_State = STATE_NORMAL;
		}
		break;
	default:
		break;
	}
}