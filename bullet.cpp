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
#include "orbit.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH				( 10 )			// ポリゴン幅
#define HEIGHT				( 10 )			// ポリゴン高さ
#define TEXTURE_ROW			( 1 )			// テクスチャ列分割数
#define TEXTURE_COLUMN		( 1 )			// テクスチャ行分割数
#define MOVE_SPEED			( 30.0f )		// 弾の移動速度
#define MOVE_SPEED_ENEMY	( 25.0f )		// 弾の移動速度
#define MOVE_SPEED_BOSS		( 50.0f )		// 弾の移動速度
#define LIFE_MAX			( 50 )			// 弾の寿命
#define LIFE_MAX_ENEMY		( 50 )			// 弾の寿命(エネミー)
#define LIFE_MAX_BOSS		( 100 )			// 弾の寿命(ボス)
#define NUM_UPDATE			( 3 )			// 更新回数
#define EXPLOSION_RAND		( 50 )			// 爆発範囲
const int PLAYER_BULLET_DAMAGE = 100;		// 弾のダメージ(プレイヤー)
const int ENEMY_BULLET_DAMAGE = 20;			// 弾のダメージ(エネミー)
const int BOSS_BULLET_DAMAGE = 10;			// 弾のダメージ(ボス)
const int PLAYER_BULLET_LENGTH = 50;		// 弾の当たり判定(プレイヤー)
const int ENEMY_BULLET_LENGTH = 20;			// 弾の当たり判定(エネミー)
const int BOSS_BULLET_LENGTH = 50;			// 弾の当たり判定(ボス)

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
CBullet::CBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBillboard(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;
	m_ObjType = OBJTYPE_NONE;
	m_Col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_Vec = Vector3(0.0f, 0.0f, 0.0f);
	m_Orbit[0] = m_Orbit[1] = NULL;
}

/*******************************************************************************
* 関数名：CBullet::~CBullet()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CBullet::~CBullet()
{
}

/*******************************************************************************
* 関数名：void CBullet::Init( Vector3 pos, Vector3 vec, D3DXCOLOR col )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CBullet::Init(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	m_Pos = pos;
	//m_Rot = vec;
	m_Vec = vec;
	m_Col = col;

	m_Orbit[0] = COrbit::Create(m_Pos, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	m_Orbit[1] = COrbit::Create(m_Pos, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_EFFECT_000);

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
	pVtx[3].col = m_Col;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CBullet::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CBullet::Uninit(void)
{
	// エフェクトの生成
	CExplosion::Create(m_Pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 100.0f, 100.0f);
	// 解放
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// 削除フラグ
	m_Delete = true;

	for (int i = 0; i < 2; i++)
	{
		if (m_Orbit[i] != NULL)
		{
			m_Orbit[i]->SetDeleteFlg(true);
		}
	}
}

/*******************************************************************************
* 関数名：void CBullet::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBullet::Update(void)
{
	m_Pos += m_Vec * MOVE_SPEED;

	/* 変数定義 */
	D3DXMATRIX mtxRot, mtxTrans;				// 向き、ポジション

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	m_Orbit[0]->Set(Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f, -10.0f, 0.0f), m_MtxWorld);
	m_Orbit[1]->Set(Vector3(10.0f, 0.0f, 0.0f), Vector3(-10.0f, 0.0f, 0.0f), m_MtxWorld);

	// 消去判定
	DeleteCheak();
	
	// 当たり判定
	UpdateCollision();
}

/*******************************************************************************
* 関数名：CBullet *CBullet::Create( Vector3 pos, Vector3 vec )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CBullet *CBullet::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	CBullet *bullet;
	bullet = new CBullet;

	bullet->Init(pos, vec, col);
	return bullet;
}

/*******************************************************************************
* 関数名：void CBullet::UpdateCollision( void )
*
* 引数	：
* 戻り値：
* 説明	：当たり判定処理
*******************************************************************************/
void CBullet::UpdateCollision(void)
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
* 関数名：void CBullet::DeleteCheak(void)
*
* 引数	：
* 戻り値：
* 説明	：消去判定処理
*******************************************************************************/
void CBullet::DeleteCheak(void)
{
	// フィールドの取得
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();

	float width, depth;
	width = mesh->GetWidthAll() * 0.5f;
	depth = mesh->GetDepthAll() * 0.5f;

	// 寿命を減らす
	m_nLife--;
	// 寿命チェック
	if (m_nLife <= 0
		|| m_Pos.x < -width || m_Pos.x > width
		|| m_Pos.z < -depth || m_Pos.z > depth
		|| mesh->GetHeight(m_Pos) > m_Pos.y)
	{// 寿命が尽きたら、またはフィールドの外に出たら、もしくはフィールドの中に入ったら破棄
		Uninit();
	}
}

/*******************************************************************************
* 関数名：void CBillboard::SetRenderStateBegin( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定開始処理
*******************************************************************************/
void CBullet::SetRenderStateBegin(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 各種設定 /////
	// ライトのOFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// フォグのOFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// ZテストのOFF
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// アルファテストのON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	// 加算合成によるアルファブレンドのレンダーステートの設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

/*******************************************************************************
* 関数名：void CBillboard::SetRenderStateEnd( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定終了処理
*******************************************************************************/
void CBullet::SetRenderStateEnd(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 設定を元に戻す /////
	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// フォグのON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// ZテストのOF
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// 元のアルファブレンドの設定に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/*******************************************************************************
* 関数名：CEnemyBullet::CBossBullet()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CEnemyBullet::CEnemyBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX_ENEMY;
}

/*******************************************************************************
* 関数名：void CEnemyBullet::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CEnemyBullet::Update(void)
{
	// エフェクトの生成
	CEffect *effect = CEffect::Create(m_Pos, m_Col, (float)ENEMY_BULLET_LENGTH, (float)ENEMY_BULLET_LENGTH);
	m_Pos += m_Vec * MOVE_SPEED_ENEMY;
	// フィールドの取得
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();

	float width, depth;
	width = mesh->GetWidthAll() * 0.5f;
	depth = mesh->GetDepthAll() * 0.5f;

	// 寿命を減らす
	m_nLife--;
	// 寿命チェック
	if (m_nLife <= 0
		|| m_Pos.x < -width || m_Pos.x > width
		|| m_Pos.z < -depth || m_Pos.z > depth
		|| mesh->GetHeight(m_Pos) > m_Pos.y)
	{// 寿命が尽きたら、またはフィールドの外に出たら、もしくはフィールドの中に入ったら破棄
		Uninit();
	}
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
		if (scene->GetObjType() == OBJTYPE_PLAYER)
		{
			Vector3 pos = scene->GetPosition();
			Vector3 diff = pos - m_Pos;
			float length = diff.Length();
			if (length < ENEMY_BULLET_LENGTH)
			{
				CPlayer *player = (CPlayer*)scene;
				player->SetDamage(ENEMY_BULLET_DAMAGE);
				player->SetKeep(ENEMY_BULLET_DAMAGE);
				CGame *game = (CGame*)CManager::GetMode();
				CCamera *camera = game->GetCamera();
				camera->SetShake(30.0f, 10);
				Uninit();
			}
		}
		scene = next;
	}
}

/*******************************************************************************
* 関数名：CEnemyBullet *CEnemyBullet::Create( Vector3 pos, Vector3 vec )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CEnemyBullet *CEnemyBullet::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	CEnemyBullet *bullet;
	bullet = new CEnemyBullet;

	bullet->Init(pos, vec, col);
	return bullet;
}


/*******************************************************************************
* 関数名：CBossBullet::CBossBullet()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CBossBullet::CBossBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX_BOSS;
}

/*******************************************************************************
* 関数名：void CBossBullet::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBossBullet::Update(void)
{
	// エフェクトの生成
	CEffect *effect = CEffect::Create(m_Pos, m_Col, (float)BOSS_BULLET_LENGTH, (float)BOSS_BULLET_LENGTH);
	m_Pos += m_Vec * MOVE_SPEED_BOSS;
	// フィールドの取得
	CGame *game = (CGame*)CManager::GetMode();
	CMeshField *mesh = game->GetMeshField();

	float width, depth;
	width = mesh->GetWidthAll() * 0.5f;
	depth = mesh->GetDepthAll() * 0.5f;

	// 寿命を減らす
	m_nLife--;
	// 寿命チェック
	if (m_nLife <= 0
		|| m_Pos.x < -width || m_Pos.x > width
		|| m_Pos.z < -depth || m_Pos.z > depth
		|| mesh->GetHeight(m_Pos) > m_Pos.y)
	{// 寿命が尽きたら、またはフィールドの外に出たら、もしくはフィールドの中に入ったら破棄
		Uninit();
	}
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	while (scene != NULL)
	{
		next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
		if (scene->GetObjType() == OBJTYPE_PLAYER)
		{
			Vector3 pos = scene->GetPosition();
			Vector3 diff = pos - m_Pos;
			float length = diff.Length();
			if (length < BOSS_BULLET_LENGTH)
			{
				CPlayer *player = (CPlayer*)scene;
				player->SetDamage(BOSS_BULLET_DAMAGE);
				CGame *game = (CGame*)CManager::GetMode();
				CCamera *camera = game->GetCamera();
				camera->SetShake(30.0f, 10);
				Uninit();
			}
		}
		scene = next;
	}
}

/*******************************************************************************
* 関数名：CBossBullet *CBossBullet::Create( Vector3 pos, Vector3 vec )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CBossBullet *CBossBullet::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col)
{
	CBossBullet *bullet;
	bullet = new CBossBullet;

	bullet->Init(pos, vec, col);
	return bullet;
}