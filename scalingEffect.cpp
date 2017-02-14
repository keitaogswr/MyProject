/*******************************************************************************
* タイトル名：
* ファイル名：scalingEffect.cpp
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
#include "scalingEffect.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 30 )		// ポリゴン幅
#define HEIGHT			( 30 )		// ポリゴン高さ
#define TEXTURE_ROW		( 1 )		// テクスチャ列分割数
#define TEXTURE_COLUMN	( 1 )		// テクスチャ行分割数
#define MOVE_SPEED		( 0.5f )	// 移動速度
#define LIFE_MAX		( 1000 )	// 寿命
#define ATTEN_DEF		( 0.0001f )	// 初期減衰係数
#define ATTEN_RISE		( 2.0f )	// 減衰係数の上昇倍率

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CScalingEffect::CScalingEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEffect(DrawOrder, ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CScalingEffect::CScalingEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEffect(DrawOrder, ObjType)
{
	m_fScalingValue = 0;
	m_Vec = Vector3(0.0f, 0.0f, 0.0f);
	m_PosOffset = Vector3(0.0f, 0.0f, 0.0f);
	m_pParentMtx = NULL;
}

/*******************************************************************************
* 関数名：CScalingEffect::~CScalingEffect()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CScalingEffect::~CScalingEffect()
{
}

/*******************************************************************************
* 関数名：void CScalingEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CScalingEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_PosOffset = pos;
	CEffect::Init(pos, col, width, height);
}

/*******************************************************************************
* 関数名：void CScalingEffect::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CScalingEffect::Update(void)
{
	// 寿命を減らす
	m_nLife--;

	// ポリゴンの大きさ更新
	m_fHeight += m_fScalingValue;
	m_fWidth += m_fScalingValue;

	// 座標計算
	m_PosOffset += m_Vec * MOVE_SPEED;

	// 座標変換
	if (m_pParentMtx)
	{
		D3DXVec3TransformCoord(&m_Pos, &m_PosOffset, m_pParentMtx);
	}

	// 頂点設定
	SetVertex();

	// 寿命チェック
	if (m_nLife <= 0 || m_fHeight < 0.1f || m_fWidth < 0.1f)
	{// 寿命が尽きたら破棄
		SetDeleteFlg(true);
	}
}

/*******************************************************************************
* 関数名：CScalingEffect *CScalingEffect::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col, float width, float height, float scale, int life)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CScalingEffect *CScalingEffect::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col, float width, float height, float scale, int life, D3DXMATRIX *matrix)
{
	CScalingEffect *effect;
	effect = new CScalingEffect;

	effect->Init(pos, col, width, height);
	effect->SetLife(life);
	effect->SetScalingValue(scale);
	effect->SetVec(vec);
	effect->SetParentMtx(matrix);

	return effect;
}

/*******************************************************************************
* 関数名：void CScalingEffect::SetWorldMatrix( void )
*
* 引数	：
* 戻り値：
* 説明	：ワールドマトリックス設定処理
*******************************************************************************/
void CScalingEffect::SetWorldMatrix(void)
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