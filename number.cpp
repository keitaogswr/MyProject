/*******************************************************************************
* タイトル名：
* ファイル名：number.cpp
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
#include "texture.h"
#include "scene.h"
#include "scene2D.h"
#include "number.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 25 )
#define HEIGHT			( 50 )
#define TEXTURE_ROW		( 10 )
#define TEXTURE_COLUMN	( 1 )
#define DEF_TIME		( 10 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CNumber::CNumber()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CNumber::CNumber()
{
	m_Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_Rot = Vector3(0.0f, 0.0f, 0.0f);
}

/*******************************************************************************
* 関数名：CNumber::~CNumber()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CNumber::~CNumber()
{
}

/*******************************************************************************
* 関数名：void CNumber::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CNumber::Init(Vector3 pos)
{
	m_Pos = pos;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_NUMBER_000);

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

	m_fLength = sqrtf(WIDTH * WIDTH + HEIGHT * HEIGHT) * 0.5f;
	m_fAngle = atan2f(WIDTH, HEIGHT);

	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_Pos.x + sinf(D3DX_PI + m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(D3DX_PI + m_fAngle + m_Rot.z) * m_fLength,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_Pos.x + sinf(D3DX_PI - m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(D3DX_PI - m_fAngle + m_Rot.z) * m_fLength,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_Pos.x + sinf(-m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(-m_fAngle + m_Rot.z) * m_fLength,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_Pos.x + sinf(m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(m_fAngle + m_Rot.z) * m_fLength,
		0.0f);

	pVtx[0].rhw =
		pVtx[1].rhw =
		pVtx[2].rhw =
		pVtx[3].rhw = 1.0f;

	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CNumber::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CNumber::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
}

/*******************************************************************************
* 関数名：void CNumber::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CNumber::Update(void)
{
	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].tex.x = m_nNum * 0.1f;
	pVtx[0].tex.y = 0.0f;
	pVtx[1].tex.x = m_nNum * 0.1f + 0.1f;
	pVtx[1].tex.y = 0.0f;
	pVtx[2].tex.x = m_nNum * 0.1f;
	pVtx[2].tex.y = 1.0f;
	pVtx[3].tex.x = m_nNum * 0.1f + 0.1f;
	pVtx[3].tex.y = 1.0f;

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CNumber::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CNumber::Draw(void)
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
		2);						//描画するプリミティブ数
}

/*******************************************************************************
* 関数名：CNumber *CNumber::Create( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CNumber *CNumber::Create(Vector3 pos)
{
	CNumber *number;
	number = new CNumber;
	number->Init(pos);
	return number;
}

/*******************************************************************************
* 関数名：void CNumber::SetNum( int num )
*
* 引数	：
* 戻り値：
* 説明	：数値セット処理
*******************************************************************************/
void CNumber::SetNum(int num)
{
	if (num >= 0 && num <= 9)
	{
		m_nNum = num;
	}
	else
	{
		MessageBox(*CManager::GetHWND(), "数値が正常ではありません", "警告", MB_ICONWARNING);
	}
}