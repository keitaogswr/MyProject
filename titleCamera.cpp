/*******************************************************************************
* タイトル名：
* ファイル名：camera.cpp
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
#include "mode.h"
#include "game.h"
#include "scene.h"
#include "camera.h"
#include "titleCamera.h"
#include "input.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "player.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define NEARZ		( 5.0f )				// Near値
#define FARZ		( 10000.0f )			// Far値
#define ANGLE		( 40.0f )				// カメラの視野角
#define MOVE_SPEED	( 3.0f )				// カメラの自走速度
#define ROT_SPEED	( 0.01f )				// カメラの自転速度
#define DISTANCE	( 150.0f )				// カメラ視点と注視点の初期距離
#define HEIGHT		( 35.0f )				// カメラ視点の初期高さ

#define ROT_ATTEN			( 0.1f )		// カメラの回転減衰係数
#define MOVE_ATTEN_V		( 0.05f )		// 視点の速度減衰係数
#define MOVE_ATTEN_R		( 0.05f )		// 注視点の速度減衰係数
#define MOVE_COEFF_V		( 15.0f )		// 視点移動距離の倍率係数
#define MOVE_COEFF_R		( 8.0f )		// 注視点移動距離の倍率係数

#define TPS_ANGLE			( 0.3f )		// TPSモード時のカメラの位置角度
#define ROT_TPS_ATTEN		( 0.1f )		// カメラの回転減衰係数(TPS)
//#define MOVE_TPS_ATTEN_V	( 0.15f )		// 視点の速度減衰係数(TPS)
//#define MOVE_TPS_ATTEN_R	( 0.15f )		// 注視点の速度減衰係数(TPS)
//#define MOVE_TPS_COEFF_V	( 8.0f )		// 視点移動距離の倍率係数(TPS)
//#define MOVE_TPS_COEFF_R	( 4.0f )		// 注視点移動距離の倍率係数(TPS)

#define SHAKE_CNT			( 10 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CTitleCamera::CTitleCamera()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CTitleCamera::CTitleCamera()
{
}

/*******************************************************************************
* 関数名：CTitleCamera::~CTitleCamera()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CTitleCamera::~CTitleCamera()
{
}

/*******************************************************************************
* 関数名：void CTitleCamera::Init( void )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CTitleCamera::Init(void)
{
	// カメラ ///
	m_Pos = Vector3(0.0f, HEIGHT, -DISTANCE);
	m_PosR = Vector3(0.0f, HEIGHT, 0.0f);
	m_VecU = Vector3(0.0f, 1.0f, 0.0f);

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_MtxProjection,
		ANGLE * (D3DX_PI / 180),
		(float)SCREEN_WIDTH / SCREEN_HEIGHT,
		NEARZ,
		FARZ);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_MtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxView);
	D3DXMatrixLookAtLH(&m_MtxView,
		&m_Pos,
		&m_PosR,
		&m_VecU);
	pDevice->SetTransform(D3DTS_VIEW, &m_MtxView);
}

/*******************************************************************************
* 関数名：void CTitleCamera::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CTitleCamera::Uninit(void)
{

}

/*******************************************************************************
* 関数名：void CTitleCamera::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CTitleCamera::Update(void)
{
	m_Rot.y += ROT_SPEED;

	CManager::CheckRot(&m_Rot); // 円周率チェック

	// 視点移動量の更新
	m_MoveV.x = sinf(m_Rot.y) * DISTANCE;
	m_MoveV.z = cosf(m_Rot.y) * DISTANCE;

	// 視点位置の更新
	m_Pos.x = m_MoveV.x;
	m_Pos.z = m_MoveV.z;
}

/*******************************************************************************
* 関数名：void CTitleCamera::SetCamera( void )
*
* 引数	：
* 戻り値：
* 説明	：カメラセット処理
*******************************************************************************/
void CTitleCamera::Set(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_MtxProjection,
		ANGLE * (D3DX_PI / 180),
		(float)SCREEN_WIDTH / SCREEN_HEIGHT,
		NEARZ,
		FARZ);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_MtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxView);
	D3DXMatrixLookAtLH(&m_MtxView,
		&m_Pos,
		&m_PosR,
		&m_VecU);
	pDevice->SetTransform(D3DTS_VIEW, &m_MtxView);
}

/*******************************************************************************
* 関数名：CTitleCamera *CTitleCamera::Create( void )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CTitleCamera *CTitleCamera::Create(void)
{
	CTitleCamera *camera;
	camera = new CTitleCamera;
	camera->Init();
	return camera;
}