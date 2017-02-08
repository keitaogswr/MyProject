/*******************************************************************************
* タイトル名：
* ファイル名：player.cpp
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
#include "scene2D.h"
#include "scene3D.h"
#include "sceneX.h"
#include "dynamicModel.h"
#include "scene3D.h"
#include "billboard.h"
#include "bullet.h"
#include "diffusionBullet.h"
#include "shadow.h"
#include "effect.h"
#include "animationBoard.h"
#include "smoke.h"
#include "player.h"
#include "enemy.h"
#include "input.h"
#include "meshField.h"
#include "camera.h"
#include "orbit.h"
#include "reticle.h"
#include "stencilShadow.h"
#include "playerLifeGauge.h"
#include "explosion.h"
#include "sound.h"
#include "afterBurner.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define HEIGHT					( 50 )			// 銃口高さ
#define MOVE_SPEED				( 0.8f )		// 移動量
#define MOVE_SPEED_VEHICLE		( 1.2f )		// 移動量
#define JUMP_SPEED				( 13.0f )		// ジャンプ量
#define GRAVITY					( 0.5f )		// 重力
#define ROT_SPEED				( 0.05f )		// 回転量
#define ROT_AGREE				( 0.05f )

#define ROT_ATTEN				( 0.3f )		// 回転量減衰係数
#define MOVE_ATTEN				( 0.08f )		// 移動量減衰係数

#define SEARCH_LENG				( 1500 )		// ロックオン距離

#define DISTANCE_R				( 1000.0f )		// カメラ注視点の初期距離

#define SHADOW_RADIUS			( 20 )			// 影の半径
#define SHADOW_RADIUS_TRANSFORM	( 17 )			// 影の半径
#define SHADOW_RADIUS_VEHICLE	( 15 )			// 影の半径
#define SHADOW_HEIGHT			( 100 )			// 影の高さ

const int LIFE_MAX = 1000;
const int EXPLOSION_CNT = 30;
const int KEEP_MAX = 30;

const float COLLISION = 40.0f;

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CPlayer::CPlayer()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CPlayer::CPlayer(DRAWORDER DrawOrder, OBJTYPE ObjType) :CDynamicModel(DrawOrder, ObjType)
{
	m_State = PLAYERSTATE_WAIT;
	m_Mode = PLAYERMODE_HUMAN;
	m_Rot = Vector3(0.0f, D3DX_PI, 0.0f);
	m_RotN = Vector3(0.0f, D3DX_PI, 0.0f);
	m_bMove = false;
	m_bJump = false;
	m_bRockOn = false;
	m_bBullet = false;
	m_Shadow = NULL;
	m_Orbit[0] = m_Orbit[1] = NULL;
	m_nLife = LIFE_MAX;
	m_TargetPos = Vector3(0.0f, 0.0f, 0.0f);
	m_nExplosionCnt = 0;
	m_nKeep = KEEP_MAX;
	m_fCollision = COLLISION;
}

/*******************************************************************************
* 関数名：CPlayer::~CPlayer()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CPlayer::~CPlayer()
{

}

/*******************************************************************************
* 関数名：void CPlayer::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CPlayer::Init(Vector3 pos)
{
	m_Pos = pos;
	m_MotionManager = CMotionManager::Create(DYNAMICMODEL_TYPE_PLAYER_00, &m_MtxWorld);
	m_MotionManager->SetMotion(PLAYERSTATE_WAIT);
	m_Shadow = CStencilShadow::Create(pos);

	m_Orbit[0] = COrbit::Create();
	m_Orbit[1] = COrbit::Create();
}

/*******************************************************************************
* 関数名：void CPlayer::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CPlayer::Uninit(void)
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
* 関数名：void CPlayer::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CPlayer::Update(void)
{
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	CMeshField *meshField = game->GetMeshField();
	if (meshField == NULL)
	{
		return;
	}

	if (game->GetState() != CGame::STATE_START && game->GetState() != CGame::STATE_END)
	{
		// プレイヤーの操作
		Operate();
	}

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

	// ロックオンの更新
	UpdateRockOn();

	// 位置更新
	m_Pos += m_Move;

	// フィールドの移動制限 //
	float width, depth;
	width = meshField->GetWidthAll() * 0.5f - 1;
	depth = meshField->GetDepthAll() * 0.5f - 1;
	// 外に出たら押し返す
	if (m_Pos.x <= -width || m_Pos.x >= width)
	{
		m_Pos.x -= m_Move.x;
	}
	if (m_Pos.z <= -depth || m_Pos.z >= depth)
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

	if (m_bRockOn == true && m_bBullet == true)
	{// ターゲットしているときx軸を敵に向かせる
		Vector2 vec2 = Vector2(m_TargetPos.z - m_Pos.z, m_TargetPos.x - m_Pos.x);
		m_RotN.x = atan2f(m_TargetPos.y - m_Pos.y, vec2.Length());
		CManager::CheckRot(&m_RotN.x);
	}
	else
	{// ターゲットしていないときは正面を向かせる
		m_RotN.x = 0.0f;
	}

	/* 回転角の更新 */
	// 目的の角度の補正
	CManager::CheckRot(&m_RotN.x);
	CManager::CheckRot(&m_RotN.y);
	// 差分計算
	m_Rad.x = m_RotN.x - m_Rot.x;
	m_Rad.y = m_RotN.y - m_Rot.y;
	CManager::CheckRot(&m_Rad);
	// 向きを更新
	m_Rot.x += m_Rad.x * ROT_ATTEN;
	m_Rot.y += m_Rad.y * ROT_ATTEN;
	CManager::CheckRot(&m_Rot);

	/* 仰け反り耐久値の更新 */
	UpdateKeep();

	// モードごとの処理
	UpdateMode();

	/* 状態の更新、反映 */
	UpdateState();
	/* モーションの更新 */
	m_MotionManager->Update();
	/* オービットの更新 */
	SetOrbit();
	/* ライフバーの更新 */
	game->GetPlayerLifeGauge()->SetLife((float)m_nLife, (float)LIFE_MAX);
	if (m_nLife <= 0)
	{
		if (m_nExplosionCnt == 0)
		{
			game->SetState(CGame::STATE_END);
		}
		m_nLife = 0;
		m_nExplosionCnt++;
		if (m_nExplosionCnt % EXPLOSION_CNT == 0)
		{
			// モデルの座標を受け取り、爆発エフェクトを生成
			CModel *model = m_MotionManager->GetModel(0);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			CExplosion::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 100.0f, 100.0f);
			CSmoke::Create(pos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 70.0f, 70.0f);
		}
	}
}

/*******************************************************************************
* 関数名：void CPlayer::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CPlayer::Draw(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxRot, mtxTrans;				// 向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
	m_MotionManager->Draw();
}

/*******************************************************************************
* 関数名：void CPlayer::Load( void )
*
* 引数	：
* 戻り値：
* 説明	：読み込み処理
*******************************************************************************/
void CPlayer::Load(void)
{
}

/*******************************************************************************
* 関数名：void CPlayer::Unload( void )
*
* 引数	：
* 戻り値：
* 説明	：解放処理
*******************************************************************************/
void CPlayer::Unload(void)
{
}

/*******************************************************************************
* 関数名：void CPlayer::SetState(PLAYERSTATE state)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤーの状態設定処理
*******************************************************************************/
void CPlayer::SetState(PLAYERSTATE state)
{
	switch (state)
	{
	case PLAYERSTATE_WAIT:
		m_MotionManager->SetMotion(0);
		break;
	case PLAYERSTATE_WALK:
		// 前移動
		if (CInput::GetKeyboardPress(DIK_W) && (!CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A)))
		{
			m_MotionManager->SetMotion(1);
		}
		// 斜め右前移動
		else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_D))
		{
			m_MotionManager->SetMotion(1);
		}
		// 斜め左前移動
		else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_A))
		{
			m_MotionManager->SetMotion(1);
		}
		// 後ろ移動
		if (CInput::GetKeyboardPress(DIK_S) && !CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A))
		{
			m_MotionManager->SetMotion(6);
		}
		// 斜め右後ろ移動
		else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_D))
		{
			m_MotionManager->SetMotion(6);
		}
		// 斜め左後ろ移動
		else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_A))
		{
			m_MotionManager->SetMotion(6);
		}
		// 右移動
		if (CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
		{
			m_MotionManager->SetMotion(4);
		}
		// 左移動
		if (CInput::GetKeyboardPress(DIK_A) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
		{
			m_MotionManager->SetMotion(5);
		}
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
		m_MotionManager->SetMotion(10);
		break;
	case PLAYERSTATE_BOMBED:
		m_MotionManager->SetMotion(11);
		break;
	case PLAYERSTATE_VEHICLE_WAIT:
		m_MotionManager->SetMotion(9);
		break;
	case PLAYERSTATE_VEHICLE_WALK:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(7);
		}
		break;
	case PLAYERSTATE_VEHICLE_TRANSFORM:
		if (m_State != state)
		{
			m_MotionManager->SetMotion(8);
		}
		break;
	default:
		break;
	}

	m_State = state;
}

/*******************************************************************************
* 関数名：void CPlayer::SetMode(PLAYERMODE mode)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤーのモード設定処理
*******************************************************************************/
void CPlayer::SetMode(PLAYERMODE mode)
{
	m_Mode = mode;
}

/*******************************************************************************
* 関数名：void CPlayer::UpdateState(void)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤーの状態更新処理
*******************************************************************************/
void CPlayer::UpdateState(void)
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
			m_bBullet = false;
			break;
		case PLAYERSTATE_TRANSFORM:
			SetMode(PLAYERMODE_VEHICLE);
			SetState(PLAYERSTATE_VEHICLE_WAIT);
			break;
		case PLAYERSTATE_BOMBED:
			SetState(PLAYERSTATE_WAIT);
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
		if (nKeyFrame == 2 && nFrame == 0)
		{// モーションの指定フレームに達したら
			Vector3 vec;

			// モデルの座標を受け取り、腕から弾を発射する
			CModel *model = m_MotionManager->GetModel(8);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			m_bRockOn ? vec = m_TargetPos - pos : vec = Vector3(sinf(D3DX_PI + m_Rot.y), 0.0f, cosf(D3DX_PI + m_Rot.y));
			vec.Normalize();
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));

			model = m_MotionManager->GetModel(7);
			matrix = model->GetMatrix();
			pos = Vector3(matrix->_41, matrix->_42, matrix->_43);
			m_bRockOn ? vec = m_TargetPos - pos : vec = Vector3(sinf(D3DX_PI + m_Rot.y), 0.0f, cosf(D3DX_PI + m_Rot.y));
			vec.Normalize();
			CBullet::Create(Vector3(pos.x, pos.y, pos.z), vec, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));

			CSound::Play(CSound::SOUND_LABEL_SE_SHOT_000);
		}
	}
}

/*******************************************************************************
* 関数名：void CPlayer::Operate(void)
*
* 引数	：
* 戻り値：
* 説明	：プレイヤ操作処理
*******************************************************************************/
void CPlayer::Operate(void)
{
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();
	if (CManager::GetOperateMode() == CManager::OPMODE_PLAYER)
	{
		switch (m_Mode)
		{
		case PLAYERMODE_HUMAN:
		{// 人型時
			// 平行移動 //
			if (CInput::GetKeyboardPress(DIK_W) || CInput::GetKeyboardPress(DIK_D) ||
				CInput::GetKeyboardPress(DIK_A) || CInput::GetKeyboardPress(DIK_S))
			{// 移動キーを押していたら
				float rad = 0.0f;
				// 前移動
				if (CInput::GetKeyboardPress(DIK_W) && (!CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A)))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// 斜め右前移動
				else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_D))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// 斜め左前移動
				else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// 後ろ移動
				if (CInput::GetKeyboardPress(DIK_S) && !CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// 斜め右後ろ移動
				else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_D))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// 斜め左後ろ移動
				else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_A))
				{
					//m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// 右移動
				if (CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
				{
					//m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// 左移動
				if (CInput::GetKeyboardPress(DIK_A) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
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
			if (CInput::GetKeyboardTrigger(DIK_SPACE) && m_State != PLAYERSTATE_ATTACK)
			{
				SetState(PLAYERSTATE_ATTACK);
				m_bBullet = true;
			}
			/* 回転角の更新 */
			m_RotN.y = camera->m_Rot.y + D3DX_PI;	// TPS視点
			break;
		}
		case PLAYERMODE_VEHICLE:
		{// バイク時
			// 平行移動 //
			if (CInput::GetKeyboardPress(DIK_W) || CInput::GetKeyboardPress(DIK_D) ||
				CInput::GetKeyboardPress(DIK_A) || CInput::GetKeyboardPress(DIK_S))
			{// 移動キーを押していたら
				float rad = 0.0f;
				// 前移動
				if (CInput::GetKeyboardPress(DIK_W) && (!CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A)))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// 斜め右前移動
				else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// 斜め左前移動
				else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// 後ろ移動
				if (CInput::GetKeyboardPress(DIK_S) && !CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// 斜め右後ろ移動
				else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// 斜め左後ろ移動
				else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// 右移動
				if (CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// 左移動
				if (CInput::GetKeyboardPress(DIK_A) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
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
			if (CInput::GetKeyboardTrigger(DIK_SPACE))
			{
				CDiffusionBullet::Create(Vector3(m_Pos.x, m_Pos.y + HEIGHT, m_Pos.z), m_Rot);
			}
			break;
		}
		case PLAYERMODE_TRANSFORM:
		{// 変形中
			// 平行移動 //
			if (CInput::GetKeyboardPress(DIK_W) || CInput::GetKeyboardPress(DIK_D) ||
				CInput::GetKeyboardPress(DIK_A) || CInput::GetKeyboardPress(DIK_S))
			{// 移動キーを押していたら
				float rad = 0.0f;
				// 前移動
				if (CInput::GetKeyboardPress(DIK_W) && (!CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A)))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI;
					rad = camera->m_Rot.y;
				}
				// 斜め右前移動
				else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.75f;
					rad = camera->m_Rot.y + D3DX_PI * 0.25f;
				}
				// 斜め左前移動
				else if (CInput::GetKeyboardPress(DIK_W) && CInput::GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.75f;
					rad = camera->m_Rot.y - D3DX_PI * 0.25f;
				}
				// 後ろ移動
				if (CInput::GetKeyboardPress(DIK_S) && !CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y;
					rad = camera->m_Rot.y + D3DX_PI;
				}
				// 斜め右後ろ移動
				else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_D))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.25f;
					rad = camera->m_Rot.y + D3DX_PI * 0.75f;
				}
				// 斜め左後ろ移動
				else if (CInput::GetKeyboardPress(DIK_S) && CInput::GetKeyboardPress(DIK_A))
				{
					m_RotN.y = camera->m_Rot.y + D3DX_PI * 0.25f;
					rad = camera->m_Rot.y - D3DX_PI * 0.75f;
				}
				// 右移動
				if (CInput::GetKeyboardPress(DIK_D) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
				{
					m_RotN.y = camera->m_Rot.y - D3DX_PI * 0.5f;
					rad = camera->m_Rot.y + D3DX_PI * 0.5f;
				}
				// 左移動
				if (CInput::GetKeyboardPress(DIK_A) && !CInput::GetKeyboardPress(DIK_W) && !CInput::GetKeyboardPress(DIK_S))
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
		if (CInput::GetKeyboardTrigger(DIK_J) && m_bJump == false
			&& (m_State == PLAYERSTATE_WAIT || m_State == PLAYERSTATE_WALK))
		{
			m_Move.y = JUMP_SPEED;
			m_bJump = true;
			SetState(PLAYERSTATE_JUMP);
		}

		// 変形
		if (CInput::GetKeyboardTrigger(DIK_V))
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
* 関数名：void CPlayer::UpdateMode(void)
*
* 引数	：
* 戻り値：
* 説明	：モードごとの処理
*******************************************************************************/
void CPlayer::UpdateMode(void)
{
	CGame *game = dynamic_cast<CGame*>(CManager::GetMode());
	CCamera *camera = game->GetCamera();
	CMeshField *meshField = game->GetMeshField();
	float field = meshField->GetHeight(m_Pos);
	switch (m_Mode)
	{
	case PLAYERMODE_HUMAN:
	{
		/* 影の位置更新 */
		m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS, SHADOW_HEIGHT);
		if (m_bMove == true)
		{
			if (m_bJump == false)
			{
				CSmoke::Create(m_Pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);
			}
		}
		break;
	}
	case PLAYERMODE_VEHICLE:
	{
		/* 影の位置更新 */
		m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS_VEHICLE, SHADOW_HEIGHT);
		if (m_bMove == true)
		{
			CModel *model = m_MotionManager->GetModel(28);
			D3DXMATRIX *matrix = model->GetMatrix();
			Vector3 pos = Vector3(matrix->_41, matrix->_42 - 3.0f, matrix->_43);
			CSmoke::Create(pos, D3DXCOLOR(0.5f, 0.3f, 0.2f, 0.2f), 20, 20);
		}
		break;
	}
	case PLAYERMODE_TRANSFORM:
	{
		/* 影の位置更新 */
		m_Shadow->Set(Vector3(m_Pos.x, field, m_Pos.z), m_Pos, SHADOW_RADIUS_TRANSFORM, SHADOW_HEIGHT);
		break;
	}
	}
}

/*******************************************************************************
* 関数名：void CPlayer::SetOrbit(void)
*
* 引数	：
* 戻り値：
* 説明	：オービットの設定処理
*******************************************************************************/
void CPlayer::SetOrbit(void)
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

	/* オービットの更新 */
	// モデルのマトリクスを取得
	model = m_MotionManager->GetModel(33);
	D3DXMATRIX *matrix = model->GetMatrix();
	m_Orbit[0]->Set(Vector3(-1.0f, 2.0f, -1.0f), Vector3(-16.0f, -30.0f, -1.0f), *matrix);
	model = m_MotionManager->GetModel(34);
	matrix = model->GetMatrix();
	m_Orbit[1]->Set(Vector3(1.0f, 2.0f, -1.0f), Vector3(16.0f, -30.0f, -1.0f), *matrix);
}

/*******************************************************************************
* 関数名：void CPlayer::UpdateRockOn(void)
*
* 引数	：
* 戻り値：
* 説明	：ロックオン処理
*******************************************************************************/
void CPlayer::UpdateRockOn(void)
{
	Vector3 vec;
	float nearLeng = SEARCH_LENG;
	float length;
	if (CInput::GetKeyboardTrigger(DIK_R))
	{
		m_bRockOn == true ? m_bRockOn = false : m_bRockOn = true;
	}

	CCamera *camera = ((CGame*)CManager::GetMode())->GetCamera();
	if (m_bRockOn)
	{// ロックオンフラグがtrueのとき
		bool targetFlag = false;
		// カメラをロックオンモードに変更
		camera->SetCameraMode(CAMERAMODE_ROCKON);
		/* 敵の索敵 */
		CScene *scene = CScene::GetList(DRAWORDER_3D);
		CScene *next = NULL;
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			if (scene->GetObjType() == OBJTYPE_ENEMY)
			{
				vec = scene->GetPosition();
				vec = m_Pos - vec;
				length = vec.Length();
				if (length < SEARCH_LENG && length < nearLeng)
				{// 一番近い敵を更新
					nearLeng = length;
					m_TargetPos = ((CEnemy*)scene)->GetTargetPos();
					targetFlag = true;
				}
			}
			scene = next;
		}
		if (targetFlag == true)
		{// 敵が範囲内にいたら
			// 照準をロックオンモードにする
			CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
			reticle->SetRockOn(true);
		}
		else if (targetFlag == false)
		{// 敵が範囲内にいなかったら
			// ロックオンフラグをオフ
			m_bRockOn = false;
			// カメラを追従モードに変更
			camera->SetCameraMode(CAMERAMODE_SNEAK);
			// 照準を通常モードにする
			CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
			reticle->SetRockOn(false);
		}
	}
	else
	{// ロックオンフラグがfalseのとき
		// カメラを追従モードに変更
		camera->SetCameraMode(CAMERAMODE_SNEAK);
		// 照準を通常モードにする
		CReticle *reticle = ((CGame*)CManager::GetMode())->GetReticle();
		reticle->SetRockOn(false);
	}
}

/*******************************************************************************
* 関数名：void CPlayer::UpdateKeep(void)
*
* 引数	：
* 戻り値：
* 説明	：モーション耐久値更新処理
*******************************************************************************/
void CPlayer::UpdateKeep(void)
{
	if (m_Mode != PLAYERMODE_TRANSFORM)
	{
		if (m_nKeep <= 0)
		{
			SetState(PLAYERSTATE_BOMBED);
			m_nKeep = KEEP_MAX;
			m_bBullet = false;
		}
	}
}

/*******************************************************************************
* 関数名：CPlayer *CPlayer::Create( Vector3 pos )
*
* 引数	：
* 戻り値：CPlayer型のポインタ
* 説明	：生成処理
*******************************************************************************/
CPlayer *CPlayer::Create(Vector3 pos)
{
	CPlayer *player;
	player = new CPlayer;
	player->Init(pos);
	return player;
}