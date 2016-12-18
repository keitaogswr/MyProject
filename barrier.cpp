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
#include "barrier.h"
#include "texture.h"


/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )				// テクスチャ列分割数
#define TEXTURE_COLUMN	( 1 )				// テクスチャ行分割数
#define MESHDOME_ANGLE	( 80.0f )			// ドーム部分の角度
#define ROT_SPEED		( 0.01f )			// 
const float MAGNIFICATION = 0.1f;

/*******************************************************************************
* 関数名：CBarrier::CBarrier()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CBarrier::CBarrier(DRAWORDER DrawOrder, OBJTYPE ObjType) :CSkySphere(DrawOrder, ObjType)
{
	ZeroMemory(&m_MeshFan, sizeof(m_MeshFan));
	ZeroMemory(&m_MeshDome, sizeof(m_MeshDome));
	m_nAlfa = m_nAlfaN = 1.0f;
}

/*******************************************************************************
* 関数名：CBarrier::~CBarrier()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CBarrier::~CBarrier()
{
}

/*******************************************************************************
* 関数名：void CBarrier::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CBarrier::Init(Vector3 pos, float rad, int row, int column)
{
	m_Pos = pos;

	// ドーム部分
	m_MeshDome.nRowBlock = row;
	m_MeshDome.nColumnBlock = column;
	m_MeshDome.nRowNumVtx = m_MeshDome.nRowBlock + 1;
	m_MeshDome.nColumnNumVtx = m_MeshDome.nColumnBlock + 1;
	m_MeshDome.fRadius = rad;
	m_MeshDome.nNumVtxMax = (m_MeshDome.nRowNumVtx * 2 - 1) * m_MeshDome.nColumnNumVtx;
	m_MeshDome.nNumIdx = (m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 * 2) + (m_MeshDome.nRowBlock * 2 - 1) * 2;
	m_MeshDome.nNumPrim = (m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 * 2) + (m_MeshDome.nRowBlock * 2 - 1) * 4;

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
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_BARRIER_000);

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
	/// ドーム部分
	m_MeshDome.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntRow = m_MeshDome.nRowNumVtx - 1; nCntRow >= 0; nCntRow--)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
				sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow),
				cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// テクスチャブロック表示
			pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;
		}
	}
	for (int nCntRow = 1; nCntRow < m_MeshDome.nRowNumVtx; nCntRow++)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) * -m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
				sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow),
				cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// テクスチャブロック表示
			pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;
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

	for (y = 0, cnt = 0; y < m_MeshDome.nRowBlock * 2; y++)
	{
		for (x = 0; x < m_MeshDome.nColumnNumVtx; x++, cnt += 2, pIdx += 2)
		{
			pIdx[0] = x + (y + 1) * m_MeshDome.nColumnNumVtx;
			pIdx[1] = x + (y + 0) * m_MeshDome.nColumnNumVtx;
			if (x == m_MeshDome.nColumnNumVtx - 1 && y != m_MeshDome.nRowBlock * 2 - 1)
			{
				cnt += 2;
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
* 関数名：void CBarrier::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CBarrier::Uninit(void)
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
* 関数名：void CBarrier::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBarrier::Update(void)
{
	m_nAlfa += (m_nAlfaN - m_nAlfa) * MAGNIFICATION;

	// 頂点バッファの設定
	VERTEX_3D *pVtx;
	int nIdxCnt = 0;
	/// ドーム部分
	m_MeshDome.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < m_MeshDome.nNumVtxMax; i++)
	{
		pVtx[i].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_nAlfa);
	}

	m_MeshDome.pVtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CBarrier::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CBarrier::Draw(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// カリングの設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	CSkySphere::Draw();
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/*******************************************************************************
* 関数名：CBarrier *CBarrier::Create( Vector3 pos, float rad, int row, int column )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CBarrier *CBarrier::Create(Vector3 pos, float rad, int row, int column)
{
	CBarrier *scene3D;
	scene3D = new CBarrier;
	scene3D->Init(pos, rad, row, column);
	return scene3D;
}