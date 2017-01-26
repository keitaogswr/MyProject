/*******************************************************************************
* タイトル名：
* ファイル名：motionManager.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _MOTIONMANAGER_H_
#define _MOTIONMANAGER_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "model.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define DATA_MAX		( 10000 )		// テキストデータの行数

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CModel;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
// ロードモード
typedef enum
{
	MOTIONLOAD_CHAR,			// キャラクター読み込み
	MOTIONLOAD_MOTIONCOUNT,		// モーション数カウント
	MOTIONLOAD_MOTIONDATA,		// モーションデータ読み込み
	MOTIONLOAD_KEYSETCOUNT,		// キーフレーム数カウント
	MOTIONLOAD_KEYSETDATA,		// キーフレームデータ読み込み
	MOTIONLOAD_KEYCOUNT,		// キーパーツ数カウント
	MOTIONLOAD_KEYDATA,			// キーパーツデータ読み込み
	MOTIONLOAD_MAX
}MOTIONLOAD;

// キー要素
typedef struct
{
	Vector3		pos;
	Vector3		rot;
	//float			fPosX;				// X軸位置
	//float			fPosY;				// Y軸位置
	//float			fPosZ;				// Z軸位置
	//float			fRotX;				// X軸回転角
	//float			fRotY;				// Y軸回転角
	//float			fRotZ;				// Z軸回転角
}KEY;

// キー情報
typedef struct
{
	int				nFlame;				// フレーム数
	int				nNumKeyParts;		// キーパーツ数 
	KEY				**pKey;
}KEY_INFO;

// モーション情報
typedef struct
{
	int				nLoop;				// ループするかどうか
	int				nNumKey;			// キーセット数
	KEY_INFO		**pKeyInfo;
}MOTION_INFO;

// アニメーションモデル列挙情報
typedef enum
{
	DYNAMICMODEL_TYPE_PLAYER_00,
	DYNAMICMODEL_TYPE_PLAYER_01,
	DYNAMICMODEL_TYPE_ENEMY_00,
	DYNAMICMODEL_TYPE_ENEMY_01,
	DYNAMICMODEL_TYPE_BOSS_00,
	DYNAMICMODEL_TYPE_MAX
}DYNAMICMODEL_TYPE;
// パーツモデル情報
typedef struct
{
	LPD3DXMESH		*pMesh;							// メッシュ情報
	LPD3DXBUFFER	*pBuffMat;						// マテリアル情報
	DWORD			*pNumMat;						// マテリアル数

	int nIdx;										// インデックス
	int nParent;									// 親のインデックス
	Vector3 posOff;									// 位置のオフセット
	Vector3 rotOff;									// 回転角のオフセット
	char partsName[256];							// パーツファイル名
}MODEL_PARTS;
// アニメーションモデル情報
typedef struct
{
	char fileName[256];								// ファイル名
	int nNumModel;									// モデル数
	int nNumMotion;									// モーション数
	MOTION_INFO		**pMotion;						// モーションポインタ
	MODEL_PARTS		**pParts;						// パーツモデルポインタ
}DYNAMICMODEL_INFO;

// モーションマネージャクラス
class CMotionManager
{
private:
	int m_nFlameCnt;													// 現モーションのフレームカウンタ
	int m_nKeyInfoId;													// 現モーションのキーセットID
	int m_nMotionId;													// 現モーション番号

	int m_nOldFlameCnt;													// 前モーションのフレームカウンタ
	int m_nOldKeyInfoId;												// 前モーションのキーセットID
	int m_nOldMotionId;													// 前モーション番号

	bool m_bSwitch;														// モーション切り替えフラグ

	CModel **m_pModel;													// モデルポインタ
	D3DXMATRIX *m_pObjMtx;												// オブジェクトのマトリクスポインタ

	bool m_bEndMotion;													// モーション終了フラグ

	static DYNAMICMODEL_INFO m_DModel[DYNAMICMODEL_TYPE_MAX];			// アニメーションモデル情報
	DYNAMICMODEL_TYPE m_ModelType;										// モデルタイプ
public:
	CMotionManager();
	~CMotionManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void Load(void);
	static void Unload(void);

	static int FileOpen(const char *fileName, int nData);

	void ResetMotion(void);
	static CMotionManager *Create(DYNAMICMODEL_TYPE type, D3DXMATRIX *pMtx);

	void SetMotion(int nId);																// モーションセット
	void SetMatrix(D3DXMATRIX *pMtx) { m_pObjMtx = pMtx; }									// このマネージャを保持しているオブジェクトのワールドマトリクスを受け取る
	D3DXMATRIX *GetMatrix(void) { return m_pObjMtx; }										// このマネージャを保持しているオブジェクトのワールドマトリクスを返す
	MOTION_INFO **GetMotion(void) { return m_DModel[m_ModelType].pMotion; }					// モーションの取得
	int GetMotionId(void) { return m_nMotionId; }											// モーション番号の取得
	int GetFrame(void) { return m_nFlameCnt; }												// 現在のフレームを取得
	int GetKeyInfoId(void) { return m_nKeyInfoId; }											// 現在のキーフレームを取得

	int GetOldMotionId(void) { return m_nOldMotionId; }										// 一個前のモーション番号の取得
	int GetOldFrame(void) { return m_nOldFlameCnt; }										// 一個前のフレームを取得
	int GetOldKeyInfoId(void) { return m_nOldKeyInfoId; }									// 一個前のキーフレームを取得

	bool GetSwitch(void) { return m_bSwitch; }												// 切り替えフラグの取得
	CModel *GetModel(int nId) { return m_pModel[nId]; }										// モデルのポインタ取得
	D3DXMATRIX *GetModelMatrix(int id) { return m_pModel[id]->GetMatrix(); }				// モデルのマトリクスの取得

	bool EndMotion(void);																	// モーションが終了しているかどうか
	static DYNAMICMODEL_INFO *GetDynamicModelInfo(int nType) { return &m_DModel[nType]; }	// アニメーションモデル情報の取得
	void SetModelType(DYNAMICMODEL_TYPE nType) { m_ModelType = nType; }						// モデルタイプのセット
	DYNAMICMODEL_TYPE GetModelType(void) { return m_ModelType; }							// モデルタイプの取得

	void SetModelColor(int num, D3DXCOLOR color){ m_pModel[num]->SetDiffuseCol(color); }
	void SetModelColorAll(D3DXCOLOR color);
	void SetModelColFlg(int num, bool flg) { m_pModel[num]->SetChangeColFlg(flg); }
	void SetModelColFlgAll(bool flg);
};

#endif