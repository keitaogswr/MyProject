/*******************************************************************************
* タイトル名：
* ファイル名：model.cpp
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "motionManager.h"
#include "model.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define SWITCH_FLAME	( 5 )			// 切り替えフレーム

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CModel::CModel()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CModel::CModel()
{
	m_pMesh = 0;
	m_pBuffMat = 0;
	m_pNumMat = 0;

	m_Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_Rot = Vector3(0.0f, 0.0f, 0.0f);

	m_PosOld = Vector3(0.0f, 0.0f, 0.0f);
	m_RotOld = Vector3(0.0f, 0.0f, 0.0f);

	m_PosOffset = Vector3(0.0f, 0.0f, 0.0f);
	m_RotOffset = Vector3(0.0f, 0.0f, 0.0f);

	m_PosNext = Vector3(0.0f, 0.0f, 0.0f);
	m_RotNext = Vector3(0.0f, 0.0f, 0.0f);

	m_PosTemp = Vector3(0.0f, 0.0f, 0.0f);
	m_RotTemp = Vector3(0.0f, 0.0f, 0.0f);

	m_nIdx = 0;
	m_nParent = -1;
	m_pParentMtx = NULL;

	m_pMotionManager = NULL;

	m_bChangeColFlg = false;
}

/*******************************************************************************
* 関数名：CModel::~CModel()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CModel::~CModel()
{
}

/*******************************************************************************
* 関数名：void CModel::Init( CMotionManager *pMotionManager )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CModel::Init(CMotionManager *pMotionManager)
{
	//BYTE *pVtxBuff;		// 頂点情報へのポインタ

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	m_pMotionManager = pMotionManager;

	DYNAMICMODEL_TYPE type = m_pMotionManager->GetModelType();
	DYNAMICMODEL_INFO *info = m_pMotionManager->GetDynamicModelInfo(type);

	m_pMesh = info->pParts[m_nIdx]->pMesh;
	m_pBuffMat = info->pParts[m_nIdx]->pBuffMat;
	m_pNumMat = info->pParts[m_nIdx]->pNumMat;
	// オフセットの設定
	m_PosOffset = m_Pos = info->pParts[m_nIdx]->posOff;
	m_RotOffset = m_Rot = info->pParts[m_nIdx]->rotOff;

	// デフォルトのモーション(0番目)を設定
	MOTION_INFO **pMotion = m_pMotionManager->GetMotion();	// デフォルトモーシヨンポインタ
	KEY_INFO *keyInfo = pMotion[0]->pKeyInfo[0];			// デフォルトキーフレーム
	KEY *key = pMotion[0]->pKeyInfo[0]->pKey[m_nIdx];		// デフォルトキー

	m_Pos += key->pos;
	m_Rot += key->rot;
	CManager::CheckRot(&m_Rot);

	// 親番号
	m_nParent = info->pParts[m_nIdx]->nParent;

	if (m_nParent >= 0)
	{// 親がいるとき
		m_pParentMtx = m_pMotionManager->GetModel(m_nParent)->GetMatrix();
	}
	else
	{// 親がいないとき
		m_pParentMtx = m_pMotionManager->GetMatrix();
	}
}

/*******************************************************************************
* 関数名：void CModel::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CModel::Uninit(void)
{
}

/*******************************************************************************
* 関数名：void CModel::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CModel::Update(void)
{
	Vector3 fDiffNow, fDiffOld;		// 円周率用変数
	Vector3 pos, rot;

	// 変数代入
	int nMotionId = m_pMotionManager->GetMotionId();								// 現在のモーション番号
	int nKeyInfoId = m_pMotionManager->GetKeyInfoId();								// 現在のキーフレーム番号
	int nFrame = m_pMotionManager->GetFrame();										// 現在のフレーム数
	// 構造体代入
	MOTION_INFO **pMotion = m_pMotionManager->GetMotion();							// モーシヨンポインタ
	KEY_INFO *keyInfo = pMotion[nMotionId]->pKeyInfo[nKeyInfoId];					// 現在のキーフレーム
	KEY *key = pMotion[nMotionId]->pKeyInfo[nKeyInfoId]->pKey[m_nIdx];				// 現在のキー
	KEY *keyNext = pMotion[nMotionId]->pKeyInfo[(nKeyInfoId + 1) % pMotion[nMotionId]->nNumKey]->pKey[m_nIdx];				// 次のキー

	// ブレンド用モーション変数
	int nOldMotionId = m_pMotionManager->GetOldMotionId();							// モーション番号
	int nOldKeyInfoId = m_pMotionManager->GetOldKeyInfoId();						// キーフレーム番号
	int nOldFrame = m_pMotionManager->GetOldFrame();								// フレーム数
	KEY_INFO *oldInfo = pMotion[nOldMotionId]->pKeyInfo[nOldKeyInfoId];				// キーフレーム
	KEY *oldKey = pMotion[nOldMotionId]->pKeyInfo[nOldKeyInfoId]->pKey[m_nIdx];		// キー
	KEY *oldKeyNext = pMotion[nOldMotionId]->pKeyInfo[(nOldKeyInfoId + 1) % pMotion[nOldMotionId]->nNumKey]->pKey[m_nIdx];	// 次のキー

	// モーション補間比率
	float rate;
	rate = (float)nFrame / (float)keyInfo->nFlame;
	// モーションブレンド比率
	float switchRate;
	switchRate = (float)nFrame / SWITCH_FLAME;

	if (pMotion[nMotionId]->nLoop == 0 && nKeyInfoId == pMotion[nMotionId]->nNumKey - 1)
	{// ループフラグが0かつ、最後のキーフレームに達していたら
		return;
	}

	// 今キーの始点と前キーの始点の差分をフレームで割り、現在値に加算
	// 位置
	if (m_pMotionManager->GetSwitch() == false)
	{// 通常モーション時
		pos = (keyNext->pos - key->pos) / (float)keyInfo->nFlame;
		//pos = key->pos * (1.0f - rate) + keyNext->pos * rate;

		m_Pos += pos;
		//m_Pos = m_PosOffset + pos;
	}
	else
	{// モーション切り替え時
		//pos = (keyNext->pos - key->pos) / (float)keyInfo->nFlame * (1.0f - switchRate) + (oldKeyNext->pos - oldKey->pos) / (float)oldInfo->nFlame * switchRate;
		if (pMotion[nOldMotionId]->nLoop == 1)
		{// ループフラグが1のとき過去モーションを更新
			m_PosOld += (oldKeyNext->pos - oldKey->pos) / (float)oldInfo->nFlame;
		}
		m_PosTemp += (keyNext->pos - key->pos) / (float)keyInfo->nFlame;

		m_Pos = m_PosOld * (1.0f - switchRate) + m_PosTemp * switchRate;
	}

	// 回転値
	if (m_pMotionManager->GetSwitch() == false)
	{// 通常モーション時
		fDiffNow = keyNext->rot - key->rot;
		CManager::CheckRot(&fDiffNow);				// 円周率チェック
		rot = fDiffNow / (float)keyInfo->nFlame;
		//rot = key->rot * (1.0f - rate) + keyNext->rot * rate;
		CManager::CheckRot(&rot);					// 円周率チェック

		m_Rot += rot;
		//m_Rot = m_RotOffset + rot;
		CManager::CheckRot(&m_Rot);					// 円周率チェック
	}
	else
	{// モーション切り替え時
		if (pMotion[nOldMotionId]->nLoop == 1)
		{// ループフラグが1のとき過去モーションを更新
			fDiffOld = oldKeyNext->rot - oldKey->rot;
			CManager::CheckRot(&fDiffOld);				// 円周率チェック
			fDiffOld /= (float)oldInfo->nFlame;
			CManager::CheckRot(&fDiffOld);				// 円周率チェック
			m_RotOld += fDiffOld;
			CManager::CheckRot(&m_RotOld);				// 円周率チェック
		}

		fDiffNow = keyNext->rot - key->rot;
		CManager::CheckRot(&fDiffNow);				// 円周率チェック
		fDiffNow /= (float)keyInfo->nFlame;
		CManager::CheckRot(&fDiffNow);				// 円周率チェック
		m_RotTemp += fDiffNow;
		CManager::CheckRot(&m_RotTemp);				// 円周率チェック

		m_Rot = m_RotOld * (1.0f - switchRate) + m_RotTemp * switchRate;
		CManager::CheckRot(&m_Rot);					// 円周率チェック
	}
}

/*******************************************************************************
* 関数名：void CModel::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CModel::Draw(void)
{
	D3DXMATRIX		mtxRot, mtxTrans;				// 向き、ポジション
	D3DXMATERIAL	*pMat;							// マテリアルポインタ
	D3DMATERIAL9	matDef;							// デフォルトのマテリアル

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	pDevice->GetMaterial(&matDef);							// 現在のマテリアルを取得
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);
	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	// 親のマトリクスと掛け合わせる
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		m_pParentMtx);

	// ワールドマトリックスをバインド
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	pMat = (D3DXMATERIAL*)(*m_pBuffMat)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)(*m_pNumMat); nCntMat++)
	{
		if (m_bChangeColFlg)
		{
			pMat[nCntMat].MatD3D.Diffuse = m_DiffuseCol;
		}
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// メッシュの描画
		(*m_pMesh)->DrawSubset(nCntMat);
	}
	// マテリアルを元に戻す
	pDevice->SetMaterial(&matDef);
}

/*******************************************************************************
* 関数名：void CModel::Reset( void )
*
* 引数	：
* 戻り値：
* 説明	：モーション切り替え処理
*******************************************************************************/
void CModel::Switch(void)
{
	// 現在の値を格納
	m_PosOld = m_Pos;
	m_RotOld = m_Rot;

	// 現在の値に次モーションの値を設定
	int nMotionId = m_pMotionManager->GetMotionId();				// 現在のモーション番号
	MOTION_INFO **pMotion = m_pMotionManager->GetMotion();			// モーシヨンポインタ
	KEY_INFO *keyInfo = pMotion[nMotionId]->pKeyInfo[0];			// キーフレーム
	KEY *key = pMotion[nMotionId]->pKeyInfo[0]->pKey[m_nIdx];		// キー
	// ブレンド用に格納
	m_PosTemp = m_PosOffset + key->pos;
	m_RotTemp = key->rot;
	CManager::CheckRot(&m_Rot);	// 円周率チェック
}

/*******************************************************************************
* 関数名：void CModel::Reset( void )
*
* 引数	：
* 戻り値：
* 説明	：モーション切り替え処理
*******************************************************************************/
void CModel::SetMatrix(void)
{
	D3DXMATRIX		mtxRot, mtxTrans;				// 向き、ポジション

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);
	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	// 親のマトリクスと掛け合わせる
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		m_pParentMtx);
}