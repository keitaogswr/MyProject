/*******************************************************************************
* タイトル名：
* ファイル名：meshCylinder.cpp
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
#include "scene.h"
#include "scene3D.h"
#include "meshCylinder.h"
#include "texture.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const float MAGNI = 0.005f;			// 高さによる影の倍率
const float RADIUS_ATTEN = 0.05f;	// 半径の慣性減衰係数
const float HEIGHT = 5000.0f;		// 高さ
const float HEIGHT_ATTEN = 0.1f;	// 高さの慣性減衰係数
const float ROT_ATTEN = 0.05f;		// 回転の慣性減衰係数

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CMeshCylinder::CMeshCylinder(DRAWORDER DrawOrder, OBJTYPE ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CMeshCylinder::CMeshCylinder(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene3D(DrawOrder, ObjType)
{
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_Scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	m_fRadius = 0.0f;
	m_fRadiusN = 0.0f;
	m_fHeight = 0.0f;
	m_fHeightN = 0.0f;
	m_nRowBlock = 0;
	m_nColumnBlock = 0;
	m_nRowNumVtx = 0;
	m_nColumnNumVtx = 0;
	m_nNumVtxMax = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;
	m_pParentMtx = NULL;
}

/*******************************************************************************
* 関数名：CMeshCylinder::~CMeshCylinder()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CMeshCylinder::~CMeshCylinder()
{
}

/*******************************************************************************
* 関数名：void CMeshCylinder::Init(Vector3 pos, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CMeshCylinder::Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
{
	m_Pos = pos;
	m_Rot = rot;
	m_pParentMtx = matrix;
	m_nRowBlock = 1;
	m_nColumnBlock = 16;
	m_nRowNumVtx = m_nRowBlock + 1;
	m_nColumnNumVtx = m_nColumnBlock + 1;
	m_fRadiusN = radius;
	m_fHeightN = HEIGHT;
	m_nNumVtxMax = m_nRowNumVtx * m_nColumnNumVtx;

	m_nNumIdx = m_nColumnNumVtx * m_nRowBlock * 2 + (m_nRowBlock - 1) * 2;
	m_nNumPrim = (m_nColumnBlock * m_nRowBlock * 2) + (m_nRowBlock - 1) * 4;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_LAZER_000);

	// 頂点バッファの設定
	VERTEX_3D *pVtx;

	// 頂点バッファの確保
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * m_nNumVtxMax,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL)))
	{
		return;
	}

	// インデックスバッファの確保
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,		// 確保するバッファサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	int nIdxCnt = 0;
	for (int nCntRow = 0; nCntRow < m_nRowNumVtx; nCntRow++)
	{
		for (int nCntColumn = m_nColumnNumVtx - 1; nCntColumn >= 0; nCntColumn--, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn),
											0.0f,
											cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = col;

			// テクスチャブロック表示
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// テクスチャ全面表示
			pVtx[nIdxCnt].tex.x = (1.0f / m_nColumnBlock) * nCntColumn;
			pVtx[nIdxCnt].tex.y = (1.0f / m_nRowBlock) * (m_nRowNumVtx - 1 - nCntRow);
		}
	}

	m_pVtxBuff->Unlock();


	// インデックス情報設定
	WORD *pIdx;

	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 変数定義
	int x, y, cnt;

	for (y = 0, cnt = 0; y < m_nRowBlock; y++)
	{
		for (x = 0; x < m_nColumnNumVtx; x++, cnt++, pIdx += 2)
		{
			pIdx[0] = x + (y + 1) * m_nColumnNumVtx;
			pIdx[1] = x + (y + 0) * m_nColumnNumVtx;
			if (x == m_nColumnNumVtx - 1 && cnt * 2 < m_nNumIdx - 2)
			{
				cnt++;
				pIdx += 2;
				pIdx[0] = x + (y + 1) * m_nColumnNumVtx;
				pIdx[1] = (y + 1 + 1) * m_nColumnNumVtx;
			}
		}
	}

	m_pIdxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CMeshCylinder::Uninit(void)
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CMeshCylinder::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_pVtxBuff);
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CMeshCylinder::Update(void)
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CMeshCylinder::Update(void)
{
	m_fRadius += (m_fRadiusN - m_fRadius) * RADIUS_ATTEN;
	m_fHeight += (m_fHeightN - m_fHeight) * HEIGHT_ATTEN;
	m_Rot.x += (-D3DX_PI * 0.55f - m_Rot.x) * ROT_ATTEN;
	//m_Rot.x += 0.1f;
	SetVertex();
}

/*******************************************************************************
* 関数名：void CMeshCylinder::Draw(void)
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CMeshCylinder::Draw(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// レンダーステートの設定
	SetRenderStateBegin();

	// ワールドマトリックスを設定
	SetWorldMatrix();

	// ワールドマトリックスをバインド
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);
	// データストリームにバインドする
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));
	// インデックスバッファをバインド
	pDevice->SetIndices(m_pIdxBuff);
	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,		// プリミティブの種類
		0,
		0,
		m_nNumVtxMax,				// 頂点数
		0,
		m_nNumPrim);				// 描画するプリミティブ数

	// レンダーステートの設定
	SetRenderStateEnd();
}

/*******************************************************************************
* 関数名：CMeshCylinder *CMeshCylinder::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CMeshCylinder *CMeshCylinder::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
{
	CMeshCylinder *cylinder;
	cylinder = new CMeshCylinder;
	cylinder->Init(pos, rot, col, radius, matrix);
	return cylinder;
}

/*******************************************************************************
* 関数名：void CMeshCylinder::SetVertex(void)
*
* 引数	：
* 戻り値：
* 説明	：頂点設定処理
*******************************************************************************/
void CMeshCylinder::SetVertex(void)
{
	// 頂点バッファの設定
	VERTEX_3D *pVtx;
	// シリンダー部分
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++)
	{
		for (int nCntColumn = m_nColumnNumVtx - 1; nCntColumn >= 0; nCntColumn--, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
		}
	}

	m_pVtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CMeshCylinder::SetRenderStateBegin( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定開始処理
*******************************************************************************/
void CMeshCylinder::SetRenderStateBegin(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ライトのOFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// フォグのOFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// カリングなし
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// ZテストのOFF
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// アルファテストのON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
	// 加算合成によるアルファブレンドのレンダーステートの設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

/*******************************************************************************
* 関数名：void CMeshCylinder::SetRenderStateEnd( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定終了処理
*******************************************************************************/
void CMeshCylinder::SetRenderStateEnd(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// フォグのON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// カリングを元に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// ZテストのON
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// 元のアルファブレンドの設定に戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
void CMeshCylinder::SetWorldMatrix(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl,
		m_Scl.x,
		m_Scl.y,
		m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_Rot.y,
		m_Rot.x,
		m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_Pos.x,
		m_Pos.y,
		m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	if (m_pParentMtx)
	{
		// 親のマトリクスと掛け合わせる
		D3DXMatrixMultiply(&m_MtxWorld,
			&m_MtxWorld,
			m_pParentMtx);
	}
}