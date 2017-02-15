/*******************************************************************************
* タイトル名：
* ファイル名：sceneX.cpp
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
#include "model.h"
#include "motionManager.h"
#include "renderer.h"
#include "scene.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "scene3D.h"
#include "billboard.h"
#include "bullet.h"
#include "effect.h"
#include "animationBoard.h"
#include "smoke.h"
#include "diffusionBullet.h"
#include "shadow.h"
#include "player.h"
#include "playerT.h"
#include "input.h"
#include "scene3D.h"
#include "meshField.h"
#include "camera.h"
#include "stencilShadow.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define HEIGHT					( 50 )			// 銃口高さ
#define MOVE_SPEED				( 0.5f )		// 移動量
#define MOVE_SPEED_VEHICLE		( 1.0f )		// 移動量
#define JUMP_SPEED				( 10.0f )		// ジャンプ量
#define GRAVITY					( 0.5f )		// 重力
#define ROT_SPEED				( 0.05f )		// 回転量
#define ROT_AGREE				( 0.05f )

#define ROT_ATTEN				( 0.1f )		// 回転量減衰係数
#define MOVE_ATTEN				( 0.08f )		// 移動量減衰係数

#define SHADOW_RADIUS			( 30 )			// 影の半径
#define SHADOW_HEIGHT			( 100 )			// 影の高さ

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CPlayerT::CPlayerT()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CPlayerT::CPlayerT(DRAWORDER DrawOrder, OBJTYPE ObjType) :CPlayer(DrawOrder, ObjType)
{
	m_State = PLAYERSTATE_WAIT;
	m_Mode = PLAYERMODE_HUMAN;
	m_Rot = Vector3(0.0f, D3DX_PI, 0.0f);
	m_bMove = false;
	m_bJump = false;
	m_Shadow = NULL;
	m_Scl = Vector3(2.0f, 2.0f, 2.0f);
}

/*******************************************************************************
* 関数名：CPlayerT::~CPlayerT()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CPlayerT::~CPlayerT()
{

}

/*******************************************************************************
* 関数名：void CPlayerT::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CPlayerT::Init(Vector3 pos)
{
	m_Pos = pos;
	m_Shadow = CStencilShadow::Create(pos);
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_01, &m_MtxWorld);
	m_MotionManager->SetMotion(PLAYERSTATE_WAIT);
}

/*******************************************************************************
* 関数名：void CPlayerT::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CPlayerT::Uninit(void)
{
	// 解放
	if (m_MotionManager)
	{
		m_MotionManager->Uninit();
		delete m_MotionManager;
	}
	m_Shadow->Uninit();
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CPlayerT::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CPlayerT::Update(void)
{
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	CMeshField *meshField = game->GetMeshField();

	// プレイヤーの操作
	Operate();

	// 重力、上昇の加算
	m_Move.y -= GRAVITY;

	/* 移動量、位置の更新 */
	// 移動量の減衰
	m_Move.x += (0.0f - m_Move.x) * MOVE_ATTEN;
	//m_Move.y += (0.0f - m_Move.y) * MOVE_ATTEN;
	m_Move.z += (0.0f - m_Move.z) * MOVE_ATTEN;

	// 接触している地面の法線を取得
	Vector3 normal = meshField->GetNormal(&m_Pos);
	// 地面の傾きを移動量に加算
	if (normal.x > 0.6f || normal.x < -0.6f)
	{
		m_Move.x += normal.x;
	}
	if (normal.z > 0.6f || normal.z < -0.6f)
	{
		m_Move.z += normal.z;
	}

	if (D3DXVec3Length(&normal) > 0 && m_Mode == PLAYERMODE_VEHICLE)
	{
		// バイク時はモデルを床の傾きに合わせてクォータニオンで回転させる
		D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXQUATERNION quat = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

		D3DXVec3Cross(&axis, &Vector3(0.0f, 1.0f, 0.0f), &normal);				// 上方向と床の法線で外積を求め、軸のベクトルを求める

		float sita = asinf(D3DXVec3Length(&axis));								// 軸から角度を求める
		D3DXQuaternionRotationAxis(&quat, &axis, sita);							// 軸から回転させるクォータニオンを求める

		D3DXQuaternionSlerp(&m_Quaternion, &m_Quaternion, &quat, 0.1f);			// 現在のクォータニオンと目的のクォータニオンの補間
	}
	else
	{// プレイヤーが床の外にいたらリセット
		m_Quaternion = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
		//m_bJump = true;
	}

	// 位置更新
	m_Pos += m_Move;

	// フィールドの移動制限 //
	float width, depth;
	width = meshField->GetWidthAll() * 0.5f;
	depth = meshField->GetDepthAll() * 0.5f;
	// 外に出たら押し返す
	if (m_Pos.x < -width || m_Pos.x > width)
	{
		m_Pos.x -= m_Move.x;
	}
	if (m_Pos.z < -depth || m_Pos.z > depth)
	{
		m_Pos.z -= m_Move.z;
	}

	// プレイヤーの高さを地面に合わせる
	float field = meshField->GetHeight(m_Pos);
	if (m_Pos.y <= field)
	{
		m_Pos.y = field;
		m_bJump = false;
	}

	/* 回転角の更新 */
	// 目的の角度の補正
	CManager::CheckRot(&m_RotN.y);
	// 差分計算
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad.y);
	// 向きを更新
	m_Rot.y += m_Rad.y * ROT_ATTEN;
	CManager::CheckRot(&m_Rot.y);

	// モードごとの処理
	UpdateMode();

	/* 影の位置更新 */
	m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);

	/* 状態の更新、反映 */
	UpdateState();
	/* モーションの更新 */
	m_MotionManager->Update();
}

/*******************************************************************************
* 関数名：void CPlayerT::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CPlayerT::Draw(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxRot, mtxTrans, mtxScl;				// 向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);

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
	m_MotionManager->Draw();
}

/*******************************************************************************
* 関数名：void CPlayerT::SetState(PLAYERSTATE state)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤーの状態設定処理
*******************************************************************************/
void CPlayerT::SetState(PLAYERSTATE state)
{
	switch (state)
	{
	case PLAYERSTATE_WAIT:
		m_MotionManager->SetMotion(0);
		break;
	case PLAYERSTATE_WALK:
		m_MotionManager->SetMotion(1);
		break;
	case PLAYERSTATE_ATTACK:
		m_MotionManager->SetMotion(2);
		break;
	case PLAYERSTATE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(3);
		}
		break;
	case PLAYERSTATE_JUMP:
		m_MotionManager->SetMotion(1);
		break;
	case PLAYERSTATE_VEHICLE_WAIT:
		m_MotionManager->SetMotion(4);
		break;
	case PLAYERSTATE_VEHICLE_WALK:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(5);
		}
		break;
	case PLAYERSTATE_VEHICLE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(6);
		}
		break;
	default:
		break;
	}

	m_State = state;
}

/*******************************************************************************
* 関数名：void CPlayerT::SetMode(PLAYERMODE mode)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤーのモード設定処理
*******************************************************************************/
void CPlayerT::SetMode(PLAYERMODE mode)
{
	m_Mode = mode;
}

/*******************************************************************************
* 関数名：void CPlayerT::UpdateState(void)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤーの状態更新処理
*******************************************************************************/
void CPlayerT::UpdateState(void)
{
	if (m_MotionManager->EndMotion())
	{// モーションが終了したら
		switch (m_State)
		{
		case PLAYERSTATE_WAIT:

			break;
		case PLAYERSTATE_WALK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_ATTACK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_TRANSFORM:
			SetMode(PLAYERMODE_VEHICLE);
			SetState(PLAYERSTATE_VEHICLE_WAIT);
			break;
		case PLAYERSTATE_VEHICLE_WAIT:

			break;
		case PLAYERSTATE_VEHICLE_WALK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_VEHICLE_TRANSFORM:
			SetMode(PLAYERMODE_HUMAN);
			SetState(PLAYERSTATE_WAIT);
			break;
		default:
			break;
		}
	}

	if (!m_bMove)
	{// 動いていなければ待機状態にする
		switch (m_State)
		{
		case PLAYERSTATE_WALK:
			SetState(PLAYERSTATE_WAIT);
			break;
		case PLAYERSTATE_VEHICLE_WALK:
			SetState(PLAYERSTATE_VEHICLE_WAIT);
			break;
		default:
			break;
		}
	}
	if (!m_bJump && !m_bMove)
	{// ジャンプしていなければ待機状態にする
		switch (m_State)
		{
		case PLAYERSTATE_JUMP:
			SetState(PLAYERSTATE_DAMP);
			break;
		case PLAYERSTATE_DAMP:
			SetState(PLAYERSTATE_WAIT);
			break;
		default:
			break;
		}
	}
	if (m_State == PLAYERSTATE_ATTACK)
	{
		int nKeyFrame = m_MotionManager->GetKeyInfoId();
		int nFrame = m_MotionManager->GetFrame();
		if (nKeyFrame == 3 && nFrame == 0)
		{
			Vector3 vec;
			vec.x = sinf(m_Rot.y + D3DX_PI);
			vec.y = 0.0f;
			vec.z = cosf(m_Rot.y + D3DX_PI);
			vec.Normalize();

			CModel *model = m_MotionManager->GetModel(26);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));

			model = m_MotionManager->GetModel(27);
			matrix = model->GetMatrix();
			pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
}

/*******************************************************************************
* 関数名：void CPlayerT::Operate(void)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤ操作処理
*******************************************************************************/
void CPlayerT::Operate(void)
{
	CInput *input = CManager::GetInput();
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	if (CManager::GetOperateMode() == CManager::OPMODE_PLAYER)
	{
		switch (m_Mode)
		{
		case PLAYERMODE_HUMAN:
		{// 人型時
		 // 平行移動 //
			if (input->GetKeyboardPress(DIK_W) || input->GetKeyboardPress(DIK_D) ||
				input->GetKeyboardPress(DIK_A) || input->GetKeyboardPress(DIK_S))
			{// 移動キーを押していたら
				float rad = 0.0f;
				// 前移動
				if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// 斜め右前移動
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// 斜め左前移動
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// 後ろ移動
				if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// 斜め右後ろ移動
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// 斜め左後ろ移動
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// 右移動
				if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// 左移動
				if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.5f;
					rad = camera->m_Rot.y - D3DX_PI * 0.5f;
				}

				m_Move.x += sinf(rad) * MOVE_SPEED;
				m_Move.z += cosf(rad) * MOVE_SPEED;
				m_bMove = true;

				if (m_State != PLAYERSTATE_ATTACK)
				{
					SetState(PLAYERSTATE_WALK);
				}
			}
			else
			{// 移動キーを押していなかったら
				m_bMove = false;
			}
			// 弾発射
			if (input->GetKeyboardTrigger(DIK_SPACE) && m_State != PLAYERSTATE_ATTACK)
			{
				SetState(PLAYERSTATE_ATTACK);
				
			}
			
			/* 回転角の更新 */
			m_RotN.y = camera->m_Rot.y + D3DX_PI;	// TPS視点
			break;
		}
		case PLAYERMODE_VEHICLE:
		{// トラック時
		 // 平行移動 //
			if (input->GetKeyboardPress(DIK_W) || input->GetKeyboardPress(DIK_D) ||
				input->GetKeyboardPress(DIK_A) || input->GetKeyboardPress(DIK_S))
			{// 移動キーを押していたら
				float rad = 0.0f;
				// 前移動
				if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// 斜め右前移動
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// 斜め左前移動
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// 後ろ移動
				if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// 斜め右後ろ移動
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// 斜め左後ろ移動
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// 右移動
				if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// 左移動
				if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.5f;
					rad = camera->m_Rot.y - D3DX_PI * 0.5f;
				}

				m_Move.x += sinf(rad) * MOVE_SPEED_VEHICLE;
				m_Move.z += cosf(rad) * MOVE_SPEED_VEHICLE;
				m_bMove = true;

				SetState(PLAYERSTATE_VEHICLE_WALK);
			}
			else
			{// 移動キーを押していなかったら
				m_bMove = false;
			}
			// 弾発射
			if (input->GetKeyboardTrigger(DIK_SPACE))
			{
				CDiffusionBullet::Create(Vector3(m_Pos.x, m_Pos.y + HEIGHT, m_Pos.z), m_Rot);
			}
			break;
		}
		case PLAYERMODE_TRANSFORM:
		{// 変形中
		 // 平行移動 //
			if (input->GetKeyboardPress(DIK_W) || input->GetKeyboardPress(DIK_D) ||
				input->GetKeyboardPress(DIK_A) || input->GetKeyboardPress(DIK_S))
			{// 移動キーを押していたら
				float rad = 0.0f;
				// 前移動
				if (input->GetKeyboardPress(DIK_W) && (!input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A)))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// 斜め右前移動
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// 斜め左前移動
				else if (input->GetKeyboardPress(DIK_W) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// 後ろ移動
				if (input->GetKeyboardPress(DIK_S) && !input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// 斜め右後ろ移動
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// 斜め左後ろ移動
				else if (input->GetKeyboardPress(DIK_S) && input->GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// 右移動
				if (input->GetKeyboardPress(DIK_D) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// 左移動
				if (input->GetKeyboardPress(DIK_A) && !input->GetKeyboardPress(DIK_W) && !input->GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.5f;
					rad = camera->m_Rot.y - D3DX_PI * 0.5f;
				}

				m_Move.x += sinf(rad) * MOVE_SPEED;
				m_Move.z += cosf(rad) * MOVE_SPEED;
				m_bMove = true;

			}
			else
			{// 移動キーを押していなかったら
				m_bMove = false;
			}
			break;
		}
		default:
			break;
		}
		// ジャンプ
		if (input->GetKeyboardTrigger(DIK_LSHIFT) && m_bJump == false
			&& (m_State == PLAYERSTATE_WAIT || m_State == PLAYERSTATE_WALK))
		{
			m_Move.y = JUMP_SPEED;
			m_bJump = true;
			SetState(PLAYERSTATE_JUMP);
		}

		// 変形
		if (input->GetKeyboardTrigger(DIK_V))
		{
			switch (m_Mode)
			{// モードごとの処理
			case PLAYERMODE_HUMAN:
				SetState(PLAYERSTATE_TRANSFORM);
				SetMode(PLAYERMODE_TRANSFORM);
				break;
			case PLAYERMODE_VEHICLE:
				SetState(PLAYERSTATE_VEHICLE_TRANSFORM);
				SetMode(PLAYERMODE_TRANSFORM);
				break;
			default:
				break;
			}
		}
	}
}

/*******************************************************************************
* 関数名：void CPlayerT::UpdateMode(void)
*
* 引数	：
* 戻り値：
* 説明	：モードごとの処理
*******************************************************************************/
void CPlayerT::UpdateMode(void)
{
	switch (m_Mode)
	{
	case PLAYERMODE_HUMAN:
	{
		break;
	}
	case PLAYERMODE_VEHICLE:
	{
		if (m_bMove == true)
		{
			CModel *model = m_MotionManager->GetModel(30);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42 - 3.0f, matrix->_43);
			CSmoke::Create(pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);

			model = m_MotionManager->GetModel(31);
			matrix = model->GetMatrix();
			pos = Vector3(matrix->_41, matrix->_42 - 3.0f, matrix->_43);
			CSmoke::Create(pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);
		}
		break;
	}
	case PLAYERMODE_TRANSFORM:
	{
		break;
	}
	}
}

/*******************************************************************************
* 関数名：CPlayerT *CPlayerT::Create( Vector3 pos )
*
* 引数	：
* 戻り値：CPlayerT型のポインタ
* 説明	：生成処理
*******************************************************************************/
CPlayerT *CPlayerT::Create(Vector3 pos)
{
	CPlayerT *player;
	player = new CPlayerT;
	player->Init(pos);
	return player;
}