/*******************************************************************************
* タイトル名：
* ファイル名：texture.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#pragma once

/*******************************************************************************
* インクルードファイル
*******************************************************************************/

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体
*******************************************************************************/
// テクスチャタイプの名前
typedef enum
{
	TEXTURE_TYPE_TITLE,
	TEXTURE_TYPE_TITLE_LOGO,
	TEXTURE_TYPE_PRESS_START,
	TEXTURE_TYPE_RESULT,
	TEXTURE_TYPE_GEAR_000,
	TEXTURE_TYPE_NUMBER_000,
	TEXTURE_TYPE_FIELD_000,
	TEXTURE_TYPE_SKY_000,
	TEXTURE_TYPE_TREE_000,
	TEXTURE_TYPE_TREE_001,
	TEXTURE_TYPE_TREE_002,
	TEXTURE_TYPE_TREE_003,
	TEXTURE_TYPE_TREE_004,
	TEXTURE_TYPE_WEED_000,
	TEXTURE_TYPE_WEED_001,
	TEXTURE_TYPE_WEED_002,
	TEXTURE_TYPE_EFFECT_000,
	TEXTURE_TYPE_SMOKE_000,
	TEXTURE_TYPE_SHADOW_000,
	TEXTURE_TYPE_BULLET_000,
	TEXTURE_TYPE_RETICLE_000,
	TEXTURE_TYPE_EXPLOSION_000,
	TEXTURE_TYPE_BARRIER_000,
	TEXTURE_TYPE_LIFEBAR_000,
	TEXTURE_TYPE_LIFEBAR_GAUGE_000,
	TEXTURE_TYPE_PAUSE_000,
	TEXTURE_TYPE_RESTART_000,
	TEXTURE_TYPE_EXIT_000,
	TEXTURE_TYPE_GAME_START_000,
	TEXTURE_TYPE_FINISH_000,
	TEXTURE_TYPE_GAMEUI_000,
	TEXTURE_TYPE_THANK_YOU_000,
	TEXTURE_TYPE_CHARGE_000,
	TEXTURE_TYPE_LAZER_000,
	TEXTURE_TYPE_MAX
}TEXTURE_TYPE;

// テクスチャのパラメータ
typedef struct
{
	char *texName;
}TEXTURE_PARAM;

/*******************************************************************************
* 前方宣言
*******************************************************************************/

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CTexture
{
private:
	static LPDIRECT3DTEXTURE9 m_Texture[TEXTURE_TYPE_MAX];					// テクスチャへのポインタ
	static TEXTURE_PARAM m_TexParam[TEXTURE_TYPE_MAX];						// テクスチャのパラメータ
public:
	CTexture();
	~CTexture();

	static void Load(void);
	static void Unload(void);

	static LPDIRECT3DTEXTURE9 *GetTexture(int nIdx);
};

