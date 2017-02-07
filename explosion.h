/*******************************************************************************
* タイトル名：
* ファイル名：explosion.h
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
#include "animationBoard.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CEffect;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CExplosion : public CAnimationBoard
{
protected:
public:
	CExplosion(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~CExplosion();

	virtual void Init(Vector3 pos, D3DXCOLOR col, float width, float height);
	virtual void Update(void);

	static CExplosion *Create(Vector3 pos, D3DXCOLOR col, float width, float height);
};

