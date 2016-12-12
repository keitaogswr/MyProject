/*******************************************************************************
* タイトル名：
* ファイル名：number.h
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
class CNumber
{
private:
	LPDIRECT3DTEXTURE9	*m_pTexture;			// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_VtxBuff;		// 頂点バッファへのポインタ

	Vector3 m_Pos;
	Vector3 m_Rot;

	float m_fLength;
	float m_fAngle;

	int m_nNum;
public:
	CNumber();
	~CNumber();

	void Init(Vector3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CNumber *Create(Vector3 pos);
	void SetNum(int num);
};