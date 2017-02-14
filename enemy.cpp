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
#include "afterBurner.h"
#include "enemy.h"
#include "billboard.h"
#include "bullet.h"
#include "input.h"
#include "effect.h"
#include "animationBoard.h"
#include "explosion.h"
#include "enemySpeed.h"
#include "player.h"
#include "camera.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define MOVE_SPEED		( 1.0f )		// 移動量
#define ROT_SPEED		( 0.05f )		// 回転量

#define ROT_ATTEN		( 0.1f )		// 回転量減衰係数
#define MOVE_ATTEN		( 0.15f )		// 移動量減衰係数

const float SHADOW_RADIUS = 20.0f;		// 影の半径
const float  SHADOW_HEIGHT = 1000.0f;	// 影の高さ
const int LIFE_MAX = 100;				// 最大ライフ
const int SEARCH_LENG = 1600;			// 索敵範囲
const int ATTACK_CNT = 60;				// 攻撃カウンタ
const float COLLISION_LENGTH = 20.0f;	// あたり判定
const int DAMAGE_CNT = 60;				// 被弾カウンタ
const int BURNER_CNT = 20;				// バーナーカウンタ

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
CEnemy::CEnemy(DRAWORDER DrawOrder, OBJTYPE ObjType) :CDynamicModel(DrawOrder, ObjType)
{
	m_Shadow = NULL;
	m_nLife = LIFE_MAX;
	m_Scl = Vector3(1.0f, 1.0f, 1.0f);
	m_bSearch = false;
	m_nAttCnt = 0;
	m_TargetPos = Vector3(0.0f, 0.0f, 0.0f);
	m_fCollisionLength = COLLISION_LENGTH;
	m_State = STATE_NORMAL;
	m_nStateCnt = 0;
	m_nSearchCnt = 0;
	m_pAfterBurner = NULL;
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
void CEnemy::Init(Vector3 pos)
{
	m_Pos = pos;
	m_TargetPos = m_Pos;
	m_Shadow = CStencilShadow::Create(m_Pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_ENEMY_00, &m_MtxWorld);
	m_MotionManager->SetMotion(0);
	m_pAfterBurner = CAfterBurner::Create(m_Pos, Vector3(0.0f, -2.0f, 0.0f), &m_MtxWorld);
}

/*******************************************************************************
* 関数名：void CEnemy::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CEnemy::Uninit(void)
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
* 関数名：void CEnemy::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CEnemy::Update(void)
{
	D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;

	// 位置更新
	m_Pos.x += m_Move.x;
	m_Pos.z += m_Move.z;
	m_TargetPos = m_Pos;

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
	// フィールドの位置取得
	//float field = meshField->GetHeight(m_Pos);
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

	if ((m_nStateCnt % BURNER_CNT) == 0)
	{
		m_pAfterBurner->Set(Vector3(0.0f, 20.0f, 0.0f), Vector3(0.0f, -0.5f, 0.0f));
	}
}

/*******************************************************************************
* 関数名：CEnemy *CEnemy::Create(Vector3 pos)
*
* 引数	：
* 戻り値：CEnemy型のポインタ
* 説明	：生成処理
*******************************************************************************/
CEnemy *CEnemy::Create(Vector3 pos)
{
	CEnemy *model;
	model = new CEnemy;
	model->Init(pos);
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
	int number = 0;

	while (fscanf(fp, "%s", str) != EOF)
	{
		if (strcmp(str, "POSXYZ") == 0)
		{
			fscanf(fp, "%f %f %f %d", &vector.x, &vector.y, &vector.z, &number);
			if (number == 0)
			{
				enemy = Create(vector);
			}
			if (number == 1)
			{
				enemy = CEnemySpeed::Create(vector);
			}
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
	m_nStateCnt++;
	switch (m_State)
	{
	case STATE_NORMAL:
		m_MotionManager->SetModelColorAll(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		m_MotionManager->SetModelColFlgAll(false);

		break;
	case STATE_DAMAGE:
		m_MotionManager->SetModelColorAll(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		if (m_nStateCnt >= DAMAGE_CNT)
		{
			m_State = STATE_NORMAL;
		}
		break;
	default:
		break;
	}
}

/*******************************************************************************
* 関数名：void CEnemy::UpdateSearch(void)
*
* 引数	：
* 戻り値：
* 説明	：索敵更新処理
*******************************************************************************/
void CEnemy::UpdateSearch(void)
{
	CGame *game = (CGame*)CManager::GetMode();
	CPlayer *player = game->GetPlayer();

	if (game->GetState() != CGame::STATE_START && game->GetState() != CGame::STATE_END)
	{
		Vector3 pos = player->GetPosition();
		Vector3 diff = m_Pos - pos;
		float length = diff.Length();
		if (length < SEARCH_LENG)
		{
			m_RotN.y = atan2f(m_Pos.x - pos.x, m_Pos.z - pos.z);
			m_bSearch = true;
		}
		else
		{
			m_bSearch = false;
			m_nAttCnt = 0;
		}
	}
}

/*******************************************************************************
* 関数名：void CEnemy::UpdateAttack(void)
*
* 引数	：
* 戻り値：
* 説明	：攻撃更新処理
*******************************************************************************/
void CEnemy::UpdateAttack(void)
{
	if (m_bSearch)
	{
		m_nAttCnt++;
		if (m_nAttCnt > ATTACK_CNT)
		{
			m_nAttCnt = 0;
			CGame *game = (CGame*)CManager::GetMode();
			CPlayer *player = game->GetPlayer();
			Vector3 pos = player->GetPosition();
			Vector3 diff = pos - m_Pos;
			diff.Normalize();
			CEnemyBullet::Create(m_Pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
		}
	}
}

/*******************************************************************************
* 関数名：void CEnemy::DeleteTarget(void)
*
* 引数	：
* 戻り値：
* 説明	：ターゲット削除処理
*******************************************************************************/
void CEnemy::DeleteTarget(void)
{
	// ターゲットされてたら
	if (m_bTarget)
	{// プレイヤーのターゲットを外す
	 /* 敵の索敵 */
		CScene *scene = CScene::GetList(DRAWORDER_3D);
		CScene *next = NULL;
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			if (scene->GetObjType() == OBJTYPE_PLAYER)
			{
				dynamic_cast<CPlayer*>(scene)->SetTarget(NULL);
				CCamera *camera = dynamic_cast<CGame*>(CManager::GetMode())->GetCamera();
				camera->SetCameraMode(CAMERAMODE_SNEAK);
			}
			scene = next;
		}
	}
}