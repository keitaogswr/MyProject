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
#include "model.h"
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "scene3D.h"
#include "meshField.h"
#include "dynamicModel.h"
#include "shadow.h"
#include "stencilShadow.h"
#include "enemy.h"
#include "bossEnemy.h"
#include "billboard.h"
#include "effect.h"
#include "bullet.h"
#include "skyDome.h"
#include "skySphere.h"
#include "barrier.h"
#include "animationBoard.h"
#include "explosion.h"
#include "input.h"
#include "chargeEffect.h"
#include "meshCylinder.h"
#include "lazer.h"
#include "enemySpeed.h"
#include "camera.h"
#include "player.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const float  MOVE_SPEED = 1.0f;			// 移動量
const float  ROT_SPEED = 0.05f;			// 回転量

const float  ROT_ATTEN = 0.1f;			// 回転量減衰係数
const float  ROT_ATTEN_1 = 0.03f;		// 回転量減衰係数
const float  MOVE_ATTEN = 0.15f;		// 移動量減衰係数

const int LIFE_MAX = 10000;				// 最大ライフ
const float SEARCH_LENG = 2000.0f;		// 索敵範囲

const float GRAVITY = 0.5f;				// 重力

const float SHADOW_RADIUS = 400.0f;		// 影の半径
const float SHADOW_HEIGHT = 100.0f;		// 影の高さ

const int COLLISION_LENGTH = 400;		// 当たり判定
const int ATTACK_CNT = 10;				// 攻撃カウンタ
const int DAMAGE_CNT = 60;				// 被弾カウンタ
const int STATE_CHANGE = 150;			// 状態変化カウンタ
const int STATE_CHANGE_ATTACK_0 = 300;	// 状態変化カウンタ(攻撃0時)
const int STATE_CHANGE_ATTACK_1 = 450;	// 状態変化カウンタ(攻撃1時)

const int LIFE_PHASE_1 = 8000;			// ライフ段階1

const int SUMMON_MAX = 5;				// 召喚最大数

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CBossEnemy::CBossEnemy(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CBossEnemy::CBossEnemy(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEnemy(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;
	m_fCollisionLength = COLLISION_LENGTH;
	m_pBarrier = NULL;
	m_State = STATE_NORMAL;
}

/*******************************************************************************
* 関数名：CBossEnemy::~CBossEnemy()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CBossEnemy::~CBossEnemy()
{
}

/*******************************************************************************
* 関数名：void CBossEnemy::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CBossEnemy::Init(Vector3 pos)
{
	m_Pos = pos;
	m_TargetPos = m_Pos + Vector3(0.0f, 200.0f, 0.0f);
	m_Shadow = CStencilShadow::Create(m_Pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_BOSS_00, &m_MtxWorld);
	m_MotionManager->SetMotion(0);
	m_pBarrier = CBarrier::Create(m_TargetPos, 300.0f, 8, 16);
}

/*******************************************************************************
* 関数名：void CBossEnemy::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CBossEnemy::Uninit(void)
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
	m_Shadow = NULL;
	m_pBarrier->SetDeleteFlg(true);
	m_pBarrier = NULL;

	// エフェクトの生成
	CExplosion::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 500.0f, 500.0f);

	CGame *game = (CGame*)CManager::GetMode();
	game->SetState(CGame::STATE_END);
}

/*******************************************************************************
* 関数名：void CBossEnemy::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBossEnemy::Update(void)
{
	D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();

	m_Move.y -= GRAVITY;

	// 位置更新
	m_Pos += m_Move;
	// プレイヤーの高さを地面に合わせる
	float field = meshField->GetHeight(m_Pos);
	if (m_Pos.y <= field)
	{
		m_Pos.y = field;
	}
	m_TargetPos = m_Pos + Vector3(0.0f, 250.0f, 0.0f);

	// 索敵処理
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
				m_RotN.y = atan2f(m_Pos.x - pos.x, m_Pos.z - pos.z);

				m_bSearch = true;
			}
			else
			{
				m_bSearch = false;
				m_nAttCnt = 0;
				SetState(STATE_NORMAL);
			}
		}
		scene = next;
	}
#ifdef _DEBUG
	CInput *input = CManager::GetInput();
	if (input->GetKeyboardTrigger(DIK_B))
	{
		SetCharge();
		SetState(STATE_ATTACK_1);
	}
	if (input->GetKeyboardTrigger(DIK_N))
	{
		SetState(STATE_SUMMON);
	}
#endif

	// 回転角の更新
	UpdateRot();

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
* 関数名：void CBossEnemy::Uninit( void )
*
* 引数	：
* 戻り値：CBossEnemy型のポインタ
* 説明	：生成処理
*******************************************************************************/
CBossEnemy *CBossEnemy::Create(Vector3 pos)
{
	CBossEnemy *model;
	model = new CBossEnemy;
	model->Init(pos);
	return model;
}

/*******************************************************************************
* 関数名：void CBossEnemy::SetBullet(CScene *scene)
*
* 引数	：
* 戻り値：
* 説明	：弾設定処理
*******************************************************************************/
void CBossEnemy::SetBullet(CScene *scene)
{
	/* 左腕 */
	// モデルのマトリクスを受け取り、座標変換
	CModel *model = m_MotionManager->GetModel(5);
	D3DXMATRIX *matrix = model->GetMatrix();
	Vector3 pos = Vector3(250.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// ターゲットとモデルの座標からベクトルを計算し、正規化
	Vector3 target = scene->GetPosition();
	Vector3 diff = target - pos;
	diff.Normalize();
	// 弾の生成
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));

	/* 右腕 */
	// モデルのマトリクスを受け取り、座標変換
	model = m_MotionManager->GetModel(6);
	matrix = model->GetMatrix();
	pos = Vector3(-250.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// ターゲットとモデルの座標からベクトルを計算し、正規化
	target = scene->GetPosition();
	diff = target - pos;
	diff.Normalize();
	// 弾の生成
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));

	/* 左ルンバ */
	// モデルのマトリクスを受け取り、座標変換
	model = m_MotionManager->GetModel(9);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, -250.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// ターゲットとモデルの座標からベクトルを計算し、正規化
	target = scene->GetPosition();
	diff = target - pos;
	diff.Normalize();
	// 弾の生成
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));

	/* 右ルンバ */
	// モデルのマトリクスを受け取り、座標変換
	model = m_MotionManager->GetModel(10);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, -250.0f);
	D3DXVec3TransformCoord(&pos, &pos, matrix);
	// ターゲットとモデルの座標からベクトルを計算し、正規化
	target = scene->GetPosition();
	diff = target - pos;
	diff.Normalize();
	// 弾の生成
	CBossBullet::Create(pos, diff, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
}

/*******************************************************************************
* 関数名：void CEnemy::UpdateState(void)
*
* 引数	：
* 戻り値：
* 説明	：状態更新処理
*******************************************************************************/
void CBossEnemy::UpdateState(void)
{
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	// 状態カウンタの更新
	m_nStateCnt++;
	switch (m_State)
	{
	case STATE_NORMAL:
		m_MotionManager->SetMotion(0);
		// バリア(透明)をセット
		m_pBarrier->Set(m_TargetPos, 0.0f);
		if (m_nStateCnt > STATE_CHANGE)
		{
			if (m_bSearch)
			{// 索敵フラグがtrue
				SetState(STATE_ATTACK_0);
			}
			else
			{// 索敵フラグがfalse
				SetState(STATE_GUARD);
			}
		}
		break;
	case STATE_DAMAGE:
		m_MotionManager->SetMotion(0);
		if (m_nStateCnt >= DAMAGE_CNT)
		{
			SetState(STATE_NORMAL);
		}
		break;
	case STATE_ATTACK_0:
		m_MotionManager->SetMotion(1);
		m_nAttCnt++;
		if ((m_nAttCnt % ATTACK_CNT) == 0)
		{
			scene = CScene::GetList(DRAWORDER_3D);
			next = NULL;
			while (scene != NULL)
			{
				next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
				if (scene->GetObjType() == OBJTYPE_PLAYER)
				{
					// モデルのマトリクス計算
					SetModelMatrix();
					// 弾のセット
					SetBullet(scene);
				}
				scene = next;
			}
		}
		if (m_nStateCnt >= STATE_CHANGE_ATTACK_0)
		{// 規定の時間が経過したら行動変化
			if (m_nLife <= LIFE_PHASE_1)
			{// ライフが一定以下だったら
				SetCharge();
				SetState(STATE_ATTACK_1);
			}
			else
			{
				// ガード状態に移行
				SetState(STATE_GUARD);
			}
		}
		break;
	case STATE_ATTACK_1:
		m_MotionManager->SetMotion(0);
		m_nAttCnt++;
		if (m_nStateCnt == 140)
		{// レーザーの発射
			SetWorldMatrix();
			SetLazer();
			CGame *game = (CGame*)CManager::GetMode();
			CCamera *camera = game->GetCamera();
			camera->SetShake(10.0f, 150);
		}
			
		if (m_nStateCnt >= STATE_CHANGE_ATTACK_1)
		{
			// 召喚状態に移行
			SetState(STATE_SUMMON);
		}
		break;
	case STATE_GUARD:
		// ガード時
		m_MotionManager->SetMotion(0);
		// バリアのセット
		m_pBarrier->Set(m_TargetPos, 1.0f);
		if (m_nStateCnt >= STATE_CHANGE)
		{
			SetState(STATE_NORMAL);
		}
		break;
	case STATE_SUMMON:
		SetSummon();
		SetState(STATE_GUARD);
		break;
	default:
		break;
	}
}

/*******************************************************************************
* 関数名：void CBossEnemy::UpdateRot( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBossEnemy::UpdateRot(void)
{
	// 減衰係数の分岐
	float rotAtten = 0.0f;

	if (m_State == STATE_ATTACK_1)
	{
		rotAtten = ROT_ATTEN_1;
	}
	else
	{
		rotAtten = ROT_ATTEN;
	}

	// 目的の角度の補正
	CManager::CheckRot(&m_RotN.y);

	// 差分計算
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);

	// 向きを更新
	m_Rot.y += m_Rad.y * rotAtten;
	CManager::CheckRot(&m_Rot.y);
}

/*******************************************************************************
* 関数名：void CBossEnemy::SetCharge( void )
*
* 引数	：
* 戻り値：
* 説明	：エフェクト設定処理
*******************************************************************************/
void CBossEnemy::SetCharge(void)
{
	// エフェクトの生成
	CChargeEffect::Create(m_TargetPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f, NULL);

	/* 左腕 */
	// モデルのマトリクスを受け取り、座標変換
	CModel *model = m_MotionManager->GetModel(7);
	D3DXMATRIX *matrix = model->GetMatrix();
	Vector3	pos = Vector3(0.0f, 0.0f, 0.0f);
	// エフェクトの生成
	CChargeEffect::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f, matrix);

	/* 右腕 */
	// モデルのマトリクスを受け取り、座標変換
	model = m_MotionManager->GetModel(8);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, 0.0f);
	// エフェクトの生成
	CChargeEffect::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 150.0f, 150.0f, matrix);
}

/*******************************************************************************
* 関数名：void CBossEnemy::SetLazer( void )
*
* 引数	：
* 戻り値：
* 説明	：レーザー設定処理
*******************************************************************************/
void CBossEnemy::SetLazer(void)
{
	// レーザーの生成
	CLazer::Create(m_TargetPos, Vector3(-D3DX_PI * 0.8f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 80.0f, &m_MtxWorld);

	/* 左腕 */
	// モデルのマトリクスを受け取り、座標変換
	CModel *model = m_MotionManager->GetModel(9);
	D3DXMATRIX *matrix = model->GetMatrix();
	Vector3	pos = Vector3(0.0f, 0.0f, -250.0f);
	// レーザーの生成
	CLazer::Create(pos, Vector3(-D3DX_PI * 0.5f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 40.0f, matrix);

	/* 右腕 */
	// モデルのマトリクスを受け取り、座標変換
	model = m_MotionManager->GetModel(10);
	matrix = model->GetMatrix();
	pos = Vector3(0.0f, 0.0f, -250.0f);
	// レーザーの生成
	CLazer::Create(pos, Vector3(-D3DX_PI * 0.5f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f), 40.0f, matrix);
}

/*******************************************************************************
* 関数名：void CBossEnemy::SetSummon( void )
*
* 引数	：
* 戻り値：
* 説明	：召喚設定処理
*******************************************************************************/
void CBossEnemy::SetSummon(void)
{
	// 変数定義
	int cnt = 0;
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	// エネミーのカウント
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
		if (scene->GetObjType() == OBJTYPE_ENEMY)
		{
			cnt++;
		}
		scene = next;
	}
	// エネミーの召喚
	if (cnt <= 1)
	{
		Vector3 pos;
		D3DXCOLOR col = D3DXCOLOR(0.1f, 0.8f, 0.8f, 0.9f);
		
		D3DXVec3TransformCoord(&pos, &Vector3(500.0f, 250.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);

		D3DXVec3TransformCoord(&pos, &Vector3(-500.0f, 250.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);

		D3DXVec3TransformCoord(&pos, &Vector3(700.0f, 150.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);

		D3DXVec3TransformCoord(&pos, &Vector3(-700.0f, 150.0f, 0.0f), &m_MtxWorld);
		CEnemySpeed::Create(pos);
		CEffect::Create(pos, col, 100.0f, 100.0f);
	}
}