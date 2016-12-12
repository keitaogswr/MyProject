/*******************************************************************************
* タイトル名：
* ファイル名：title.cpp
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
#include "input.h"
#include "manager.h"
#include "mode.h"
#include "fade.h"
#include "game.h"
#include "result.h"
#include "title.h"
#include "load.h"
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "logo.h"
#include "scene3D.h"
#include "sceneX.h"
#include "motionManager.h"
#include "dynamicModel.h"
#include "light.h"
#include "camera.h"
#include "titleCamera.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CTitle::CTitle()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CTitle::CTitle()
{
	m_bMultithread = false;
}

/*******************************************************************************
* 関数名：CTitle::~CTitle()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CTitle::~CTitle()
{
}

/*******************************************************************************
* 関数名：void CTitle::Init( void )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CTitle::Init(void)
{
	CMotionManager::Load();
	CScene *scene;
	scene = CBg::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), TEXTURE_TYPE_TITLE);
	scene = CLogo::Create(Vector3(SCREEN_WIDTH / 2, 300.0f, 0.0f), 0.0f, TEXTURE_TYPE_TITLE_LOGO, 600.0f, 90.0f, false);
	scene = CLogo::Create(Vector3(SCREEN_WIDTH / 2, 500.0f, 0.0f), 0.0f, TEXTURE_TYPE_PRESS_START, 500.0f, 90.0f, true);
	scene = CDynamicModel::Create(Vector3(0.0f, 0.0f, 0.0f));
	// カメラ生成
	m_Camera = CTitleCamera::Create();
	m_Light = CLight::Create();
}

/*******************************************************************************
* 関数名：void CTitle::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CTitle::Uninit(void)
{
	CScene::UninitAll();
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	// リソース解放
	CMotionManager::Unload();
}

/*******************************************************************************
* 関数名：void CTitle::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CTitle::Update(void)
{
	m_Camera->Update();
	CScene::UpdateAll();
	if (CInput::GetKeyboardTrigger(DIK_RETURN))
	{
		CFade::Start(new CLoad);
	}

}

/*******************************************************************************
* 関数名：void CTitle::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CTitle::Draw(void)
{
	m_Camera->Set();
	CScene::DrawAll();
}