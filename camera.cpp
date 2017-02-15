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
#include "input.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "player.h"
#include "enemy.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define NEARZ		( 5.0f )				// Near値
#define FARZ		( 10000.0f )			// Far値
#define ANGLE		( 40.0f )				// カメラの視野角
#define MOVE_SPEED	( 3.0f )				// カメラの自走速度
#define ROT_SPEED	( 0.05f )				// カメラの自転速度
#define DISTANCE_R	( 1000.0f )				// カメラ注視点の初期距離
#define DISTANCE_V	( 220.0f )				// カメラ視点と初期距離
#define HEIGHT		( 50.0f )				// カメラ視点の初期高さ

#define ROT_ATTEN			( 0.1f )		// カメラの回転減衰係数
#define MOVE_ATTEN_V		( 0.05f )		// 視点の速度減衰係数
#define MOVE_ATTEN_R		( 0.05f )		// 注視点の速度減衰係数
#define MOVE_COEFF_V		( 15.0f )		// 視点移動距離の倍率係数
#define MOVE_COEFF_R		( 8.0f )		// 注視点移動距離の倍率係数
#define ROT_ATTEN_ROCKON	( 0.5f )		// カメラの回転減衰係数(ロックオンモード時)
#define MOVE_ATTEN_R_ROCKON	( 0.4f )		// 注視点の速度減衰係数(ロックオンモード時)

#define TPS_ANGLE			( 0.3f )		// TPSモード時のカメラの位置角度
#define ROT_TPS_ATTEN		( 0.1f )		// カメラの回転減衰係数(TPS)
//#define MOVE_TPS_ATTEN_V	( 0.15f )		// 視点の速度減衰係数(TPS)
//#define MOVE_TPS_ATTEN_R	( 0.15f )		// 注視点の速度減衰係数(TPS)
//#define MOVE_TPS_COEFF_V	( 8.0f )		// 視点移動距離の倍率係数(TPS)
//#define MOVE_TPS_COEFF_R	( 4.0f )		// 注視点移動距離の倍率係数(TPS)

#define SHAKE_CNT			( 10 )			// 画面揺れ時間
#define AMPLITUDE			( 30.0f )		// 画面の揺れ幅

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CCamera::CCamera()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CCamera::CCamera()
{
	m_Pos = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rot = Vector3( 0.0f, 0.0f, 0.0f );
	m_PosR = Vector3( 0.0f, 0.0f, 0.0f );
	m_VecU = Vector3( 0.0f, 0.0f, 0.0f );
	m_Rad = Vector3( 0.0f, 0.0f, 0.0f );
	m_RotN = Vector3( 0.0f, 0.0f, 0.0f );
	m_bShake = false;
	m_nCnt = 0;
	m_nMode = CAMERAMODE_SNEAK;

	m_MoveV = Vector3(0.0f, 0.0f, 0.0f);
	m_MoveR = Vector3(0.0f, 0.0f, 0.0f);
	m_fAmplitude = AMPLITUDE;
	m_nShakeTime = SHAKE_CNT;
}

/*******************************************************************************
* 関数名：CCamera::~CCamera()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CCamera::~CCamera()
{
}

/*******************************************************************************
* 関数名：void CCamera::Init( void )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CCamera::Init( void )
{
	// カメラ ///
	m_Pos = Vector3( 0.0f, HEIGHT, -DISTANCE_V );
	m_PosR = Vector3( 0.0f, 0.0f, 0.0f );
	m_VecU = Vector3( 0.0f, 1.0f, 0.0f );

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity( &m_MtxProjection );

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH( &m_MtxProjection,
								ANGLE * ( D3DX_PI / 180 ),
								( float )SCREEN_WIDTH / SCREEN_HEIGHT,
								NEARZ,
								FARZ );

	// プロジェクションマトリックスの設定
	pDevice->SetTransform( D3DTS_PROJECTION, &m_MtxProjection );

	// ビューマトリックスの初期化
	D3DXMatrixIdentity( &m_MtxView );
	D3DXMatrixLookAtLH( &m_MtxView,
						&m_Pos,
						&m_PosR,
						&m_VecU );
	pDevice->SetTransform( D3DTS_VIEW, &m_MtxView );
}

/*******************************************************************************
* 関数名：void CCamera::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CCamera::Uninit( void )
{

}

/*******************************************************************************
* 関数名：void CCamera::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CCamera::Update( void )
{
	// モデル構造体の取得
	CGame *game = ( CGame* )CManager::GetMode();
	CPlayer *player = game->GetPlayer();
	CInput *input = CManager::GetInput();

	Operate();
	
	// 画面を揺らす
	if (m_bShake)
	{
		m_nCnt++;

		m_PosR.x += m_fAmplitude * sinf(m_nCnt * D3DX_PI * 0.5f) * sinf(m_Rot.y + D3DX_PI);
		m_PosR.y += m_fAmplitude * sinf(m_nCnt * D3DX_PI * 0.5f);
		m_PosR.z += m_fAmplitude * sinf(m_nCnt * D3DX_PI * 0.5f) * cosf(m_Rot.y + D3DX_PI);
	}
	if (m_nCnt == m_nShakeTime)
	{
		m_bShake = false;
		m_nCnt = 0;
	}

	Vector3 pos = player->GetPosition();
	Vector3 move = player->GetMove();
	Vector3 rad;
	float radTps;
	CEnemy *target;
	Vector3 targetPos;
	float x, z;
	switch (m_nMode)
	{// カメラモードごとの処理
	case CAMERAMODE_SNEAK:
		// 回転角の更新
		rad = m_RotN - m_Rot;
		CManager::CheckRot(&rad);	// 円周率チェック
		m_Rot += rad * ROT_ATTEN;
		CManager::CheckRot(&m_Rot); // 円周率チェック

		// 注視点移動量の更新
		// 速度によって次の目的地を伸ばす
		m_MoveR.x = (move.x * MOVE_COEFF_V + pos.x
			- sinf(m_Rot.y + D3DX_PI) * DISTANCE_R - m_PosR.x) * MOVE_ATTEN_R;
		m_MoveR.y = (pos.y + HEIGHT - m_PosR.y) * MOVE_ATTEN_R;
		m_MoveR.z = (move.z * MOVE_COEFF_V + pos.z
			- cosf(m_Rot.y + D3DX_PI) * DISTANCE_R - m_PosR.z) * MOVE_ATTEN_R;

		// 注視点の位置更新
		m_PosR.x += m_MoveR.x;
		m_PosR.y += m_MoveR.y;
		m_PosR.z += m_MoveR.z;

		radTps = m_Rot.y - TPS_ANGLE;
		CManager::CheckRot(&radTps); // 円周率チェック

		// 視点移動量の更新
		m_MoveV.x = ((move.x * MOVE_COEFF_R + pos.x
			- sinf(radTps) * DISTANCE_V) - m_Pos.x) * MOVE_ATTEN_V;
		m_MoveV.y = (pos.y + HEIGHT - m_Pos.y) * MOVE_ATTEN_R;
		m_MoveV.z = ((move.z * MOVE_COEFF_R + pos.z
			- cosf(radTps) * DISTANCE_V) - m_Pos.z) * MOVE_ATTEN_V;

		// 視点位置の更新
		m_Pos.x += m_MoveV.x;
		m_Pos.y += m_MoveV.y;
		m_Pos.z += m_MoveV.z;
		break;
	case CAMERAMODE_ROCKON:
		// 回転角の更新
		x = m_PosR.x - pos.x;
		z = m_PosR.z - pos.z;
		m_RotN.y = atan2f(x, z);
		CManager::CheckRot(&m_RotN);	// 円周率チェック
		rad = m_RotN - m_Rot;
		CManager::CheckRot(&rad);		// 円周率チェック
		m_Rot += rad * ROT_ATTEN;
		CManager::CheckRot(&m_Rot);		// 円周率チェック

		// ターゲットの位置取得
		target = CEnemy::Get(player->GetTargetId());
		if (target)
		{
			targetPos = target->GetTargetPos();
		}
		else
		{
			return;
		}
		
		// 注視点移動量の更新
		// 速度によって次の目的地を伸ばす
		m_MoveR.x = (targetPos.x - m_PosR.x) * MOVE_ATTEN_R_ROCKON;
 		m_MoveR.y = (targetPos.y - m_PosR.y) * MOVE_ATTEN_R_ROCKON;
		m_MoveR.z = (targetPos.z - m_PosR.z) * MOVE_ATTEN_R_ROCKON;

		// 注視点の位置更新
		m_PosR.x += m_MoveR.x;
		m_PosR.y += m_MoveR.y;
		m_PosR.z += m_MoveR.z;

		radTps = m_Rot.y - TPS_ANGLE;
		CManager::CheckRot(&radTps); // 円周率チェック

		// 視点移動量の更新
		m_MoveV.x = ((pos.x - DISTANCE_V * sinf(radTps)) - m_Pos.x) * ROT_ATTEN_ROCKON;
		m_MoveV.y = (pos.y + HEIGHT - m_Pos.y) * ROT_ATTEN_ROCKON;
		m_MoveV.z = ((pos.z - DISTANCE_V * cosf(radTps)) - m_Pos.z) * ROT_ATTEN_ROCKON;

		// 視点位置の更新
		m_Pos.x += m_MoveV.x;
		m_Pos.y += m_MoveV.y;
		m_Pos.z += m_MoveV.z;
		break;
	default:
		break;
	}
}

/*******************************************************************************
* 関数名：void CCamera::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CCamera::Draw( void )
{

}

/*******************************************************************************
* 関数名：void CCamera::SetCamera( void )
* 
* 引数	：
* 戻り値：
* 説明	：カメラセット処理
*******************************************************************************/
void CCamera::Set( void )
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity( &m_MtxProjection );

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH( &m_MtxProjection,
								ANGLE * ( D3DX_PI / 180 ),
								( float )SCREEN_WIDTH / SCREEN_HEIGHT,
								NEARZ,
								FARZ );

	// プロジェクションマトリックスの設定
	pDevice->SetTransform( D3DTS_PROJECTION, &m_MtxProjection );

	// ビューマトリックスの初期化
	D3DXMatrixIdentity( &m_MtxView );
	D3DXMatrixLookAtLH( &m_MtxView,
						&m_Pos,
						&m_PosR,
						&m_VecU );
	pDevice->SetTransform( D3DTS_VIEW, &m_MtxView );
}

/*******************************************************************************
* 関数名：CCamera *CCamera::Create( void )
* 
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CCamera *CCamera::Create( void )
{
	CCamera *camera;
	camera = new CCamera;
	camera->Init();
	return camera;
}

/*******************************************************************************
* 関数名：Vector3 CCamera::GetPosition( void )
* 
* 引数	：
* 戻り値：
* 説明	：座標位置を返す
*******************************************************************************/
Vector3 CCamera::GetPosition( void )
{
	return m_Pos;
}

/*******************************************************************************
* 関数名：D3DXMATRIX *CCamera::GetMtxProj( void )
* 
* 引数	：
* 戻り値：
* 説明	：プロジェクションマトリクスを返す
*******************************************************************************/
D3DXMATRIX *CCamera::GetMtxProj( void )
{
	return &m_MtxProjection;
}

/*******************************************************************************
* 関数名：D3DXMATRIX *CCamera::GetMtxProj( void )
* 
* 引数	：
* 戻り値：
* 説明	：ビューマトリクスを返す
*******************************************************************************/
D3DXMATRIX *CCamera::GetMtxView( void )
{
	return &m_MtxView;
}

/*******************************************************************************
* 関数名：D3DXVECTOR3 CCamera::GetVecRV( void )
* 
* 引数	：
* 戻り値：
* 説明	：視点から注視点のベクトル返す
*******************************************************************************/
D3DXVECTOR3 CCamera::GetVecRV( void )
{
	D3DXVECTOR3 vector;
	vector = m_PosR - m_Pos;

	return vector;
}

/*******************************************************************************
* 関数名：void CCamera::SetShake(float amplitudeint, int time)
*
* 引数	：
* 戻り値：
* 説明	：カメラを揺らす
*******************************************************************************/
void CCamera::SetShake(float amplitudeint, int time)
{
	m_nCnt = 0;
	m_bShake = true;
	m_fAmplitude = amplitudeint;
	m_nShakeTime = time;
}

/*******************************************************************************
* 関数名：void CCamera::SetCameraMode(int id)
*
* 引数	：
* 戻り値：
* 説明	：カメラのモード設定
*******************************************************************************/
void CCamera::SetCameraMode(int id)
{
	m_nMode = id;
}

/*******************************************************************************
* 関数名：void CCamera::UpdateMode(void)
*
* 引数	：
* 戻り値：
* 説明	：カメラのモード設定
*******************************************************************************/
void CCamera::UpdateMode(void)
{

}

/*******************************************************************************
* 関数名：void CCamera::Operate(void)
*
* 引数	：
* 戻り値：
* 説明	：カメラの操作設定
*******************************************************************************/
void CCamera::Operate(void)
{
	CInput *input = CManager::GetInput();

	if (input->GetJoyStickConnected())
	{
		OperateJoyStick();
		return;
	}

	// 視点旋回
	if (input->GetKeyboardPress(DIK_LEFT))
	{
		m_RotN.y -= ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // 円周率チェック
	}
	// 視点旋回
	if (input->GetKeyboardPress(DIK_RIGHT))
	{
		m_RotN.y += ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // 円周率チェック
	}

	// 注視点上下
	if (input->GetKeyboardPress(DIK_T))
	{
		m_PosR.y += 30.0f;
	}
	if (input->GetKeyboardPress(DIK_G))
	{
		m_PosR.y -= 30.0f;
	}
}

/*******************************************************************************
* 関数名：void CCamera::Operate(void)
*
* 引数	：
* 戻り値：
* 説明	：カメラの操作設定
*******************************************************************************/
void CCamera::OperateJoyStick(void)
{
	CInput *input = CManager::GetInput();
	XINPUT_STATE *stateX = input->GetPressState();

	// 視点旋回
	if (stateX->Gamepad.sThumbRX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		m_RotN.y -= ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // 円周率チェック
	}
	// 視点旋回
	if (stateX->Gamepad.sThumbRX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		m_RotN.y += ROT_SPEED;
		CManager::CheckRot(&m_RotN.y); // 円周率チェック
	}
}