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
#include "explosion.h"
#include "texture.h"
#include "sound.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH				( 30 )		// ポリゴン幅
#define HEIGHT				( 30 )		// ポリゴン高さ
#define TEXTURE_ROW			( 2 )		// テクスチャ列分割数
#define TEXTURE_COLUMN		( 8 )		// テクスチャ行分割数
#define MOVE_SPEED			( 10.0f )	// 移動速度
#define LIFE_MAX			( 1000 )	// 寿命
#define ATTEN_DEF			( 0.0001f )	// 初期減衰係数
#define ATTEN_RISE			( 2.0f )	// 減衰係数の上昇倍率
#define DRAW_SPEED			( 3 )		// 描画速度

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CExplosion::CExplosion()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CExplosion::CExplosion(DRAWORDER DrawOrder, OBJTYPE ObjType) : CAnimationBoard(DrawOrder, ObjType)
{
	m_nCntAnim = 0;
	m_nPatternAnim = 0;
	m_nRowAnim = 0;
	m_nTexColumn = TEXTURE_COLUMN;
	m_nTexRow = TEXTURE_ROW;
	CSound::Play(CSound::SOUND_LABEL_EXPLOSION_000);
}

/*******************************************************************************
* 関数名：CExplosion::~CExplosion()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CExplosion::~CExplosion()
{
}

/*******************************************************************************
* 関数名：void CAnimationBoard::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CExplosion::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_Pos = pos;
	m_Col = col;
	m_fWidth = width;
	m_fHeight = height;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_EXPLOSION_000);

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
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
		m_fHeight * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f,
		m_fHeight * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
		-m_fHeight * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f,
		-m_fHeight * 0.5f,
		0.0f);

	pVtx[0].nor =
		pVtx[1].nor =
		pVtx[2].nor =
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col =
		pVtx[1].col =
		pVtx[2].col =
		pVtx[3].col = m_Col;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_COLUMN, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_ROW);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_COLUMN, 1.0f / TEXTURE_ROW);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CExplosion::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CExplosion::Update(void)
{
	// 寿命を減らす
	m_nLife--;

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
				SetDeleteFlg(true);
			}
		}
		// 頂点設定
		SetVertex();
	}
}

/*******************************************************************************
* 関数名：CExplosion *CExplosion::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CExplosion *CExplosion::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CExplosion *effect;
	effect = new CExplosion;

	effect->Init(pos, col, width, height);
	return effect;
}