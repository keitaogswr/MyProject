/*******************************************************************************
* タイトル名：
* ファイル名：chargeEffect.h
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

/*******************************************************************************
* クラス宣言
*******************************************************************************/
// チャージエフェクト
class CChargeEffect : public CAnimationBoard
{
private:
	D3DXMATRIX *m_pParentMtx;						// 親のマトリクス
	Vector3 m_PosOffset;
public:
	CChargeEffect(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~CChargeEffect();

	void Init(Vector3 pos, D3DXCOLOR col, float width, float height, D3DXMATRIX *matrix);
	void Update(void);

	void SetRenderStateBegin(void);
	void SetRenderStateEnd(void);

	static CChargeEffect *Create(Vector3 pos, D3DXCOLOR col, float width, float height, D3DXMATRIX *matrix);

	virtual void SetWorldMatrix(void);				// ワールドマトリックス設定
};

