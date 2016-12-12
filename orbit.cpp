/*******************************************************************************
* タイトル名：
* ファイル名：orbit.cpp
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
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "input.h"
#include "texture.h"
#include "orbit.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：COrbit::COrbit()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
COrbit::COrbit(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene3D(DrawOrder, ObjType)
{
	for (int i = 0; i < ORBIT_VERTEX; i++)
	{
		m_OldPos[i] = Vector3(0.0f, 0.0f, 0.0f);
	}
	m_ParentPos = Vector3(0.0f, 0.0f, 0.0f);
	m_HeadPos = Vector3(0.0f, 0.0f, 0.0f);
}

/*******************************************************************************
* 関数名：COrbit::~COrbit()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
COrbit::~COrbit()
{
}

/*******************************************************************************
* 関数名：void COrbit::Init(void)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void COrbit::Init(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 頂点バッファの確保
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * ORBIT_VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	SetVertex();
}

/*******************************************************************************
* 関数名：void COrbit::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void COrbit::Uninit(void)
{
	// 解放処理
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void COrbit::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void COrbit::Update(void)
{
	for (int i = 0; i < ORBIT_VERTEX * 0.5f - 1; i++)
	{// 過去のデータを保存
		m_OldPos[i * 2] = m_OldPos[(i + 1) * 2];
		m_OldPos[i * 2 + 1] = m_OldPos[(i + 1) * 2 + 1];
	}

	// 座標変換
	D3DXVec3TransformCoord(&m_ParentPos,
		&m_ParentPos,
		&m_ParentMtx);
	m_OldPos[ORBIT_VERTEX - 1] = m_ParentPos;
	// 座標変換
	D3DXVec3TransformCoord(&m_HeadPos,
		&m_HeadPos,
		&m_ParentMtx);
	m_OldPos[ORBIT_VERTEX - 2] = m_HeadPos;

	SetVertex();
}
/*******************************************************************************
* 関数名：void COrbit::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void COrbit::Draw(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

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
	// カリング設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);
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
	pDevice->SetTexture(0, NULL);

	//ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//プリミティブの種類
		0,							//最初の頂点のインデックス
		ORBIT_VERTEX - 2);			//描画するプリミティブ数

	// 設定を元に戻す /////
	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// フォグのON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// ZテストのON
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// 元のアルファブレンドの設定に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/*******************************************************************************
* 関数名：COrbit *COrbit::Create(void)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
COrbit *COrbit::Create(void)
{
	COrbit *meshField;
	meshField = new COrbit;
	meshField->Init();
	return meshField;
}

/*******************************************************************************
* 関数名：void COrbit::SetVertex( void )
*
* 引数	：
* 戻り値：
* 説明	：頂点設定処理
*******************************************************************************/
void COrbit::SetVertex(void)
{
	// 頂点情報設定
	VERTEX_3D *pVtx;
	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < ORBIT_VERTEX; i++)
	{
		pVtx[i].pos = m_OldPos[i];
		pVtx[i].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[i].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f / ORBIT_VERTEX * i);
	}

	m_VtxBuff->Unlock();
}