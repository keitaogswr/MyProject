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

/*******************************************************************************
* マクロ定義
*******************************************************************************/

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
	scene = CSkySphere::Create(Vector3(0.0f, 0.0f, 0.0f), 7000.0f, 8, 16);
	m_Player = CPlayer::Create(Vector3(10.0f, 1000.0f, 300.0f));

	CEnemy::Load();
	scene = CBossEnemy::Create(Vector3(0.0f, 1000.0f, 0.0f));

	m_pReticle = CReticle::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f));
	scene = CTime::Create(Vector3(SCREEN_WIDTH / 2, 50.0f, 0.0f));
	// カメラ生成
	m_Camera = CCamera::Create();
	// ライト生成
	m_Light = CLight::Create();

	// リソース読み込み
	CTreeObject::LoadFile();
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
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	CScene::UninitAll();
	m_Player = NULL;
	m_MeshField = NULL;
	m_pReticle = NULL;
	CMotionManager::Unload();
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
	m_Camera->Update();

	CScene::UpdateAll();

	if (CInput::GetKeyboardTrigger(DIK_RETURN))
	{
		CFade::Start(new CResult);
	}
	int cnt = 0;
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
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
		//CFade::Start(new CResult);
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
