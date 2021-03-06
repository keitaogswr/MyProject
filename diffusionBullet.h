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

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CDiffusionBullet : public CBullet
{
private:
public:
	CDiffusionBullet(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CDiffusionBullet();

	void Init(Vector3 pos, Vector3 rot);
	void Uninit(void);
	void Update(void);

	static CDiffusionBullet *Create(Vector3 pos, Vector3 rot);
};

class CDiffusionBulletChild : public CDiffusionBullet
{
public:
	CDiffusionBulletChild(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CDiffusionBulletChild() {}

	void Init(Vector3 pos, Vector3 rot);
	void Update(void);

	static CDiffusionBulletChild *Create(Vector3 pos, Vector3 rot);
};