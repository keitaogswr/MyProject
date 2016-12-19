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

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH				( 10 )			// ポリゴン幅
#define HEIGHT				( 10 )			// ポリゴン高さ
#define TEXTURE_ROW			( 1 )			// テクスチャ列分割数
#define TEXTURE_COLUMN		( 1 )			// テクスチャ行分割数
#define MOVE_SPEED			( 10.0f )		// 弾の移動速度
#define MOVE_SPEED_ENEMY	( 25.0f )		// 弾の移動速度
#define MOVE_SPEED_BOSS		( 40.0f )		// 弾の移動速度
#define LIFE_MAX			( 50 )			// 弾の寿命
#define LIFE_MAX_ENEMY		( 50 )			// 弾の寿命(エネミー)
#define LIFE_MAX_BOSS		( 100 )			// 弾の寿命(ボス)
#define NUM_UPDATE			( 3 )			// 更新回数
#define EXPLOSION_RAND		( 50 )			// 爆発範囲
const int PLAYER_BULLET_DAMAGE = 100;		// 弾のダメージ(プレイヤー)
const int ENEMY_BULLET_DAMAGE = 20;			// 弾のダメージ(エネミー)
const int BOSS_BULLET_DAMAGE = 30;			// 弾のダメージ(ボス)

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
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

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
	// エフェクトの生成
	for (int i = 0; i < NUM_UPDATE; i++)
	{
		// 位置更新
		//m_Pos.x += sinf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
		//m_Pos.z += cosf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
		m_Pos += m_Vec * MOVE_SPEED;
		CEffect *effect = CEffect::Create(m_Pos, m_Col, 30, 30);
	}

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
* 関数名：void CBullet::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CBullet::Draw(void)
{

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
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 20, 20);
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
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 100, 100);
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