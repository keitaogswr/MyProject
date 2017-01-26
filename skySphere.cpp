/*******************************************************************************
* タイトル名：
* ファイル名：skyDome.cpp
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
#include "renderer.h"
#include "scene.h"
#include "scene3D.h"
#include "skyDome.h"
#include "input.h"
#include "camera.h"
#include "skySphere.h"
#include "texture.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MESHDOME_ANGLE	( 80.0f )			// ドーム部分の角度
#define ROT_SPEED		( 0.01f )			// 

/*******************************************************************************
* 関数名：CSkySphere::CSkySphere()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CSkySphere::CSkySphere(DRAWORDER DrawOrder, OBJTYPE ObjType) :CSkyDome(DrawOrder, ObjType)
{
	ZeroMemory(&m_MeshFan, sizeof(m_MeshFan));
	ZeroMemory(&m_MeshDome, sizeof(m_MeshDome));
}

/*******************************************************************************
* 関数名：CSkySphere::~CSkySphere()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CSkySphere::~CSkySphere()
{
}

/*******************************************************************************
* 関数名：void CSkySphere::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CSkySphere::Init(Vector3 pos, float rad, int row, int column)
{
	m_Pos = pos;

	// ドーム部分
	m_MeshDome.nRowBlock = row;
	m_MeshDome.nColumnBlock = column;
	m_MeshDome.nRowNumVtx = m_MeshDome.nRowBlock + 1;
	m_MeshDome.nColumnNumVtx = m_MeshDome.nColumnBlock + 1;
	m_MeshDome.fRadius = rad;
	m_MeshDome.nNumVtxMax = m_MeshDome.nRowNumVtx * m_MeshDome.nColumnNumVtx;
	m_MeshDome.nNumIdx = (m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2) + (m_MeshDome.nRowBlock - 1) * 2;
	m_MeshDome.nNumPrim = (m_MeshDome.nColumnBlock * m_MeshDome.nRowBlock * 2) + (m_MeshDome.nRowBlock - 1) * 4;

	// ファン部分
	m_MeshFan[0].nNumVtxMax = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[0].nNumIdx = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[0].nNumPrim = m_MeshDome.nColumnBlock;
	m_MeshFan[1].nNumVtxMax = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[1].nNumIdx = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[1].nNumPrim = m_MeshDome.nColumnBlock;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_SKY_000);

	// 頂点バッファの確保 /////
	// ドーム部分
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * m_MeshDome.nNumVtxMax,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_MeshDome.pVtxBuff,
		NULL)))
	{
		return;
	}
	// ファン部分
	for (int i = 0; i < 2; i++)
	{
		if (FAILED(pDevice->CreateVertexBuffer(
			sizeof(VERTEX_3D) * m_MeshFan[i].nNumVtxMax,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&m_MeshFan[i].pVtxBuff,
			NULL)))
		{
			return;
		}

	}
	
	// インデックスバッファの確保 /////
	// ドーム部分
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_MeshDome.nNumIdx,		// 確保するバッファサイズ
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_MeshDome.pIdxBuff,
		NULL);
	// ファン部分
	for (int i = 0; i < 2; i++)
	{
		pDevice->CreateIndexBuffer(sizeof(WORD) * m_MeshFan[i].nNumIdx,		// 確保するバッファサイズ
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_MeshFan[i].pIdxBuff,
			NULL);
	}

	// 頂点バッファの設定
	VERTEX_3D *pVtx;
	int nIdxCnt = 0;
	int rowVtxMax;
	/// ドーム部分
	m_MeshDome.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntRow = (m_MeshDome.nRowNumVtx - 1) / 2; nCntRow >= 0 ; nCntRow--)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
											sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow),
											cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// テクスチャブロック表示
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// テクスチャ全面表示
			pVtx[nIdxCnt].tex.x = (1.0f / m_MeshDome.nColumnBlock) * nCntColumn;
			pVtx[nIdxCnt].tex.y = (1.0f / m_MeshDome.nRowBlock * 0.5f) * (m_MeshDome.nRowNumVtx - 1 - nCntRow);
		}
	}
	rowVtxMax = (m_MeshDome.nRowNumVtx - 1) / 2 + 1;
	for (int nCntRow = 1; nCntRow < rowVtxMax; nCntRow++)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) * -m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
											sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow),
											cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// テクスチャブロック表示
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// テクスチャ全面表示
			pVtx[nIdxCnt].tex.x = (1.0f / m_MeshDome.nColumnBlock) * nCntColumn;
			pVtx[nIdxCnt].tex.y = (1.0f / m_MeshDome.nRowBlock * 0.5f) * nCntRow + 0.5f;
		}
	}

	m_MeshDome.pVtxBuff->Unlock();

	/// ファン部分
	m_MeshFan[0].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_MeshDome.fRadius;
	pVtx[0].pos.z = 0.0f;

	pVtx[0].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 0.0f;

	for (int i = 1, nIdxCnt = 0; i < m_MeshFan[0].nNumVtxMax; i++)
	{
		pVtx[i].pos.x = cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i) * m_MeshDome.fRadius;

		pVtx[i].pos.y = sinf(MESHDOME_ANGLE * RAD) * m_MeshDome.fRadius;
		pVtx[i].pos.z = cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i) * m_MeshDome.fRadius;

		pVtx[i].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i),
			sinf(MESHDOME_ANGLE * RAD),
			cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i));
		pVtx[i].col = D3DCOLOR_RGBA(255, 255, 255, 255);

		pVtx[i].tex.x = 0.0f;
		pVtx[i].tex.y = 0.1f;
	}

	m_MeshFan[0].pVtxBuff->Unlock();

	m_MeshFan[1].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = -m_MeshDome.fRadius;
	pVtx[0].pos.z = 0.0f;

	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 1.0f;

	for (int i = 1, nIdxCnt = 0; i < m_MeshFan[1].nNumVtxMax; i++)
	{
		pVtx[i].pos.x = cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)) * m_MeshDome.fRadius;

		pVtx[i].pos.y = -sinf(MESHDOME_ANGLE * RAD) * m_MeshDome.fRadius;
		pVtx[i].pos.z = cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)) * m_MeshDome.fRadius;

		pVtx[i].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)),
			-sinf(MESHDOME_ANGLE * RAD),
			cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)));
		pVtx[i].col = D3DCOLOR_RGBA(255, 255, 255, 255);

		pVtx[i].tex.x = 0.0f;
		pVtx[i].tex.y = 0.9f;
	}

	m_MeshFan[1].pVtxBuff->Unlock();


	// インデックス情報設定 /////
	WORD *pIdx;

	m_MeshDome.pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 変数定義
	int x, y, cnt;

	for (y = 0, cnt = 0; y < m_MeshDome.nRowBlock; y++)
	{
		for (x = 0; x < m_MeshDome.nColumnNumVtx; x++, cnt++, pIdx += 2)
		{
			pIdx[0] = x + (y + 1) * m_MeshDome.nColumnNumVtx;
			pIdx[1] = x + (y + 0) * m_MeshDome.nColumnNumVtx;
			if (x == m_MeshDome.nColumnNumVtx - 1 && cnt * 2 < m_MeshDome.nNumIdx - 2)
			{
				cnt++;
				pIdx += 2;
				pIdx[0] = x + (y + 0) * m_MeshDome.nColumnNumVtx;
				pIdx[1] = (y + 1 + 1) * m_MeshDome.nColumnNumVtx;
			}
		}
	}

	m_MeshDome.pIdxBuff->Unlock();
	// ファン部分
	m_MeshFan[0].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int i = 0; i < m_MeshFan[0].nNumIdx; i++)
	{
		pIdx[i] = i;
	}

	m_MeshFan[0].pIdxBuff->Unlock();

	m_MeshFan[1].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int i = 0; i < m_MeshFan[1].nNumIdx; i++)
	{
		pIdx[i] = i;
	}

	m_MeshFan[1].pIdxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CSkySphere::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CSkySphere::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_MeshDome.pVtxBuff);
	SAFE_RELEASE(m_MeshDome.pIdxBuff);
	for (int i = 0; i < 2; i++)
	{
		SAFE_RELEASE(m_MeshFan[i].pVtxBuff);
		SAFE_RELEASE(m_MeshFan[i].pIdxBuff);
	}
	m_pTexture = NULL;
}

/*******************************************************************************
* 関数名：void CSkySphere::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CSkySphere::Update(void)
{
	// カメラを追従
	CMode *game = CManager::GetMode();
	CCamera *camera = game->GetCamera();
	m_Pos.x = camera->GetPosition().x;
	m_Pos.z = camera->GetPosition().z;
	//m_Rot.y += ROT_SPEED;
}

/*******************************************************************************
* 関数名：void CSkySphere::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CSkySphere::Draw(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 各種設定 /////
	// ライトのOFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// アルファテストのON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
	// フォグのOFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// カリングの設定
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

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


	/// ドームの描画
	// データストリームにバインドする
	pDevice->SetStreamSource(0, m_MeshDome.pVtxBuff, 0, sizeof(VERTEX_3D));
	// インデックスバッファをバインド
	pDevice->SetIndices(m_MeshDome.pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,	// プリミティブの種類
		0,
		0,
		m_MeshDome.nNumVtxMax,	// 頂点数
		0,
		m_MeshDome.nNumPrim);	// 描画するプリミティブ数

	/// ファンの描画
	// カリング設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// データストリームにバインドする
	pDevice->SetStreamSource(0, m_MeshFan[0].pVtxBuff, 0, sizeof(VERTEX_3D));
	// インデックスバッファをバインド
	pDevice->SetIndices(m_MeshFan[0].pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLEFAN,			// プリミティブの種類
		0,
		0,
		m_MeshFan[0].nNumVtxMax,	// 頂点数
		0,
		m_MeshFan[0].nNumPrim);		// 描画するプリミティブ数

	// カリングをもとに戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// データストリームにバインドする
	pDevice->SetStreamSource(0, m_MeshFan[1].pVtxBuff, 0, sizeof(VERTEX_3D));
	// インデックスバッファをバインド
	pDevice->SetIndices(m_MeshFan[1].pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLEFAN,			// プリミティブの種類
		0,
		0,
		m_MeshFan[1].nNumVtxMax,	// 頂点数
		0,
		m_MeshFan[1].nNumPrim);		// 描画するプリミティブ数

	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	// フォグのON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// カリングをもとに戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/*******************************************************************************
* 関数名：CSkySphere *CSkySphere::Create( Vector3 pos, float rad, int row, int column )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CSkySphere *CSkySphere::Create(Vector3 pos, float rad, int row, int column)
{
	CSkySphere *scene3D;
	scene3D = new CSkySphere;
	scene3D->Init(pos, rad, row, column);
	return scene3D;
}