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
#include "effect.h"
#include "animationBoard.h"
#include "explosion.h"
#include "enemySpeed.h"
#include "player.h"
#include "afterBurner.h"
#include "camera.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define MOVE_SPEED		( 2.0f )		// 移動量
#define ROT_SPEED		( 0.05f )		// 回転量

#define ROT_ATTEN		( 0.1f )		// 回転量減衰係数
#define MOVE_ATTEN		( 0.01f )		// 移動量減衰係数

const float SHADOW_RADIUS = 20.0f;		// 影の半径
const float SHADOW_HEIGHT = 1000.0f;	// 影の高さ
const int LIFE_MAX = 100;				// 最大ライフ
const int SEARCH_LENG = 1600;			// 索敵範囲
const int ATTACK_CNT = 60;				// 攻撃カウンタ
const int COLLISION_LENGTH = 10;		// あたり判定
const int DAMAGE_CNT = 60;				// 被弾カウンタ
const int SEARCH_CNT = 10;				// 索敵カウンタ
const int BURNER_CNT = 15;				// バーナーカウンタ

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CEnemySpeed::CEnemySpeed(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CEnemySpeed::CEnemySpeed(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
{
	m_Scl = Vector3(1.5f, 1.5f, 1.5f);
}

/*******************************************************************************
* 関数名：CEnemySpeed::~CEnemy()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CEnemySpeed::~CEnemySpeed()
{
}

/*******************************************************************************
* 関数名：void CEnemySpeed::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CEnemySpeed::Init(Vector3 pos)
{
	m_Pos = pos;
	m_TargetPos = m_Pos;
	m_Shadow = CStencilShadow::Create(m_Pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_ENEMY_01, &m_MtxWorld);
	m_MotionManager->SetMotion(0);
	m_pAfterBurner = CAfterBurner::Create(m_Pos, Vector3(0.0f, 0.0f, 0.0f), &m_MtxWorld);
}

/*******************************************************************************
* 関数名：void CEnemySpeed::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CEnemySpeed::Uninit(void)
{
	// 解放
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	m_Shadow->SetDeleteFlg(true);
	// エフェクトの生成
	CExplosion::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 500.0f, 500.0f);
	SAFE_DELETE(m_pAfterBurner);
}

/*******************************************************************************
* 関数名：void CEnemySpeed::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CEnemySpeed::Update(void)
{
	// 位置更新
	UpdateMove();

	// 索敵更新
	UpdateSearch();

	// 目的の角度の補正
	CManager::CheckRot(&m_RotN.y);

	// 差分計算
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);

	// 向きを更新
	m_Rot.y += m_Rad.y * ROT_ATTEN;
	CManager::CheckRot(&m_Rot.y);

	// 攻撃更新
	UpdateAttack();

	// 状態遷移
	UpdateState();

	// 影の位置更新
	m_Shadow->Set(Vector3(m_Pos.x, m_Pos.y, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	// モーション更新
	m_MotionManager->Update();

	// 寿命チェック
	if (m_nLife <= 0)
	{
		// 削除フラグ
		SetDeleteFlg(true);
		DeleteTarget();
	}
}

/*******************************************************************************
* 関数名：CEnemySpeed *CEnemySpeed::Create(Vector3 pos)
*
* 引数	：
* 戻り値：CEnemy型のポインタ
* 説明	：生成処理
*******************************************************************************/
CEnemySpeed *CEnemySpeed::Create(Vector3 pos)
{
	CEnemySpeed *model;
	model = new CEnemySpeed;
	model->Init(pos);
	return model;
}

/*******************************************************************************
* 関数名：void CEnemySpeed::UpdateState(void)
*
* 引数	：
* 戻り値：
* 説明	：状態更新処理
*******************************************************************************/
void CEnemySpeed::UpdateState(void)
{
	m_nStateCnt++;
	switch (m_State)
	{
	case STATE_NORMAL:
		if (m_bSearch)
		{
			SetState(STATE_ATTACK);
		}
		if ((m_nStateCnt % BURNER_CNT) == 0)
		{
			m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, -0.5f, 0.0f));
		}
		break;
	case STATE_MOVE:
		if ((m_nStateCnt % BURNER_CNT) == 0)
		{
			m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
		}
		break;
	case STATE_ATTACK:
		if ((m_nStateCnt % BURNER_CNT) == 0)
		{
			m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
		}
		break;
	case STATE_DAMAGE:
		m_MotionManager->SetModelColorAll(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		if (m_nStateCnt >= DAMAGE_CNT)
		{
			SetState(STATE_NORMAL);
		}
		break;
	default:
		break;
	}
}

/*******************************************************************************
* 関数名：void CEnemySpeed::SetState(int state)
*
* 引数	：
* 戻り値：
* 説明	：状態設定処理
*******************************************************************************/
void CEnemySpeed::SetState(int state)
{
	switch (state)
	{
	case STATE_NORMAL:
		m_MotionManager->SetMotion(0);
		break;
	case STATE_MOVE:
		m_MotionManager->SetMotion(1);
		break;
	case STATE_ATTACK:
		m_MotionManager->SetMotion(2);
		break;
	default:
		break;
	}
	m_State = state;
	m_nStateCnt = 0;
}

/*******************************************************************************
* 関数名：void CEnemySpeed::UpdateMove(void)
*
* 引数	：
* 戻り値：
* 説明	：移動更新処理
*******************************************************************************/
void CEnemySpeed::UpdateMove(void)
{
	if (m_State == STATE_MOVE || m_State == STATE_ATTACK)
	{
		m_nSearchCnt++;
		if ((m_nSearchCnt % SEARCH_CNT) == 0)
		{
			CGame *game = (CGame*)CManager::GetMode();
			CPlayer *player = game->GetPlayer();
			Vector3 pos = player->GetPosition();
			Vector3 diff = Vector3(pos.x - m_Pos.x, 0.0f, pos.z - m_Pos.z);
			diff.Normalize();
			m_Move += diff * MOVE_SPEED;
		}
	}
	// 移動量の減衰
	m_Move += (Vector3(0.0f, 0.0f, 0.0f) - m_Move) * MOVE_ATTEN;
	// 位置の更新
	m_Pos += m_Move;

	// 敵同士の当たり判定
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
		if (scene->GetObjType() == OBJTYPE_ENEMY && scene != this)
		{
			Vector3 pos = scene->GetPosition();
			CEnemy *enemy = (CEnemy*)scene;
			float collision = enemy->GetCollisionLength();
			if (CheckCollision(pos, collision))
			{
				m_Pos -= m_Move;
			}
		}
		scene = next;
	}

	// フィールドの当たり判定
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();
	if (mesh->GetHeight(m_Pos) > m_Pos.y)
	{// 寿命が尽きたら、またはフィールドの外に出たら、もしくはフィールドの中に入ったら破棄
		m_Pos -= m_Move;
	}
	
	// ターゲット位置を更新
	m_TargetPos = m_Pos;
}

/*******************************************************************************
* 関数名：bool CEnemySpeed::CheckCollision(Vector3 pos, float collision)
*
* 引数	：
* 戻り値：
* 説明	：当たり判定処理
*******************************************************************************/
bool CEnemySpeed::CheckCollision(Vector3 pos, float collision)
{
	if (m_Pos.x + m_fCollisionLength > pos.x - collision && m_Pos.x - m_fCollisionLength < pos.x + collision &&
		m_Pos.y + m_fCollisionLength > pos.y - collision && m_Pos.y - m_fCollisionLength < pos.y + collision &&
		m_Pos.z + m_fCollisionLength > pos.z - collision && m_Pos.z - m_fCollisionLength < pos.z + collision)
	{
		Vector3 diff = pos - m_Pos;
		float length = diff.Length();
		if (length < (collision + m_fCollisionLength))
		{
			return true;
		}
	}
	
	return false;
}