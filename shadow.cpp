/*******************************************************************************
* タイトル名：
* ファイル名：shadow.cpp
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
#include "shadow.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )				// 幅
#define HEIGHT			( 0 )				// 高さ
#define DEPTH			( 150 )				// 奥行
#define TEXTURE_ROW		( 1 )				// テクスチャ列分割数
#define TEXTURE_COLUMN	( 1 )				// テクスチャ行分割数
#define MAGNI			( 0.005f )			// 高さによる影の倍率

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CShadow::CShadow()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CShadow::CShadow(DRAWORDER DrawOrder, OBJTYPE ObjType):CScene3D(DrawOrder, ObjType)
{
	m_fWidth = 0.0f;
	m_fDepth = 0.0f;
	m_fAlfa = 1.0f;
}

/*******************************************************************************
* 関数名：CShadow::~CShadow()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CShadow::~CShadow()
{
}

/*******************************************************************************
* 関数名：void CShadow::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CShadow::Init(Vector3 pos)
{
	m_Pos = pos;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_SHADOW_000);

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

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
		HEIGHT,
		m_fDepth * 0.5f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f,
		HEIGHT,
		m_fDepth * 0.5f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
		HEIGHT,
		-m_fDepth * 0.5f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f,
		HEIGHT,
		-m_fDepth * 0.5f);

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
* 関数名：void CShadow::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CShadow::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CShadow::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CShadow::Update(void)
{
	// 頂点情報設定
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f,	0.0f, m_fDepth * 0.5f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f, 0.0f, m_fDepth * 0.5f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f, 0.0f, -m_fDepth * 0.5f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f, 0.0f, -m_fDepth * 0.5f);

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlfa);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CShadow::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CShadow::Draw(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 各種設定 /////
	// レンダーステート設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
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

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// ストリームにバインド
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//プリミティブの種類
		0,							//最初の頂点のインデックス
		2);							//描画するプリミティブ数

	// 設定を元に戻す /////
	// レンダーステートを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/*******************************************************************************
* 関数名：CShadow *CShadow::Create( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CShadow *CShadow::Create(Vector3 pos)
{
	CShadow *shadow;
	shadow = new CShadow;

	shadow->Init(pos);
	return shadow;
}

/*******************************************************************************
* 関数名：void SetShadow(Vector3 pos,  Vector3 obj, float width, float depth)
*
* 引数	：Vector3 pos		// ポリゴンの原点位置
*		  float width		// 1ブロックの幅
*		  float depth		// 1ブロックの奥行
*
* 戻り値：
* 説明	：影設定処理
*******************************************************************************/
void CShadow::Set(Vector3 pos, Vector3 obj, float width, float depth)
{
	// 構造体に値を設定
	m_Pos = pos;
	m_fWidth = width;
	m_fDepth = depth;
	float diff = obj.y - pos.y;
	m_Scl.x = 1.0f + (diff * MAGNI);
	m_Scl.z = 1.0f + (diff * MAGNI);

	m_fAlfa = 1.0f - (diff * MAGNI);
}