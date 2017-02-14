/*******************************************************************************
* タイトル名：
* ファイル名：bullet.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _BULLET_H_
#define _BULLET_H_

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
class CScene3D;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CBullet : public CBillboard
{
protected:
	int m_nLife;									// 寿命
	OBJTYPE m_ObjType;								// 弾の属性
	D3DXCOLOR m_Col;
	Vector3 m_Vec;

	virtual void UpdateCollision(void);	// 当たり判定
	virtual void DeleteCheak(void);		// 消去判定
public:
	CBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CBullet();

	virtual void Init(Vector3 pos, Vector3 vec, D3DXCOLOR col);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	static CBullet *Create(Vector3 pos, Vector3 vec, D3DXCOLOR col);
};

class CEnemyBullet : public CBullet
{
protected:
public:
	CEnemyBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CEnemyBullet(){}

	void Update(void);

	static CEnemyBullet *Create(Vector3 pos, Vector3 vec, D3DXCOLOR col);
};


class CBossBullet : public CBullet
{
protected:
public:
	CBossBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CBossBullet() {}

	void Update(void);

	static CBossBullet *Create(Vector3 pos, Vector3 vec, D3DXCOLOR col);
};


#endif