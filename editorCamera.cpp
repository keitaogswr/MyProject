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
#include "editorCamera.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define NEARZ				( 5.0f )		// Near値
#define FARZ				( 10000.0f )	// Far値
#define ANGLE				( 40.0f )		// カメラの視野角
#define MOVE_SPEED			( 10.0f )		// カメラの自走速度
#define ROT_SPEED			( 0.1f )		// カメラの自転速度
#define DISTANCE			( 1000.0f )		// カメラ視点と注視点の初期距離
#define HEIGHT				( 500.0f )		// カメラ視点の初期高さ

#define ROT_ATEEN			( 0.1f )		// カメラの回転減衰係数
#define MOVE_ATEEN_V		( 0.5f )		// 視点の速度減衰係数
#define MOVE_ATEEN_R		( 0.05f )		// 注視点の速度減衰係数
#define MOVE_COEFF_V		( 15.0f )		// 視点移動距離の倍率係数
#define MOVE_COEFF_R		( 8.0f )		// 注視点移動距離の倍率係数

#define TPS_ANGLE			( 0.3f )		// TPSモード時のカメラの位置角度
#define ROT_TPS_ATEEN		( 0.1f )		// カメラの回転減衰係数(TPS)

#define SHAKE_CNT			( 10 )			// 振動カウンタ

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CEditorCamera::CEditorCamera()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CEditorCamera::CEditorCamera()
{
}

/*******************************************************************************
* 関数名：CEditorCamera::~CEditorCamera()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CEditorCamera::~CEditorCamera()
{
}

/*******************************************************************************
* 関数名：void CEditorCamera::Init( void )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CEditorCamera::Init(void)
{
	// カメラ ///
	m_Pos = Vector3(0.0f, HEIGHT, -DISTANCE);
	m_PosR = Vector3(0.0f, 0.0f, 0.0f);
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
* 関数名：void CEditorCamera::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CEditorCamera::Uninit(void)
{

}

/*******************************************************************************
* 関数名：void CEditorCamera::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CEditorCamera::Update(void)
{
	float rad = 0.0f;
	if (CInput::GetKeyboardPress(DIK_Q))
	{
		m_Rot.y -= ROT_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_E))
	{
		m_Rot.y += ROT_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_R))
	{
		m_MoveV.y += MOVE_SPEED;
		m_MoveR.y += MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_F))
	{
		m_MoveV.y -= MOVE_SPEED;
		m_MoveR.y -= MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_T))
	{
		m_MoveR.y += MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_G))
	{
		m_MoveR.y -= MOVE_SPEED;
	}
	if (CInput::GetKeyboardPress(DIK_W) || CInput::GetKeyboardPress(DIK_D) ||
		CInput::GetKeyboardPress(DIK_A) || CInput::GetKeyboardPress(DIK_S))
	{// 移動キーを押していたら
		// 前移動
		if (CInput::GetKeyboardPress(DIK_W) && (!CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A)))
		{
			rad = m_Rot.y;
		}
		// 斜め右前移動
		else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_D))
		{
			rad = m_Rot.y + D3DX_PI * 0.25f;
		}
		// 斜め左前移動
		else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_A))
		{
			rad = m_Rot.y - D3DX_PI * 0.25f;
		}
		// 後ろ移動
		if (CInput::GetKeyboardPress(DIK_S) && !CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A))
		{
			rad = m_Rot.y + D3DX_PI;
		}
		// 斜め右後ろ移動
		else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_D))
		{
			rad = m_Rot.y + D3DX_PI * 0.75f;
		}
		// 斜め左後ろ移動
		else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_A))
		{
			rad = m_Rot.y - D3DX_PI * 0.75f;
		}
		// 右移動
		if (CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
		{
			rad = m_Rot.y + D3DX_PI * 0.5f;
		}
		// 左移動
		if (CInput::GetKeyboardPress(DIK_A) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
		{
			rad = m_Rot.y - D3DX_PI * 0.5f;
		}
		CManager::CheckRot(&rad); // 円周率チェック

		// 視点移動量の更新
		m_MoveV.x += sinf(rad) * MOVE_SPEED;
		m_MoveV.z += cosf(rad) * MOVE_SPEED;
	}

	/* 移動量、位置の更新 */
	// 移動量の減衰
	m_MoveV += (Vector3(0.0f, 0.0f, 0.0f) - m_MoveV) * MOVE_ATEEN_V;
	m_MoveR += (Vector3(0.0f, 0.0f, 0.0f) - m_MoveR) * MOVE_ATEEN_V;

	// 視点、注視点位置の更新
	m_Pos += m_MoveV;
	m_PosR.x = m_Pos.x + sinf(m_Rot.y) * DISTANCE;
	m_PosR.y += m_MoveR.y;
	m_PosR.z = m_Pos.z + cosf(m_Rot.y) * DISTANCE;
}

/*******************************************************************************
* 関数名：CEditorCamera *CEditorCamera::Create( void )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CEditorCamera *CEditorCamera::Create(void)
{
	CEditorCamera *camera;
	camera = new CEditorCamera;
	camera->Init();
	return camera;
}