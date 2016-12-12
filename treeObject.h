/*******************************************************************************
* タイトル名：
* ファイル名：billboard.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _TREEOBJECT_H_
#define _TREEOBJECT_H_

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
class CTreeObject : public CBillboard
{
public:
	CTreeObject(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CTreeObject();

	void Init(Vector3 pos, int num);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CTreeObject *Create(Vector3 pos, int num);
	static void Set(Vector3 pos, int num);

	static int SaveFile(void);
	static int LoadFile(void);

	int GetTreeType(void) { return m_nTreeType; }

	// 草木オブジェクトの種類
	typedef enum
	{
		TREEOBJ_TYPE_TREE0,
		TREEOBJ_TYPE_TREE1,
		TREEOBJ_TYPE_TREE2,
		TREEOBJ_TYPE_TREE3,
		TREEOBJ_TYPE_TREE4,
		TREEOBJ_TYPE_WEED0,
		TREEOBJ_TYPE_WEED1,
		TREEOBJ_TYPE_WEED2,
		TREEOBJ_TYPE_MAX
	}TREEOBJ_TYPE;

	// 草木オブジェクトのパラメータ
	typedef struct
	{
		int nIdx;
		float width;
		float height;
	}TREEOBJ_PARAM;

	static TREEOBJ_PARAM treeObjParam[TREEOBJ_TYPE_MAX];
	static int m_nTexNum;

	TREEOBJ_TYPE billboardType;		// 草木オブジェクトのタイプ
private:
	int m_nTreeType;
};

#endif