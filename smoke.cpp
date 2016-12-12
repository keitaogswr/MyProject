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
#include "texture.h"
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
#include "smoke.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH				( 30 )		// ポリゴン幅
#define HEIGHT				( 30 )		// ポリゴン高さ
#define TEXTURE_ROW			( 8 )		// テクスチャ列分割数
#define TEXTURE_COLUMN		( 1 )		// テクスチャ行分割数
#define MOVE_SPEED			( 10.0f )	// 移動速度
#define LIFE_MAX			( 1000 )	// 寿命
#define ATTEN_DEF			( 0.0001f )	// 初期減衰係数
#define ATTEN_RISE			( 2.0f )	// 減衰係数の上昇倍率
#define DRAW_SPEED			( 3 )		// 描画速度
#define UP_SPEED			( 0.5f )	// 上昇速度
#define LENG_SPEED			( 0.5f )	// 上昇速度

/*******************************************************************************
* 関数名：CSmoke::CSmoke()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CSmoke::CSmoke(DRAWORDER DrawOrder, OBJTYPE ObjType) :CAnimationBoard(DrawOrder, ObjType)
{
}

/*******************************************************************************
* 関数名：CSmoke::~CSmoke()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CSmoke::~CSmoke()
{
}

/*******************************************************************************
* 関数名：void CSmoke::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CSmoke::Update(void)
{
	// 寿命を減らす
	m_nLife--;

	m_fWidth += LENG_SPEED;
	m_fHeight += LENG_SPEED;

	// パターン描画更新
	m_nCntAnim++;

	m_Pos.y += UP_SPEED;
	if (m_nCntAnim == DRAW_SPEED)
	{
		m_nCntAnim = 0;
		m_nPatternAnim++;
		if (m_nPatternAnim == TEXTURE_ROW)
		{
			m_nPatternAnim = 0;

			SetDeleteFlg(true);
		}
	}
	// 頂点設定
	SetVertex();
}

/*******************************************************************************
* 関数名：CSmoke *CSmoke::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CSmoke *CSmoke::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CSmoke *effect;
	effect = new CSmoke;

	effect->Init(pos, col, width, height);
	return effect;
}
