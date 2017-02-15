/*******************************************************************************
* タイトル名：
* ファイル名：stencilShadow.cpp
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
#include "stencilShadow.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const float MAGNI = 0.005f;			// 高さによる影の倍率

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CStencilShadow::CStencilShadow(DRAWORDER DrawOrder, OBJTYPE ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CStencilShadow::CStencilShadow(DRAWORDER DrawOrder, OBJTYPE ObjType):CScene(DrawOrder, ObjType)
{
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_pVtxBuff2D = NULL;
	m_pVtxBuffFan[0] = NULL;
	m_pVtxBuffFan[1] = NULL;
	m_Scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	m_fRadius = 0.0f;
	m_fRadiusN = 0.0f;
	m_fHeight = 0.0f;
	m_nRowBlock = 0;
	m_nColumnBlock = 0;
	m_nRowNumVtx = 0;
	m_nColumnNumVtx = 0;
	m_nNumVtxMax = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;

	m_fAlfa = 0.7f;
}

/*******************************************************************************
* 関数名：CStencilShadow::~CStencilShadow()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CStencilShadow::~CStencilShadow()
{
}

/*******************************************************************************
* 関数名：void CStencilShadow::Init(Vector3 pos)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CStencilShadow::Init(Vector3 pos)
{
	m_Pos = pos;
	m_nRowBlock = 1;
	m_nColumnBlock = 16;
	m_nRowNumVtx = m_nRowBlock + 1;
	m_nColumnNumVtx = m_nColumnBlock + 1;
	m_fRadius = m_fRadiusN = 50.0f;
	m_fHeight = 100.0f;
	m_nNumVtxMax = m_nRowNumVtx * m_nColumnNumVtx;

	m_nNumIdx = m_nColumnNumVtx * m_nRowBlock * 2 + (m_nRowBlock - 1) * 2;
	m_nNumPrim = (m_nColumnBlock * m_nRowBlock * 2) + (m_nRowBlock - 1) * 4;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock * 0.5f - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
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

	// ファン部分 ///
	// 頂点バッファの確保
	for(int i = 0; i < 2; i++)
	{
		if (FAILED(pDevice->CreateVertexBuffer(
			sizeof(VERTEX_3D) * (m_nColumnNumVtx + 1),
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&m_pVtxBuffFan[i],
			NULL)))
		{
			return;
		}
	}

	// 頂点バッファの設定
	m_pVtxBuffFan[0]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * 0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = 1; nCntColumn < m_nColumnNumVtx; nCntColumn++)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * 0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[0]->Unlock();

	m_pVtxBuffFan[1]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * -0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = m_nColumnNumVtx; nCntColumn >= 1; nCntColumn--)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * -0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[1]->Unlock();

	// 全画面2Dポリゴン
	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff2D,
		NULL)))
	{
		return;
	}

	// 頂点バッファの設定
	VERTEX_2D *pVtx2D;

	m_pVtxBuff2D->Lock(0, 0, (void**)&pVtx2D, 0);

	pVtx2D[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx2D[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx2D[2].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx2D[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	pVtx2D[0].rhw =
	pVtx2D[1].rhw =
	pVtx2D[2].rhw =
	pVtx2D[3].rhw = 1.0f;

	pVtx2D[0].col =
	pVtx2D[1].col =
	pVtx2D[2].col =
	pVtx2D[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_fAlfa);

	m_pVtxBuff2D->Unlock();
}

/*******************************************************************************
* 関数名：void CStencilShadow::Uninit(void)
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CStencilShadow::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pVtxBuff2D);
	SAFE_RELEASE(m_pVtxBuffFan[0]);
	SAFE_RELEASE(m_pVtxBuffFan[1]);
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CStencilShadow::Update(void)
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CStencilShadow::Update(void)
{
	m_fRadius += (m_fRadiusN - m_fRadius) * 0.5f;
	SetVertex();
}

/*******************************************************************************
* 関数名：void CStencilShadow::Draw(void)
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CStencilShadow::Draw(void)
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

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// 各種設定 /////
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Zバッファ
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);				// ステンシルバッファ
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);				// バックバッファ
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILREF, 1);

	// 円柱(表)を+1で描画 ///
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
	// ファンを描画 ///
	for (int i = 0; i < 2; i++)
	{
		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);
		// データストリームにバインドする
		pDevice->SetStreamSource(0, m_pVtxBuffFan[i], 0, sizeof(VERTEX_3D));
		// ポリゴンの描画
		pDevice->DrawPrimitive(
			D3DPT_TRIANGLEFAN,			// プリミティブの種類
			0,							// 最初の頂点のインデックス
			m_nColumnBlock);			// 描画するプリミティブ数
	}
	
	// 円柱(裏)を-1で描画 ///
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
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

	// ファンを描画 ///
	for (int i = 0; i < 2; i++)
	{
		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);
		// データストリームにバインドする
		pDevice->SetStreamSource(0, m_pVtxBuffFan[i], 0, sizeof(VERTEX_3D));
		// ポリゴンの描画
		pDevice->DrawPrimitive(
			D3DPT_TRIANGLEFAN,			// プリミティブの種類
			0,							// 最初の頂点インデックス
			m_nColumnBlock);			// 描画するプリミティブ数
	}

	// 各種設定 /////
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);			// バックバッファ
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);					// Zバッファ

	// 全画面2Dポリゴン描画 ///
	// ストリームにバインド
	pDevice->SetStreamSource(0, m_pVtxBuff2D, 0, sizeof(VERTEX_2D));
	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);
	// ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//プリミティブの種類
		0,							//最初の頂点のインデックス
		2);							//描画するプリミティブ数

	// 各種設定 /////
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);					// Zバッファ
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);			// ステンシルバッファ
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ
}

/*******************************************************************************
* 関数名：CStencilShadow *CStencilShadow::Create(Vector3 pos)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CStencilShadow *CStencilShadow::Create(Vector3 pos)
{
	CStencilShadow *stencilShadow;
	stencilShadow = new CStencilShadow;
	stencilShadow->Init(pos);
	return stencilShadow;
}

/*******************************************************************************
* 関数名：void SetShadow(Vector3 pos, Vector3 obj, float radius, float heigh)
*
* 引数	：Vector3 pos		// ポリゴンの原点位置
*		　Vector3 obj		// オブジェクトの位置
*		　float radius		// 影の半径
*		　float height		// シリンダーの高さ
* 戻り値：
* 説明	：影設定処理
*******************************************************************************/
void CStencilShadow::Set(Vector3 pos, Vector3 obj, float radius, float height)
{
	// 構造体に値を設定
	m_Pos = pos;
	m_fRadiusN = radius;
	m_fHeight = height;
	float diff = obj.y - pos.y;
	m_Scl.x = 1.0f + (diff * MAGNI);
	m_Scl.z = 1.0f + (diff * MAGNI);

	m_fAlfa = 0.7f - (diff * MAGNI);
}

/*******************************************************************************
* 関数名：void SetVertex(void)
*
* 引数	：
* 戻り値：
* 説明	：影頂点設定処理
*******************************************************************************/
void CStencilShadow::SetVertex(void)
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
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock * 0.5f - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
		}
	}

	m_pVtxBuff->Unlock();
	// ファン部分
	m_pVtxBuffFan[0]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * 0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = 1; nCntColumn <= m_nColumnNumVtx; nCntColumn++)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * 0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[0]->Unlock();

	m_pVtxBuffFan[1]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * -0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = m_nColumnNumVtx; nCntColumn >= 1; nCntColumn--)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * -0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[1]->Unlock();

	// 頂点バッファの設定
	VERTEX_2D *pVtx2D;

	m_pVtxBuff2D->Lock(0, 0, (void**)&pVtx2D, 0);


	pVtx2D[0].col =
	pVtx2D[1].col =
	pVtx2D[2].col =
	pVtx2D[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_fAlfa);

	m_pVtxBuff2D->Unlock();
}