/*******************************************************************************
* タイトル名：
* ファイル名：orbit.h
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
#define ORBIT_VERTEX		( 20 )

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
class COrbit : public CScene3D
{
private:
	Vector3 m_OldPos[ORBIT_VERTEX];
	Vector3 m_ParentPos;
	Vector3 m_HeadPos;
	D3DXMATRIX m_ParentMtx;
	D3DXCOLOR m_Col;

	void SetVertex(void);
public:
	COrbit(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~COrbit();
	void Init(Vector3 pos, D3DXCOLOR col);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Set(Vector3 parentPos, Vector3 headPos, D3DXMATRIX matrix) { m_ParentPos = parentPos; m_HeadPos = headPos; m_ParentMtx = matrix; }

	static COrbit *Create(Vector3 pos, D3DXCOLOR col);
};

