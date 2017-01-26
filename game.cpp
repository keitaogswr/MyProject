/*******************************************************************************
* タイトル名：
* ファイル名：game.cpp
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
#include "fade.h"
#include "title.h"
#include "result.h"
#include "pause.h"
#include "sound.h"
#include "input.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "scene2D.h"
#include "scene3D.h"
#include "sceneX.h"
#include "motionManager.h"
#include "dynamicModel.h"
#include "player.h"
#include "playerT.h"
#include "enemy.h"
#include "bossEnemy.h"
#include "meshField.h"
#include "skyDome.h"
#include "skySphere.h"
#include "field.h"
#include "billboard.h"
#include "treeObject.h"
#include "time.h"
#include "game.h"
#include "bullet.h"
#include "shadow.h"
#include "effect.h"
#include "reticle.h"
#include "barrier.h"
#include "playerLifeGauge.h"
#include "texture.h"
#include "sound.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const int START_CNT = 180;
const int END_CNT = 180;

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CGame::CGame()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CGame::CGame()
{
	m_bMultithread = true;
	m_Camera = NULL;
	m_Light = NULL;
	m_MeshField = NULL;
	m_Player = NULL;
	m_pReticle = NULL;
	m_pPlayerLifeGauge = NULL;
	m_pLogo = NULL;
	m_bPause = false;
	m_State = STATE_START;
	m_nStateCnt = 0;
}

/*******************************************************************************
* 関数名：CGame::~CGame()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CGame::~CGame()
{
}

/*******************************************************************************
* 関数名：void CGame::Init( void )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CGame::Init(void)
{
	CScene *scene;
	// 乱数生成
	srand((unsigned)time(NULL));

	// モーションデータ読み込み
	CMotionManager::Load();

	// 各オブジェクト生成処理
	//scene = CField::Create( Vector3( 0.0f, 0.0f, 0.0f ) );
	m_MeshField = CMeshField::Create(Vector3(0.0f, 0.0f, 0.0f), 100, 100, 64, 64);
	scene = CSkySphere::Create(Vector3(0.0f, 0.0f, 0.0f), 7000.0f, 16, 16);
	m_Player = CPlayer::Create(Vector3(10.0f, 1000.0f, 100.0f));

	CEnemy::Load();

	m_pReticle = CReticle::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f));
	scene = CTime::Create(Vector3(SCREEN_WIDTH / 2, 50.0f, 0.0f));
	m_pPlayerLifeGauge = CPlayerLifeGauge::Create(Vector3(300.0f, 50.0f, 0.0f));
	CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), SCREEN_WIDTH, SCREEN_HEIGHT, TEXTURE_TYPE_GAMEUI_000);
	m_pLogo = CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), 600.0f, 200.0f, TEXTURE_TYPE_GAME_START_000);
	// カメラ生成
	m_Camera = CCamera::Create();
	// ライト生成
	m_Light = CLight::Create();

	// リソース読み込み
	CTreeObject::LoadFile();

	CPause::Init();
	// サウンド再生
	CSound::Play(CSound::SOUND_LABEL_BGM_001);
}

/*******************************************************************************
* 関数名：void CGame::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CGame::Uninit(void)
{
	// リソース解放
	CPause::Uninit();
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	CScene::UninitAll();
	m_Player = NULL;
	m_MeshField = NULL;
	m_pReticle = NULL;
	m_pPlayerLifeGauge = NULL;
	m_pLogo = NULL;
	CMotionManager::Unload();

	CSound::Stop();
}

/*******************************************************************************
* 関数名：void CGame::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CGame::Update(void)
{
	StateUpdate();
	if (CInput::GetKeyboardTrigger(DIK_P))
	{
		m_bPause = m_bPause == true ? false : true;
	}
	if (!m_bPause)
	{
		m_Camera->Update();

		CScene::UpdateAll();
#ifdef _DEBUG
		if (CInput::GetKeyboardTrigger(DIK_RETURN))
		{
			CFade::Start(new CResult);
		}
		// 敵の消去
		CScene *scene = CScene::GetList(DRAWORDER_3D);
		CScene *next = NULL;
		if (CInput::GetKeyboardTrigger(DIK_DELETE))
		{
			while (scene != NULL)
			{
				next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
				if (scene->GetObjType() == OBJTYPE_ENEMY)
				{
					scene->SetDeleteFlg(true);
				}
				scene = next;
			}
		}
#endif
	}
	else
	{
		CPause::Update();
	}
}

/*******************************************************************************
* 関数名：void CGame::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CGame::Draw(void)
{
	m_Camera->Set();
	CScene::DrawAll();
	if (m_bPause)
	{
		CPause::Draw();
	}
}

/*******************************************************************************
* 関数名：CCamera *CGame::GetCamera( void )
*
* 引数	：
* 戻り値：
* 説明	：カメラ取得処理
*******************************************************************************/
CCamera *CGame::GetCamera(void)
{
	return m_Camera;
}

/*******************************************************************************
* 関数名：CMeshField *CGame::GetMeshField( void )
*
* 引数	：
* 戻り値：
* 説明	：メッシュフィールド取得処理
*******************************************************************************/
CMeshField *CGame::GetMeshField(void)
{
	return m_MeshField;
}

/*******************************************************************************
* 関数名：CPlayer *CGame::GetPlayer( void )
*
* 引数	：
* 戻り値：
* 説明	：プレイヤー取得処理
*******************************************************************************/
CPlayer *CGame::GetPlayer(void)
{
	return m_Player;
}

/*******************************************************************************
* 関数名：void CGame::StateUpdate(void)
*
* 引数	：
* 戻り値：
* 説明	：ゲーム状態遷移処理
*******************************************************************************/
void CGame::StateUpdate(void)
{
	int cnt = 0;
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	switch (m_State)
	{
	case STATE_START:
		m_nStateCnt++;
		if (m_nStateCnt > START_CNT)
		{
			SetState(STATE_NORMAL);
			m_pLogo->SetDeleteFlg(true);
		}
		break;
	case STATE_NORMAL:
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			if (scene->GetObjType() == OBJTYPE_ENEMY)
			{
				cnt++;
			}
			scene = next;
		}
		if (cnt == 0)
		{
			SetState(STATE_BOSS);
			CBossEnemy::Create(Vector3(0.0f, 1000.0f, 0.0f));
		}
		break;
	case STATE_BOSS:

		break;
	case STATE_END:
		if (m_nStateCnt == 0)
		{
			m_pLogo = CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), 600.0f, 200.0f, TEXTURE_TYPE_FINISH_000);
		}
		m_nStateCnt++;
		if (m_nStateCnt > END_CNT)
		{
			CFade::Start(new CResult);
		}
		break;
	default:
		break;
	}
}