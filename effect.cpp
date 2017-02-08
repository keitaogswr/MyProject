/*******************************************************************************
* タイトル名：
* ファイル名：effect.cpp
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
#include "input.h"
#include "effect.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 30 )		// ポリゴン幅
#define HEIGHT			( 30 )		// ポリゴン高さ
#define TEXTURE_ROW		( 1 )		// テクスチャ列分割数
#define TEXTURE_COLUMN	( 1 )		// テクスチャ行分割数
#define MOVE_SPEED		( 10.0f )	// 移動速度
#define LIFE_MAX		( 1000 )	// 寿命
#define ATTEN_DEF		( 0.0001f )	// 初期減衰係数
#define ATTEN_RISE		( 2.0f )	// 減衰係数の上昇倍率

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CEffect::CEffect()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CEffect::CEffect(DRAWORDER DrawOrder, OBJTYPE ObjType):CBillboard(DrawOrder, ObjType)
{
	m_nLife = LIFE_MAX;

	m_fWidth = WIDTH;
	m_fHeight = HEIGHT;
	m_Col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_fAtten = ATTEN_DEF;
}

/*******************************************************************************
* 関数名：CEffect::~CEffect()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CEffect::~CEffect()
{
}

/*******************************************************************************
* 関数名：void CEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_Pos = pos;
	m_Col = col;
	m_fWidth = width;
	m_fHeight = height;

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

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
								m_fHeight * 0.5f,
								0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f,
								m_fHeight * 0.5f,
								0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
								-m_fHeight * 0.5f,
								0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f,
								-m_fHeight * 0.5f,
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
* 関数名：void CEffect::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CEffect::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CEffect::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CEffect::Update(void)
{
	// 寿命を減らす
	m_nLife--;

	// ポリゴンの大きさ更新
	m_fHeight += (0.0f - m_fHeight) * m_fAtten;
	m_fWidth += (0.0f - m_fWidth) * m_fAtten;
	m_fAtten *= ATTEN_RISE;

	// 頂点設定
	SetVertex();

	// 寿命チェック
	if (m_nLife <= 0 || m_fHeight < 0.5f || m_fWidth < 0.5f)
	{// 寿命が尽きたら破棄
		SetDeleteFlg(true);
	}
}

/*******************************************************************************
* 関数名：CEffect *CEffect::Create( Vector3 pos, D3DXCOLOR col )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CEffect *CEffect::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CEffect *effect;
	effect = new CEffect;

	effect->Init(pos, col, width, height);
	return effect;
}

/*******************************************************************************
* 関数名：void CEffect::SetVertex( void )
*
* 引数	：
* 戻り値：
* 説明	：頂点設定処理
*******************************************************************************/
void CEffect::SetVertex(void)
{
	// 頂点バッファの設定
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f, m_fHeight * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f, m_fHeight * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f, -m_fHeight * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f, -m_fHeight * 0.5f, 0.0f);

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
* 関数名：void CEffect::SetRenderStateBegin( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定開始処理
*******************************************************************************/
void CEffect::SetRenderStateBegin(void)
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
* 関数名：void CEffect::SetRenderStateEnd( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定終了処理
*******************************************************************************/
void CEffect::SetRenderStateEnd(void)
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
* 関数名：void CEffect::SetWorldMatrix( void )
*
* 引数	：
* 戻り値：
* 説明	：ワールドマトリックス設定処理
*******************************************************************************/
void CEffect::SetWorldMatrix(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ビューマトリックスの逆行列を求める
	D3DXMatrixInverse(&m_MtxWorld,
		NULL,
		&mtxView);

	// マトリクスの移動に関する変数クリア
	m_MtxWorld._41 = 0.0f;
	m_MtxWorld._42 = 0.0f;
	m_MtxWorld._43 = 0.0f;

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);
}