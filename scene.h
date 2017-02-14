/*******************************************************************************
* タイトル名：
* ファイル名：scene.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _SCENE_H_
#define _SCENE_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/

/*******************************************************************************
* ライブラリのリンク
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
class Vector3;
class CList;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
typedef enum
{
	OBJTYPE_NONE,
	OBJTYPE_ENEMY,
	OBJTYPE_PLAYER,
	OBJTYPE_MESHFIELD,
	OBJTYPE_BILLBOARD,
	OBJTYPE_EFFECT,
	OBJTYPE_MAX
}OBJTYPE;

typedef enum
{
	DRAWORDER_BACK,
	DRAWORDER_STENCIL,
	DRAWORDER_3D,
	DRAWORDER_3DLATE,
	DRAWORDER_EFFECT,
	DRAWORDER_2D,
	DRAWORDER_MAX
}DRAWORDER;

// シーンクラス
class CScene
{
protected:
	static CScene *m_Top[DRAWORDER_MAX];	// 先頭
	static CScene *m_Cur[DRAWORDER_MAX];	// 現在

	Vector3 m_Pos;			// 位置
	Vector3 m_Rot;			// 回転値
	bool m_Delete;			// 削除フラグ
	bool m_bPause;			// ポーズフラグ
	bool m_bDraw;			// 描画フラグ

	OBJTYPE m_ObjType;
	DRAWORDER m_DrawOrder;
public:
	CScene(DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_NONE);
	~CScene();
	virtual void Init(void) {}
	virtual void Uninit(void) = 0;
	virtual void Update(void) {}
	virtual void Draw(void) = 0;

	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	void SetPosition(Vector3 pos);
	void Release(DRAWORDER DrawOrder);
	Vector3 GetPosition(void) { return m_Pos; }
	Vector3 GetRotation(void) { return m_Rot; }

	void SetObjType(OBJTYPE type) { m_ObjType = type; }
	OBJTYPE GetObjType(void) { return m_ObjType; }

	void SetDrawOrder(DRAWORDER type) { m_DrawOrder = type; }
	void SetDeleteFlg(bool flg) { m_Delete = flg; }
	bool GetDeleteFlg(void) { return m_Delete; }
	void SetPauseFlg(bool flg) { m_bPause = flg; }
	void SetDrawFlg(bool flg) { m_bDraw = flg; }

	virtual float GetCollision(void) { return NULL; }
	virtual void SetDamage(int damage) {}
	virtual void SetKeep(int keep) {}
	virtual Vector3 GetTargetPos(void) { return Vector3(0.0f, 0.0f, 0.0f); }
	virtual CScene *GetTarget(void) { return NULL; }

	CScene *m_Prev;			// 前のポインタ
	CScene *m_Next;			// 次のポインタ

	static CScene *GetList(DRAWORDER DrawOrder);
	static CScene *GetCur(DRAWORDER DrawOrder);

	CScene *GetNext(void);
};

#endif