/*******************************************************************************
* タイトル名：
* ファイル名：afterBurner.h
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
class CAfterBurner
{
private:
	D3DXMATRIX *m_pParentMtx;
public:
	CAfterBurner();
	~CAfterBurner();

	void Init(Vector3 pos, Vector3 vec, D3DXMATRIX *matrix);
	void Update(void);
	void Uninit(void);
	void Set(Vector3 pos, Vector3 vec);

	static CAfterBurner *Create(Vector3 pos, Vector3 vec, D3DXMATRIX *matrix);
};

