/*******************************************************************************
* タイトル名：
* ファイル名：animationBoard.h
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

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CEffect;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CAnimationBoard : public CEffect
{
protected:
	int m_nCntAnim;
	int m_nPatternAnim;
	int m_nRowAnim;
	int m_nTexColumn;
	int m_nTexRow;

	void SetVertex(void);
public:
	CAnimationBoard(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~CAnimationBoard();

	virtual void Init(Vector3 pos, D3DXCOLOR col, float width, float height);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	static CAnimationBoard *Create(Vector3 pos, D3DXCOLOR col, float width, float height);
};

