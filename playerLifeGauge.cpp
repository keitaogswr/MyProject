/*******************************************************************************
* タイトル名：
* ファイル名：scene2D.cpp
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
#include "scene2D.h"
#include "texture.h"
#include "playerLifeGauge.h"

/*******************************************************************************
* タイトル名：
* ファイル名：scene2D.cpp
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
#include "scene2D.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 600 )
#define HEIGHT			( 60 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CPlayerLifeGauge::CPlayerLifeGauge()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CPlayerLifeGauge::CPlayerLifeGauge(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene2D(DrawOrder, ObjType)
{
	m_fMax = 1.0f;
	m_fCurrent = 1.0f;
}

/*******************************************************************************
* 関数名：CPlayerLifeGauge::~CPlayerLifeGauge()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CPlayerLifeGauge::~CPlayerLifeGauge()
{
}

/*******************************************************************************
* 関数名：void CPlayerLifeGauge::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CPlayerLifeGauge::Init(Vector3 pos)
{
	// ゲージの生成
	CScene2D::Create(pos, WIDTH, HEIGHT, TEXTURE_TYPE_LIFEBAR_GAUGE_000);
	// 値の格納
	m_Pos = pos;
	m_fWidth = WIDTH;
	m_fHeight = HEIGHT;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_LIFEBAR_000);

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) * 0.5f;
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// 頂点バッファの設定
	SetVertex();
}

/*******************************************************************************
* 関数名：void CPlayerLifeGauge::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CPlayerLifeGauge::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
}

/*******************************************************************************
* 関数名：void CPlayerLifeGauge::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CPlayerLifeGauge::Update(void)
{
	SetVertex();
}

/*******************************************************************************
* 関数名：void CPlayerLifeGauge::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CPlayerLifeGauge::Draw(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ストリームにバインド
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	// ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//プリミティブの種類
		0,							//最初の頂点のインデックス
		2);							//描画するプリミティブ数
}

/*******************************************************************************
* 関数名：CPlayerLifeGauge *CPlayerLifeGauge::Create( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CPlayerLifeGauge *CPlayerLifeGauge::Create(Vector3 pos)
{
	CPlayerLifeGauge *scene2D;
	scene2D = new CPlayerLifeGauge;
	scene2D->Init(pos);
	return scene2D;
}

/*******************************************************************************
* 関数名：void CPlayerLifeGauge::SetVertex(void)
*
* 引数	：
* 戻り値：
* 説明	：頂点設定処理
*******************************************************************************/
void CPlayerLifeGauge::SetVertex(void)
{
	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f,
		m_Pos.y - m_fHeight * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f + m_fWidth * m_fCurrent / m_fMax,
		m_Pos.y - m_fHeight * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f,
		m_Pos.y + m_fHeight * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f + m_fWidth * m_fCurrent / m_fMax,
		m_Pos.y + m_fHeight * 0.5f,
		0.0f);

	pVtx[0].rhw =
	pVtx[1].rhw =
	pVtx[2].rhw =
	pVtx[3].rhw = 1.0f;

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = m_Col;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_fCurrent / m_fMax, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(m_fCurrent / m_fMax, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}