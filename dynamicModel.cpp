/*******************************************************************************
* タイトル名：
* ファイル名：dynamicModel.cpp
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
#include "mode.h"
#include "game.h"
#include "motionManager.h"
#include "model.h"
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "scene3D.h"
#include "meshField.h"
#include "dynamicModel.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 50 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MOVE_SPEED		( 1.0f )		// 移動量
#define ROT_SPEED		( 0.05f )		// 回転量

#define ROT_ATEEN		( 0.1f )		// 回転量減衰係数
#define MOVE_ATEEN		( 0.15f )		// 移動量減衰係数

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CDynamicModel::CDynamicModel()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CDynamicModel::CDynamicModel( DRAWORDER DrawOrder, OBJTYPE ObjType ):CSceneX( DrawOrder, ObjType )
{
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Move = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rot = Vector3( 0.0f, 0.0f, 0.0f );
	m_RotN = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rad = Vector3( 0.0f, 0.0f, 0.0f );
	m_Texture = NULL;
	D3DXQuaternionIdentity( &m_Quaternion );
	D3DXQuaternionIdentity( &m_QuaterN );

	m_MotionManager = NULL;
}

/*******************************************************************************
* 関数名：CDynamicModel::~CDynamicModel()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CDynamicModel::~CDynamicModel()
{
}

/*******************************************************************************
* 関数名：void CDynamicModel::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CDynamicModel::Init( Vector3 pos )
{
	m_Pos = pos;
	int num = rand() % 2;
	if (num == 0)
	{
		m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_00, &m_MtxWorld);
	}
	else if (num == 1)
	{
		m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_01, &m_MtxWorld);
	}

	m_MotionManager->SetMotion(0);
}

/*******************************************************************************
* 関数名：void CDynamicModel::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CDynamicModel::Uninit( void )
{
	// 解放
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CDynamicModel::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CDynamicModel::Update( void )
{

	m_MotionManager->Update();
}

/*******************************************************************************
* 関数名：void CDynamicModel::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CDynamicModel::Draw( void )
{
	/* 変数定義 */
	D3DXMATRIX mtxRot, mtxTrans, mtxScl;				// 向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity( &m_MtxWorld );

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
	// 回転を反映
	D3DXMatrixRotationYawPitchRoll( &mtxRot, m_Rot.y, m_Rot.x, m_Rot.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxRot );
	// 位置を反映
	D3DXMatrixTranslation( &mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxTrans );
	m_MotionManager->Draw();
}

/*******************************************************************************
* 関数名：CDynamicModel *CDynamicModel::Create( Vector3 pos )
* 
* 引数	：
* 戻り値：CDynamicModel型のポインタ
* 説明	：生成処理
*******************************************************************************/
CDynamicModel *CDynamicModel::Create( Vector3 pos )
{
	CDynamicModel *model;
	model = new CDynamicModel;
	model->Init( pos );
	return model;
}

/*******************************************************************************
* 関数名：void CDynamicModel::SetModelMatrix(void)
*
* 引数	：
* 戻り値：
* 説明	：モデルのマトリクスの設定処理
*******************************************************************************/
void CDynamicModel::SetModelMatrix(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxRot, mtxTrans;				// 向き、ポジション

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// クォータニオンを反映
	D3DXMatrixRotationQuaternion(&mtxRot, &m_Quaternion);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	// モデルのマトリクスセット
	CModel *model;
	for (int i = 0; i < CMotionManager::GetDynamicModelInfo(m_MotionManager->GetModelType())->nNumModel; i++)
	{
		model = m_MotionManager->GetModel(i);
		model->SetMatrix();
	}
}