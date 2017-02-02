/*******************************************************************************
* タイトル名：
* ファイル名：lazer.cpp
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
#include "debugProc.h"
#include "texture.h"
#include "lazer.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const float MAGNI = 0.005f;			// 高さによる影の倍率
const float RADIUS_ATEEN = 0.1f;	// 半径の慣性減衰係数
const float HEIGHT = 5000.0f;		// 高さ
const int	DELETE_CNT = 150;		// 消滅カウンタ
const int	DAMAGE_VALUE = 1;		// ダメージ
const int	KEEP_VALUE = 20;		// 被耐久値

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CLazer::CLazer(DRAWORDER DrawOrder, OBJTYPE ObjType) :CMeshCylinder(DrawOrder, ObjType)
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CLazer::CLazer(DRAWORDER DrawOrder, OBJTYPE ObjType) :CMeshCylinder(DrawOrder, ObjType)
{
	m_pCylinder = NULL;
	m_nDeleteCnt = 0;
}

/*******************************************************************************
* 関数名：CLazer::~CLazer()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CLazer::~CLazer()
{
}

/*******************************************************************************
* 関数名：void CLazer::Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CLazer::Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
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

	m_pCylinder = CMeshCylinder::Create(pos, rot, D3DXCOLOR(0.7f, 0.0f, 1.0f, 0.8f), radius * 0.5f, matrix);
}

/*******************************************************************************
* 関数名：void CLazer::Uninit(void)
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CLazer::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_pVtxBuff);
	// 削除フラグ
	m_Delete = true;
	if (m_pCylinder)
	{
		m_pCylinder->SetDeleteFlg(true);
		m_pCylinder = NULL;
	}
}

/*******************************************************************************
* 関数名：void CLazer::Update(void)
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CLazer::Update(void)
{
	CMeshCylinder::Update();
	Collision();
	m_nDeleteCnt++;
	if ((m_nDeleteCnt % DELETE_CNT) == 0)
	{
		SetDeleteFlg(true);
		m_pCylinder->SetDeleteFlg(true);
	}
}

/*******************************************************************************
* 関数名：void CLazer::Draw(void)
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CLazer::Draw(void)
{
	CMeshCylinder::Draw();
}

/*******************************************************************************
* 関数名：CLazer *CLazer::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CLazer *CLazer::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
{
	CLazer *lazer;
	lazer = new CLazer;
	lazer->Init(pos, rot, col, radius, matrix);
	return lazer;
}

/*******************************************************************************
* 関数名：void CLazer::SetRenderStateBegin( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定開始処理
*******************************************************************************/
void CLazer::SetRenderStateBegin(void)
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
* 関数名：void CLazer::SetRenderStateEnd( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定終了処理
*******************************************************************************/
void CLazer::SetRenderStateEnd(void)
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
void CLazer::SetWorldMatrix(void)
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

	// 親のマトリクスと掛け合わせる
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		m_pParentMtx);
}

/*******************************************************************************
* 関数名：void CMeshCylinder::Collision( void )
*
* 引数	：
* 戻り値：
* 説明	：当たり判定処理
*******************************************************************************/
void CLazer::Collision(void)
{
	/* 変数定義 */
	Vector3 pos, out;
	Vector3 vec0, vec1;
	float length;
	float work;

	// 座標変換
	SetWorldMatrix();
	D3DXVec3TransformCoord(&pos,
		&Vector3(0.0f, m_fHeight, 0.0f),
		&m_MtxWorld);

	// ベクトル計算
	vec0 = pos - m_Pos;				// レーザーのベクトル
	length = vec0.LengthSq();		// レーザーの長さ

	// 当たり判定処理
	if (length > 0.0f)
	{
		CScene *scene = CScene::GetList(DRAWORDER_3D);
		CScene *next = NULL;
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			if (scene->GetObjType() == OBJTYPE_PLAYER)
			{
				// ベクトル計算
				vec1 = scene->GetPosition() - m_Pos;					// レーザーとプレイヤーの距離
				// 割合の計算
				work = D3DXVec3Dot(&vec1, &vec0) / length;				// レーザーの長さに対する最短距離の割合
				work = work < 0.0f ? 0.0f : work > 1.0f ? 1.0f : work;	// レーザーの長さの外の場合修正
				// 近似点の計算
				out = m_Pos + (vec0 * work);
				// 衝突判定
				if ((scene->GetPosition() - out).Length() < m_fRadius + scene->GetCollision())
				{
					scene->SetDamage(DAMAGE_VALUE);
					scene->SetKeep(KEEP_VALUE);
				}
			}
			scene = next;
		}
	}
}