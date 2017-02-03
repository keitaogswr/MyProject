/*******************************************************************************
* タイトル名：
* ファイル名：animationBoard.cpp
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
#include "animationBoard.h"
#include "chargeEffect.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const float	WIDTH = 30.0f;			// ポリゴン幅
const float	HEIGHT = 30.0f;			// ポリゴン高さ
const int	TEXTURE_ROW = 1;		// テクスチャ列分割数
const int	TEXTURE_COLUMN = 10;	// テクスチャ行分割数
const float	MOVE_SPEED = 10.0f;		// 移動速度
const int	LIFE_MAX = 1000;		// 寿命
const float	ATTEN_DEF = 0.0001f;	// 初期減衰係数
const float	ATTEN_RISE = 2.0f;		// 減衰係数の上昇倍率
const int	DRAW_SPEED = 5;			// 描画速度
const int	LOOP_MAX = 3;			// ループ回数

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CChargeEffect::CChargeEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) : CAnimationBoard(DrawOrder, ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CChargeEffect::CChargeEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) : CAnimationBoard(DrawOrder, ObjType)
{
	m_nTexColumn = TEXTURE_COLUMN;
	m_nTexRow = TEXTURE_ROW;
	m_pParentMtx = NULL;
	m_PosOffset = Vector3(0.0f, 0.0f, 0.0f);
}

/*******************************************************************************
* 関数名：CChargeEffect::~CChargeEffect()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CChargeEffect::~CChargeEffect()
{
}

/*******************************************************************************
* 関数名：void CChargeEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CChargeEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height, D3DXMATRIX *matrix)
{
	m_Pos = pos;
	m_PosOffset = pos;
	m_Col = col;
	m_fWidth = width;
	m_fHeight = height;
	m_pParentMtx = matrix;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_CHARGE_000);

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
	SetVertex();
}

/*******************************************************************************
* 関数名：void CChargeEffect::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CChargeEffect::Update(void)
{
	// 座標変換
	if (m_pParentMtx)
	{
		D3DXVec3TransformCoord(&m_Pos, &m_PosOffset, m_pParentMtx);
	}
	
	// パターン描画更新
	m_nCntAnim++;
	if ((m_nCntAnim % DRAW_SPEED) == 0)
	{
		m_nPatternAnim++;
		if ((m_nPatternAnim % TEXTURE_COLUMN) == 0)
		{
			m_nRowAnim++;
			if ((m_nRowAnim % TEXTURE_ROW) == 0)
			{
				m_nLoopCnt++;
			}
		}
		// 頂点設定
		SetVertex();
	}
	if (m_nLoopCnt >= LOOP_MAX)
	{
		SetDeleteFlg(true);
	}
}

/*******************************************************************************
* 関数名：CChargeEffect *CChargeEffect::Create(Vector3 pos, D3DXCOLOR col, float width, float height, D3DXMATRIX matrix)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CChargeEffect *CChargeEffect::Create(Vector3 pos, D3DXCOLOR col, float width, float height, D3DXMATRIX *matrix)
{
	CChargeEffect *effect;
	effect = new CChargeEffect;

	effect->Init(pos, col, width, height, matrix);
	return effect;
}

/*******************************************************************************
* 関数名：void CChargeEffect::SetRenderStateBegin( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定開始処理
*******************************************************************************/
void CChargeEffect::SetRenderStateBegin(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ライトのOFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// フォグのOFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// ZテストのOFF
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// アルファテストのON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
	// 加算合成によるアルファブレンドのレンダーステートの設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

/*******************************************************************************
* 関数名：void CChargeEffect::SetRenderStateEnd( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定終了処理
*******************************************************************************/
void CChargeEffect::SetRenderStateEnd(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// フォグのON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// ZテストのON
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// 元のアルファブレンドの設定に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/*******************************************************************************
* 関数名：void CMeshCylinder::SetWorldMatrix( void )
*
* 引数	：
* 戻り値：
* 説明	：ワールドマトリックス設定処理
*******************************************************************************/
void CChargeEffect::SetWorldMatrix(void)
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