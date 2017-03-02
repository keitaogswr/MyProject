/*******************************************************************************
* タイトル名：
* ファイル名：bullet.cpp
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
* マクロ定義
*******************************************************************************/
#define WIDTH				( 10 )			// ポリゴン幅
#define HEIGHT				( 10 )			// ポリゴン高さ
#define TEXTURE_ROW			( 1 )			// テクスチャ列分割数
#define TEXTURE_COLUMN		( 1 )			// テクスチャ行分割数
#define MOVE_SPEED			( 20.0f )		// 弾の移動速度
#define MOVE_SPEED_ENEMY	( 25.0f )		// 弾の移動速度
#define MOVE_SPEED_BOSS		( 50.0f )		// 弾の移動速度
#define ROT_SPEED			( 0.01f )		// 回転速度
#define LIFE_MAX			( 120 )			// 弾の寿命
#define LIFE_MAX_ENEMY		( 50 )			// 弾の寿命(エネミー)
#define LIFE_MAX_BOSS		( 100 )			// 弾の寿命(ボス)
#define NUM_UPDATE			( 3 )			// 更新回数
#define EXPLOSION_RAND		( 50 )			// 爆発範囲
const int PLAYER_BULLET_DAMAGE = 1;			// 弾のダメージ(プレイヤー)
const int ENEMY_BULLET_DAMAGE = 20;			// 弾のダメージ(エネミー)
const int BOSS_BULLET_DAMAGE = 10;			// 弾のダメージ(ボス)
const int PLAYER_BULLET_LENGTH = 50;		// 弾のダメージ(プレイヤー)
const int ENEMY_BULLET_LENGTH = 20;			// 弾の当たり判定(エネミー)
const int BOSS_BULLET_LENGTH = 50;			// 弾の当たり判定(ボス)
const int SMOKE_UPDATE = 1;					// 煙の発生間隔
const float ROT_ANGLE = 0.05f;				// 回転角度

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CBullet::CBullet()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
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
* 関数名：CBullet::~CBullet()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CMissile::~CMissile()
{
}

/*******************************************************************************
* 関数名：CMissile::Init(Vector3 pos, Vector3 vec, int id)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CMissile::Init(Vector3 pos, Vector3 vec, int id)
{
	m_Pos = pos;
	m_Vec = vec;
	m_Vec.Normalize();
	m_nTargetId = id;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_BULLET_000);

	// 頂点バッファの生成
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

	// 頂点バッファの設定
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
* 関数名：CMissile::Update(void)
*
* 引数	：void
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CMissile::Update(void)
{
	if (m_nTargetId < 0)
	{// ターゲットしてなかったら
		return;
	}
	m_nSmokeCnt++;
	if ((m_nSmokeCnt % SMOKE_UPDATE) == 0)
	{// 煙の生成
		CSmoke::Create(m_Pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f), 70.0f, 70.0f);
	}

	// クォータニオンの更新
	UpdateQuaternion();

	// 位置更新
	m_Pos += m_Vec * MOVE_SPEED;

	// 当たり判定更新
	UpdateCollision();

	// 消去判定
	DeleteCheak();
}

/*******************************************************************************
* 関数名：void CMissile::Update(void)
*
* 引数	：void
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CMissile::Draw(void)
{
	CBillboard::Draw();
}

/*******************************************************************************
* 関数名：CMissile *CMissile::Create(Vector3 pos, Vector3 vec, int id)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CMissile *CMissile::Create(Vector3 pos, Vector3 vec, int id)
{
	CMissile *missile;
	missile = new CMissile;

	missile->Init(pos, vec, id);
	return missile;
}

/*******************************************************************************
* 関数名：CMissile::UpdateCollision(void)
*
* 引数	：
* 戻り値：
* 説明	：当たり判定更新処理
*******************************************************************************/
void CMissile::UpdateCollision(void)
{
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
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
* 関数名：CMissile::UpdateQuaternion(void)
*
* 引数	：
* 戻り値：
* 説明	：クォータニオン更新処理
*******************************************************************************/
void CMissile::UpdateQuaternion(void)
{
	Vector3 axis = Vector3(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

	/* 敵の索敵 */
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	// ターゲットしていなければ
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
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
	{// ターゲットしている敵がいなかったら更新しない
		return;
	}

	// 自分から敵へのベクトルを求める
	Vector3 vecTarget = scene->GetTargetPos() - m_Pos;

	////ベクトルAとBの長さを計算する
	float lengthA;

	// 自分の向いている向きと敵とのベクトルで外積を求め、軸のベクトルを求める
	D3DXVec3Cross(&axis, &m_Vec, &vecTarget);

	D3DXQuaternionRotationAxis(&quat, &axis, ROT_ANGLE);	// 軸から回転させるクォータニオンを求める

	D3DXQUATERNION R, Qv;
	D3DXQuaternionConjugate(&R, &quat);

	// ベクトル回転
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