/*******************************************************************************
* タイトル名：
* ファイル名：meshCylinder.h
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
#include "scene3D.h"

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
class CMeshCylinder : public CScene3D
{
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;				// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	m_pIdxBuff;				// インデックスバッファへのポインタ

	float m_fRadius;								// 筒半径
	float m_fRadiusN;								// 筒半径目標値
	float m_fHeight;								// 1ブロックの高さ
	float m_fHeightN;								// 1ブロックの高さ
	int m_nRowBlock;								// 行ブロック数
	int m_nColumnBlock;								// 列ブロック数
	int m_nRowNumVtx;								// 行頂点数
	int m_nColumnNumVtx;							// 列頂点数
	int m_nNumVtxMax;								// 頂点最大数
	int m_nNumIdx;									// インデックス最大数
	int m_nNumPrim;									// プリミティブ最大数
	D3DXMATRIX *m_pParentMtx;						// 親のマトリクス

	void SetVertex(void);							// 頂点設定

	virtual void SetRenderStateBegin(void);			// レンダラーステート設定開始
	virtual void SetRenderStateEnd(void);			// レンダラーステート設定終了
public:
	CMeshCylinder(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~CMeshCylinder();

	void Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CMeshCylinder *Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix);

	virtual void SetWorldMatrix(void);				// ワールドマトリックス設定
};

