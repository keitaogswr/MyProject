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
#include "effect.h"
#include "bullet.h"
#include "diffusionBullet.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 20 )			// ポリゴン幅
#define HEIGHT			( 20 )			// ポリゴン高さ
#define TEXTURE_ROW		( 1 )			// テクスチャ列分割数
#define TEXTURE_COLUMN	( 1 )			// テクスチャ行分割数
#define MOVE_SPEED		( 5.0f )		// 弾の移動速度
#define LIFE_MAX		( 50 )			// 弾の寿命
#define NUM_UPDATE		( 10 )			// 更新回数

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CDiffusionBullet::CDiffusionBullet()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CDiffusionBullet::CDiffusionBullet(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBullet(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;
}

/*******************************************************************************
* 関数名：CDiffusionBullet::~CDiffusionBullet()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CDiffusionBullet::~CDiffusionBullet()
{
}

/*******************************************************************************
* 関数名：void CDiffusionBullet::Init( Vector3 pos, Vector3 rot )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CDiffusionBullet::Init(Vector3 pos, Vector3 rot)
{
	CBullet::Init(pos, rot, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
}

/*******************************************************************************
* 関数名：void CDiffusionBullet::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CDiffusionBullet::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CDiffusionBullet::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CDiffusionBullet::Update(void)
{
	// 位置更新
	m_Pos.x += sinf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
	m_Pos.z += cosf(m_Rot.y + D3DX_PI) * MOVE_SPEED;

	// エフェクトの生成
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 30, 30);

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
		|| m_Pos.z < -depth || m_Pos.z > depth)
	{// 寿命が尽きたら破棄
		CBullet *bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x + 0.3f, m_Rot.y + 0.5f, m_Rot.z));
		bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x - 0.3f, m_Rot.y + 0.5f, m_Rot.z));
		bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x + 0.3f, m_Rot.y - 0.5f, m_Rot.z));
		bullet = CDiffusionBulletChild::Create(m_Pos, Vector3(m_Rot.x - 0.3f, m_Rot.y - 0.5f, m_Rot.z));

		Uninit();
	}
}

/*******************************************************************************
* 関数名：CDiffusionBullet *CDiffusionBullet::Create( Vector3 pos, Vector3 rot )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CDiffusionBullet *CDiffusionBullet::Create(Vector3 pos, Vector3 rot)
{
	CDiffusionBullet *bullet;
	bullet = new CDiffusionBullet;

	bullet->Init(pos, rot);
	return bullet;
}

/*******************************************************************************
* 関数名：CDiffusionBullet::CDiffusionBullet()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CDiffusionBulletChild::CDiffusionBulletChild(DRAWORDER DrawOrder, OBJTYPE ObjType) :CDiffusionBullet(DrawOrder, ObjType)
{
	m_nLife = 120;
}


/*******************************************************************************
* 関数名：void CDiffusionBulletChild::Init( Vector3 pos, Vector3 rot )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CDiffusionBulletChild::Init(Vector3 pos, Vector3 rot)
{
	CBullet::Init(pos, rot, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
}

/*******************************************************************************
* 関数名：void CDiffusionBulletChild::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CDiffusionBulletChild::Update(void)
{
	// 位置更新
	m_Pos.x += sinf(m_Rot.y + D3DX_PI) * MOVE_SPEED;
	m_Pos.y += sinf(m_Rot.x + D3DX_PI) * MOVE_SPEED;
	m_Pos.z += cosf(m_Rot.y + D3DX_PI) * MOVE_SPEED;


	// エフェクトの生成
	CEffect *effect = CEffect::Create(m_Pos, m_Col, 30, 30);

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
		|| m_Pos.z < -depth || m_Pos.z > depth)
	{// 寿命が尽きたら破棄
		Uninit();
	}
}

/*******************************************************************************
* 関数名：CDiffusionBulletChild *CDiffusionBulletChild::Create(Vector3 pos, Vector3 rot)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CDiffusionBulletChild *CDiffusionBulletChild::Create(Vector3 pos, Vector3 rot)
{
	CDiffusionBulletChild *bullet;
	bullet = new CDiffusionBulletChild;

	bullet->Init(pos, rot);
	return bullet;
}