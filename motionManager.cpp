/*******************************************************************************
* タイトル名：
* ファイル名：motionManager.cpp
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
#include "scene.h"
#include "sceneX.h"
#include "model.h"
#include "motionManager.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define SWITCH_FLAME	( 5 )			// 切り替えフレーム

/*******************************************************************************
* グローバル変数
*******************************************************************************/
DYNAMICMODEL_INFO CMotionManager::m_DModel[DYNAMICMODEL_TYPE_MAX] =
{
	{ "data\\MOTION\\motion_b_type_origin.txt" },
	{ "data\\MOTION\\motion_haitatsuoh.txt" },
	{ "data\\MOTION\\motion_enemyFly.txt" },
	{ "data\\MOTION\\motion_enemySpeed.txt" },
	{ "data\\MOTION\\motion_enemyBoss_TypeFortress.txt" }
};

/*******************************************************************************
* 関数名：CMotionManager::CMotionManager()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CMotionManager::CMotionManager()
{
	m_nFlameCnt = 0;
	m_nKeyInfoId = 0;
	m_bSwitch = false;
	m_nMotionId = -1;

	m_nOldFlameCnt = 0;
	m_nOldKeyInfoId = 0;
	m_nOldMotionId = -1;

	m_bEndMotion = false;

	m_ModelType = DYNAMICMODEL_TYPE_MAX;
}

/*******************************************************************************
* 関数名：CMotionManager::~CMotionManager()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CMotionManager::~CMotionManager()
{
}

/*******************************************************************************
* 関数名：void CMotionManager::Init( Vector3 pos, Vector3 rot )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
HRESULT CMotionManager::Init()
{
	// モデルの確保、初期化
	m_pModel = new CModel*[m_DModel[m_ModelType].nNumModel];
	for (int nCnt = 0; nCnt < m_DModel[m_ModelType].nNumModel; nCnt++)
	{
		m_pModel[nCnt] = new CModel;
	}
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		m_pModel[i]->SetIndex(i);		// インデックスの設定
		m_pModel[i]->Init(this);
	}

	return S_OK;
}

/*******************************************************************************
* 関数名：void CMotionManager::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CMotionManager::Uninit(void)
{
	// 解放
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
 		m_pModel[i]->Uninit();
		SAFE_DELETE(m_pModel[i]);
	}
	SAFE_DELETE_ARRAY(m_pModel);
}

/*******************************************************************************
* 関数名：void CMotionManager::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CMotionManager::Update(void)
{
	MOTION_INFO **pMotion, **pOldMotion;
	pMotion = &m_DModel[m_ModelType].pMotion[m_nMotionId];
	pOldMotion = &m_DModel[m_ModelType].pMotion[m_nOldMotionId];

	if (m_bEndMotion == true)
	{// モーションが終了していたらモーションしない
		return;
	}

	// 各パーツの更新
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		m_pModel[i]->Update();
	}
	m_nFlameCnt++;			// フレームカウント
	if (m_bSwitch == true && (*pOldMotion)->nLoop == 1)
	{
		m_nOldFlameCnt++;
	}

	if (SWITCH_FLAME < m_nFlameCnt && m_bSwitch == true)
	{// モーション切り替え状態中で、現フレームが指定されたフレームに達したら
	 // 各パーツの今キーの始点情報を前キーの始点情報に保存
		m_bSwitch = false;		// モーション切り替えOFF
	}
	// 通常モーション中で、現フレームがモーションに指定されたフレーム数に達したら
	else if ((*pMotion)->pKeyInfo[m_nKeyInfoId]->nFlame <= m_nFlameCnt && m_bSwitch == false)
	{
		// 各パーツの今キーの始点情報を前キーの始点情報に保存
		m_nKeyInfoId++;		// 次のキーセットへ
		m_nFlameCnt = 0;	// フレームカウンタを初期化
	}
	else if (m_bSwitch == true)
	{
		if ((*pMotion)->pKeyInfo[m_nKeyInfoId]->nFlame < m_nFlameCnt)
		{
			m_nKeyInfoId++;			// 次のキーセットへ
			m_nFlameCnt = 0;		// フレームカウンタを初期化
		}
		if ((*pOldMotion)->pKeyInfo[m_nOldKeyInfoId]->nFlame < m_nOldFlameCnt)
		{
			m_nOldKeyInfoId++;
			m_nOldFlameCnt = 0;
		}
	}
	// キーセット数が規定を超えたら
	if (m_nKeyInfoId >= (*pMotion)->nNumKey)
	{
		m_nKeyInfoId = (*pMotion)->nNumKey - 1;
		if ((*pMotion)->nLoop == 0)
		{// ループフラグが設定されていない時
			m_bEndMotion = true;	// モーション終了フラグON
		}
		else if ((*pMotion)->nLoop == 1)
		{// ループフラグが設定されている時
			m_nKeyInfoId = 0;
			m_nFlameCnt = 0;		// フレームカウンタを初期化
		}
	}
	if (m_nOldKeyInfoId >= (*pOldMotion)->nNumKey)
	{
		m_nOldKeyInfoId = (*pOldMotion)->nNumKey - 1;
	}
}

/*******************************************************************************
* 関数名：void CMotionManager::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CMotionManager::Draw(void)
{
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		m_pModel[i]->Draw();
	}
}

/*******************************************************************************
* 関数名：void CMotionManager::Load( void )
*
* 引数	：
* 戻り値：
* 説明	：読み込み処理
*******************************************************************************/
void CMotionManager::Load(void)
{
	// アニメーションモデルのリソース読み込み
	for (int i = 0; i < DYNAMICMODEL_TYPE_MAX; i++)
	{
		FileOpen(m_DModel[i].fileName, i);
	}
}

/*******************************************************************************
* 関数名：void CMotionManager::Unload( void )
*
* 引数	：
* 戻り値：
* 説明	：解放処理
*******************************************************************************/
void CMotionManager::Unload(void)
{
	// デバイス、リソースの解放
	for (int i = 0; i < DYNAMICMODEL_TYPE_MAX; i++)
	{
		for (int j = 0; j < m_DModel[i].nNumModel; j++)
		{
			SAFE_RELEASE(*m_DModel[i].pParts[j]->pMesh);
			SAFE_DELETE(m_DModel[i].pParts[j]->pMesh);
			SAFE_RELEASE(*m_DModel[i].pParts[j]->pBuffMat);
			SAFE_DELETE(m_DModel[i].pParts[j]->pBuffMat);
			SAFE_DELETE(m_DModel[i].pParts[j]->pNumMat);
			SAFE_DELETE(m_DModel[i].pParts[j]);
		}
		for (int j = 0; j < m_DModel[i].nNumMotion; j++)
		{
			for (int k = 0; k < m_DModel[i].pMotion[j]->nNumKey; k++)
			{
				for (int l = 0; l < m_DModel[i].nNumModel; l++)
				{
					SAFE_DELETE(m_DModel[i].pMotion[j]->pKeyInfo[k]->pKey[l]);
				}
				SAFE_DELETE_ARRAY(m_DModel[i].pMotion[j]->pKeyInfo[k]->pKey);
				SAFE_DELETE(m_DModel[i].pMotion[j]->pKeyInfo[k]);
			}
			SAFE_DELETE_ARRAY(m_DModel[i].pMotion[j]->pKeyInfo);
			SAFE_DELETE(m_DModel[i].pMotion[j]);
		}
		SAFE_DELETE_ARRAY(m_DModel[i].pMotion);
		SAFE_DELETE_ARRAY(m_DModel[i].pParts);
		m_DModel[i].nNumModel = 0;
		m_DModel[i].nNumMotion = 0;
	}
}

/*******************************************************************************
* 関数名：int CMotionManager::FileOpen( char *FileName, int nData )
*
* 引数	：
* 戻り値：
* 説明	：テキストファイル格納処理
*******************************************************************************/
int CMotionManager::FileOpen(const char *filename, int nData)
{
	FILE *fp;
	char str[256];
	// テキストファイルの読み込み
	if ((fp = fopen(filename, "r")) == NULL)
	{// テキストファイルの読み込みの失敗
		return EXIT_FAILURE;
	}

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	int nId = 0,			// モデルID
		nMotionId = 0,		// モーションID
		nKeysetId = 0,		// キーセットID
		nKeyId = 0;			// キーフレームID

	while (fscanf(fp, "%s", str) != EOF)
	{
		if (strcmp(str, "NUM_MODEL") == 0)
		{// モデルの読み込み数
			fscanf(fp, "%*[NUM_MODEL =]%d", &m_DModel[nData].nNumModel);
			m_DModel[nData].pParts = new MODEL_PARTS*[m_DModel[nData].nNumModel];
			for (int nCnt = 0; nCnt < m_DModel[nData].nNumModel; nCnt++)
			{
				m_DModel[nData].pParts[nCnt] = new MODEL_PARTS;
				m_DModel[nData].pParts[nCnt]->pMesh = new LPD3DXMESH;
				m_DModel[nData].pParts[nCnt]->pBuffMat = new LPD3DXBUFFER;
				m_DModel[nData].pParts[nCnt]->pNumMat = new DWORD;
			}
		}
		else if (strcmp(str, "MODEL_FILENAME") == 0)
		{// モデルのファイル名
			char name[256];
			ZeroMemory(&m_DModel[nData].pParts[nId]->partsName, sizeof(m_DModel[nData].pParts[nId]->partsName));
			fscanf(fp, "%*[MODEL_FILENAME =]%s", &name);
			strcpy(m_DModel[nData].pParts[nId]->partsName, name);
			// Xファイルの読み込み
			D3DXLoadMeshFromX(&m_DModel[nData].pParts[nId]->partsName[0],	// 読み込みファイル名
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				m_DModel[nData].pParts[nId]->pBuffMat,			// マテリアル情報
				NULL,
				m_DModel[nData].pParts[nId]->pNumMat,			// マテリアルの数
				m_DModel[nData].pParts[nId]->pMesh);			// メッシュ情報
			nId++;
		}
		// キャラクターデータ /////
		else if (strcmp(str, "CHARACTERSET") == 0)
		{
			nId = 0;
			while (strcmp(str, "END_CHARACTERSET"))
			{
				fscanf(fp, "%s", &str);
				//	パーツデータ /////
				if (strcmp(str, "PARTSSET") == 0)
				{
					while (strcmp(str, "END_PARTSSET"))
					{
						fscanf(fp, "%s", &str);
						if (strcmp(str, "INDEX") == 0)
						{// モデルID
							fscanf(fp, "%*[INDEX =]%d", &m_DModel[nData].pParts[nId]->nIdx);
						}
						if (strcmp(str, "PARENT") == 0)
						{// 親のID
							fscanf(fp, "%*[PARENT =]%d", &m_DModel[nData].pParts[nId]->nParent);
						}
						if (strcmp(str, "POS") == 0)
						{// 位置
							Vector3 *pos = &m_DModel[nData].pParts[nId]->posOff;
							fscanf(fp, "%*[POS =]%f%f%f", &pos->x, &pos->y, &pos->z);
						}
						if (strcmp(str, "ROT") == 0)
						{// 回転値
							Vector3 *rot = &m_DModel[nData].pParts[nId]->rotOff;
							fscanf(fp, "%*[ROT =]%f%f%f", &rot->x, &rot->y, &rot->z);
						}
					}
					nId++;
				}
			}
		}
		// キャラクターデータここまで /////


		// モーションカウント /////
		else if (strcmp(str, "MOTIONSET") == 0)
		{// モーション数
			m_DModel[nData].nNumMotion++;
		}
	}

	// モーションの動的確保 /////
	m_DModel[nData].pMotion = new MOTION_INFO*[m_DModel[nData].nNumMotion];
	for (int nMotion = 0; nMotion < m_DModel[nData].nNumMotion; nMotion++)
	{
		m_DModel[nData].pMotion[nMotion] = new MOTION_INFO;
	}

	fseek(fp, SEEK_SET, 0);			// ファイルポインタを先頭に戻す

	// モーションデータの動的確保 /////
	nMotionId = 0;	// 変数初期化
	while (fscanf(fp, "%s", str) != EOF)
	{
		// モーションデータ /////
		if (strcmp(str, "MOTIONSET") == 0)
		{// モーション数
			nKeysetId = 0;	// 変数初期化
			while (strcmp(str, "END_MOTIONSET"))
			{
				fscanf(fp, "%s", &str);
				if (strcmp(str, "NUM_KEY") == 0)
				{// キーフレーム数
					int nNumKey;
					fscanf(fp, "%*[NUM_KEY =]%d", &m_DModel[nData].pMotion[nMotionId]->nNumKey);
					nNumKey = m_DModel[nData].pMotion[nMotionId]->nNumKey;
					m_DModel[nData].pMotion[nMotionId]->pKeyInfo = new KEY_INFO*[nNumKey];
					for (int i = 0; i < nNumKey; i++)
					{
						m_DModel[nData].pMotion[nMotionId]->pKeyInfo[i] = new KEY_INFO;
						m_DModel[nData].pMotion[nMotionId]->pKeyInfo[i]->nNumKeyParts = m_DModel[nData].nNumModel;
					}
				}
				if (strcmp(str, "KEYSET") == 0)
				{
					while (strcmp(str, "END_KEYSET"))
					{
						fscanf(fp, "%s", &str);
					}
					int nNumKeyParts = m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->nNumKeyParts;
					m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey = new KEY*[nNumKeyParts];
					for (int i = 0; i < nNumKeyParts; i++)
					{
						m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[i] = new KEY;
					}
					nKeysetId++;
				}
			}
			nMotionId++;
		}
	}
	// モーションデータの動的確保ここまで /////

	fseek(fp, SEEK_SET, 0);			// ファイルポインタを先頭に戻す

	// モーションの値格納 /////
	nMotionId = 0;	// 変数初期化
	while (fscanf(fp, "%s", str) != EOF)
	{
		// モーションデータ /////
		if (strcmp(str, "MOTIONSET") == 0)
		{// モーション数
			nKeysetId = 0;	// 変数初期化
			while (strcmp(str, "END_MOTIONSET"))
			{
				fscanf(fp, "%s", &str);
				if (strcmp(str, "LOOP") == 0)
				{// ループするかどうか
					fscanf(fp, "%*[LOOP =]%d", &m_DModel[nData].pMotion[nMotionId]->nLoop);
				}
				if (strcmp(str, "KEYSET") == 0)
				{
					nKeyId = 0;	// 変数初期化
					while (strcmp(str, "END_KEYSET"))
					{
						fscanf(fp, "%s", &str);
						if (strcmp(str, "END_KEYSET") == 0)
						{// ループ終了
							break;
						}
						while (strcmp(str, "END_KEY"))
						{
							fscanf(fp, "%s", &str);
							if (strcmp(str, "FRAME") == 0)
							{// フレーム数
								fscanf(fp, "%*[FRAME =]%d", &m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->nFlame);
							}
							if (strcmp(str, "POS") == 0)
							{// 位置
								fscanf(fp, "%*[POS =]%f %f %f", &m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->pos.x,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->pos.y,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->pos.z);
							}
							if (strcmp(str, "ROT") == 0)
							{// 回転角
								fscanf(fp, "%*[ROT =]%f %f %f", &m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->rot.x,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->rot.y,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->rot.z);
							}
						}
						nKeyId++;
					}
					nKeysetId++;
				}
			}
			nMotionId++;
		}
	}
	// モーションの値の格納ここまで /////

	fclose(fp);

	return 0;
}

/*******************************************************************************
* 関数名：void CMotionManager::ResetMotion( void )
*
* 引数	：
* 戻り値：
* 説明	：モーション再設定処理
*******************************************************************************/
void CMotionManager::ResetMotion(void)
{
	CModel **model;

	// 構造体ポインタの取得
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		// モデル情報の再設定
		model = &m_pModel[i];
		(*model)->Switch();
	}
}

/*******************************************************************************
* 関数名：CMotionManager *CMotionManager::Create(DYNAMICMODEL_TYPE type, D3DXMATRIX *pMtx)
*
* 引数	：
* 戻り値：
* 説明	：モーションマネージャー生成処理
*******************************************************************************/
CMotionManager *CMotionManager::Create(DYNAMICMODEL_TYPE type, D3DXMATRIX *pMtx)
{
	CMotionManager *manager = new CMotionManager;
	manager->SetMatrix(pMtx);
	manager->SetModelType(type);
	manager->Init();
	return manager;
}

/*******************************************************************************
* 関数名：void CMotionManager::SetMotion( int nId )
*
* 引数	：
* 戻り値：
* 説明	：モーション設定処理
*******************************************************************************/
void CMotionManager::SetMotion(int nId)
{
	if (m_nMotionId != nId)
	{
		m_nOldFlameCnt = m_nFlameCnt;
		m_nOldKeyInfoId = m_nKeyInfoId;
		if (m_nMotionId != -1)
		{
			m_nOldMotionId = m_nMotionId;
		}
		else
		{
			m_nOldMotionId = nId;
		}

		// モーション番号の設定
		m_nMotionId = nId;
		ResetMotion();
		// フレーム、キーセットのリセット
		m_nFlameCnt = 0;
		m_nKeyInfoId = 0;
		// 切り替えフラグ、終了フラグのセット
		m_bSwitch = true;
		m_bEndMotion = false;
	}
}

/*******************************************************************************
* 関数名：bool CMotionManager::EndMotion( void )
*
* 引数	：
* 戻り値：
* 説明	：モーション終了判定処理
*******************************************************************************/
bool CMotionManager::EndMotion(void)
{
	return m_bEndMotion;
}

/*******************************************************************************
* 関数名：void CMotionManager::SetModelColorAll(D3DXCOLOR color)
*
* 引数	：
* 戻り値：
* 説明	：全モデル色設定処理
*******************************************************************************/
void CMotionManager::SetModelColorAll(D3DXCOLOR color)
{
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		SetModelColor(i, color);
	}
}

/*******************************************************************************
* 関数名：void CMotionManager::SetModelColFlgAll(bool flg)
*
* 引数	：
* 戻り値：
* 説明	：全モデル色変更フラグ処理
*******************************************************************************/
void CMotionManager::SetModelColFlgAll(bool flg)
{
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		SetModelColFlg(i, flg);
	}
}