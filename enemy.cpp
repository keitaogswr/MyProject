/*******************************************************************************
* タイトル名：
* ファイル名：enemy.cpp
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
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
* マクロ定義
*******************************************************************************/
#define MOVE_SPEED		( 1.0f )		// 移動量
#define ROT_SPEED		( 0.05f )		// 回転量

#define ROT_ATEEN		( 0.1f )		// 回転量減衰係数
#define MOVE_ATEEN		( 0.15f )		// 移動量減衰係数

const float SHADOW_RADIUS = 20.0f;		// 影の半径
const float  SHADOW_HEIGHT = 1000.0f;	// 影の高さ
#define LIFE_MAX		( 100 )			// 最大ライフ
#define SEARCH_LENG		( 500 )
#define ATTACK_CNT		( 60 )
const int COLLISION_LENGTH = 40;
const int DAMAGE_CNT = 60;

#define TEXTFILENAME	( "data\\EDITDATA\\EnemyData.txt" )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CEnemy::CEnemy( DRAWORDER DrawOrder, OBJTYPE ObjType ):CDynamicModel( DrawOrder, ObjType )
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
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
* 関数名：CEnemy::~CEnemy()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CEnemy::~CEnemy()
{
}

/*******************************************************************************
* 関数名：void CEnemy::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
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
* 関数名：void CEnemy::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CEnemy::Uninit( void )
{
	// 解放
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	// 削除フラグ
	SetDeleteFlg(true);
	m_Shadow->SetDeleteFlg(true);
}

/*******************************************************************************
* 関数名：void CEnemy::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CEnemy::Update( void )
{
	D3DXVECTOR3 axis = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXQUATERNION quat = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 0.0f );
	CGame *game = ( CGame* )CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();

	// 位置更新
	m_Pos.x += m_Move.x;
	m_Pos.z += m_Move.z;

	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
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

	// 目的の角度の補正
	CManager::CheckRot( &m_RotN.y );

	// 差分計算
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot( &m_Rad.y );

	// 向きを更新
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
				next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
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

	// 状態遷移
	UpdateState();
	// フィールドの位置取得
	//float field = meshField->GetHeight(m_Pos);
	// 影の位置更新
	m_Shadow->Set(Vector3(m_Pos.x, m_Pos.y, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	// モーション更新
	m_MotionManager->Update();

	// 寿命チェック
	if (m_nLife <= 0)
	{
		Uninit();
	}
}

/*******************************************************************************
* 関数名：void CEnemy::Uninit( void )
* 
* 引数	：
* 戻り値：CEnemy型のポインタ
* 説明	：生成処理
*******************************************************************************/
CEnemy *CEnemy::Create( Vector3 pos )
{
	CEnemy *model;
	model = new CEnemy;
	model->Init( pos );
	return model;
}

/*******************************************************************************
* 関数名：void CEnemy::Load( void )
*
* 引数	：
* 戻り値：
* 説明	：読み込み処理
*******************************************************************************/
int CEnemy::Load(void)
{
	FILE *fp;

	// ファイル読み込み開始
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

	// ファイル読み込み終了
	fclose(fp);

	return 0;
}

/*******************************************************************************
* 関数名：void CEnemy::UpdateState(void)
*
* 引数	：
* 戻り値：
* 説明	：状態更新処理
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