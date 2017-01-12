/*******************************************************************************
* タイトル名：
* ファイル名：model.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _MODEL_H_
#define _MODEL_H_

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
class CMotionManager;
class CScene;
class CSceneX;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
// モデル情報
class CModel
{
private:
	LPD3DXMESH		*m_pMesh;			// メッシュ情報
	LPD3DXBUFFER	*m_pBuffMat;		// マテリアル情報
	DWORD			*m_pNumMat;			// マテリアル数

	Vector3			m_Pos;				// モデルの現在の位置
	Vector3			m_Rot;				// モデルの現在の回転角

	Vector3			m_PosOld;			// モデルの前モーションの位置
	Vector3			m_RotOld;			// モデルの前モーションの回転角

	Vector3			m_PosOffset;		// モデルの位置オフセット
	Vector3			m_RotOffset;		// モデルの回転角オフセット

	Vector3			m_PosTemp;			// モーションの位置保存
	Vector3			m_RotTemp;			// モーションの回転角保存

	Vector3			m_PosNext;			// 次モーションの位置
	Vector3			m_RotNext;			// 次モーションの回転角

	D3DXMATRIX		m_MtxWorld;			// ワールドマトリックス
	int				m_nIdx;				// モデルID
	int				m_nParent;			// 親のID
	D3DXMATRIX		*m_pParentMtx;		// 親のマトリクスポインタ

	CMotionManager	*m_pMotionManager;	// モーションマネージャーのポインタ
	D3DXCOLOR		m_DiffuseCol;		// ディフーズカラー
	bool			m_bChangeColFlg;	// 色変更フラグ
public:
	CModel();
	~CModel();

	void Init(CMotionManager *pMotionManager);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Switch(void);

	void SetMatrix();

	void SetIndex(int nIdx) { m_nIdx = nIdx; }									// インデックスセット
	void SetParent(int parent) { m_nParent = parent; }							// 親インデックスセット
	D3DXMATRIX *GetMatrix(void) { return &m_MtxWorld; }							// ワールドマトリクスの取得
	void SetMatrix(D3DXMATRIX *pMatrix) { m_pParentMtx = pMatrix; }				// 親のワールドマトリクスのセット
	void SetPosition(Vector3 pos) { m_Pos = pos; }								// 位置のセット
	void SetRotation(Vector3 rot) { m_Rot = rot; }								// 回転角のセット
	Vector3 GetPosition(void) { return m_Pos; }									// 現在の位置の取得
	Vector3 GetRotation(void) { return m_Rot; }									// 現在の回転角の取得
	Vector3 GetPosOffset(void) { return m_PosOffset; }							// 位置オフセットのセット
	Vector3 GetRotOffset(void) { return m_RotOffset; }							// 回転角オフセットのセット
	void SetPosNext(Vector3 pos) { m_PosNext = pos; }							// 次のキーの位置のセット
	void SetRotNext(Vector3 rot) { m_RotNext = rot; }							// 次のキーの回転角のセット

	void SetDiffuseCol(D3DXCOLOR col) { m_DiffuseCol = col; m_bChangeColFlg = true; }
	void SetChangeColFlg(bool flg) { m_bChangeColFlg = flg; }
};

#endif