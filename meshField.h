/*******************************************************************************
* タイトル名：
* ファイル名：meshField.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

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
class CMeshField : public CScene3D
{
	private:
		LPDIRECT3DINDEXBUFFER9	m_IdxBuff;		// インデックスバッファへのポインタ

		float m_fWidth;							// ブロック幅
		float m_fDepth;							// ブロック奥行
		int m_nRowBlock;						// 行ブロック数
		int m_nColumnBlock;						// 列ブロック数
		int m_nRowNumVtx;						// 行頂点数
		int m_nColumnNumVtx;					// 列頂点数
		int m_nNumVtxMax;						// 頂点最大数
		int m_nNumIdx;							// インデックス最大数
		int m_nNumPrim;							// プリミティブ最大数
		
		float m_fWidthAll;						// フィールド全体の幅
		float m_fDepthAll;						// フィールド全体の奥行

		int m_nCollNumVtx;						// 当たり判定のあったメッシュの起点頂点番号

		void SetVertex( void );
		void SetColorVertex( int idx, D3DCOLOR col );
		void ResetPosition( void );
		void Operate( void );

		int SaveFile( void );
		int LoadFile( void );

		Vector3 *mesh;							// メッシュ情報
	public:
		CMeshField( DRAWORDER DrawOrder = DRAWORDER_BACK, OBJTYPE ObjType = OBJTYPE_MESHFIELD );
		~CMeshField();
		void Init( Vector3 pos, float width, float height, int row, int column );
		void Uninit( void );
		void Update( void );
		void Draw( void );

		static CMeshField *Create( Vector3 pos, float width, float height, int row, int column );
		
		int Collision( Vector3 *vector );
		float GetHeight( Vector3 pos );
		Vector3 GetNormal( Vector3 *vector );
		float GetWidthAll(void);
		float GetDepthAll(void);
};

#endif