/*******************************************************************************
* タイトル名：
* ファイル名：result.cpp
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
#include "input.h"
#include "game.h"
#include "title.h"
#include "result.h"
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "texture.h"
#include "sound.h"
#include "time.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const int SWITCH_CNT = 180;

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CResult::CResult()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CResult::CResult()
{
	m_bMultithread = false;
	m_nStateCnt = 0;
}

/*******************************************************************************
* 関数名：CResult::~CResult()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CResult::~CResult()
{
}

/*******************************************************************************
* 関数名：void CResult::Init( void )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CResult::Init( void )
{
	CScene *scene = CBg::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), TEXTURE_TYPE_RESULT);
	CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), 800.0f, 60.0f, TEXTURE_TYPE_THANK_YOU_000);

	// サウンド再生
	CSound::Play(CSound::SOUND_LABEL_BGM_002);
}

/*******************************************************************************
* 関数名：void CResult::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CResult::Uninit( void )
{
	CScene::UninitAll();
	CSound::Stop();
	HewNetLib *hew = CManager::GetHew();
	// 好きな文字列        スコアがあるならスコア( 0から1000まで )
	hew->SendResult("End", (int)CTime::GetTime());
}

/*******************************************************************************
* 関数名：void CResult::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CResult::Update( void )
{
	CInput *input = CManager::GetInput();
	CScene::UpdateAll();
	m_nStateCnt++;
	if (input->GetKeyboardTrigger(DIK_RETURN) || m_nStateCnt > SWITCH_CNT || input->TriggerJoyStick(XINPUT_GAMEPAD_START))
	{
		CFade::Start(new CTitle);
	}
}

/*******************************************************************************
* 関数名：void CResult::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CResult::Draw( void )
{
	CScene::DrawAll();
}