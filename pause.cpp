/*******************************************************************************
* タイトル名：
* ファイル名：pause.cpp
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
#include "load.h"
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "texture.h"
#include "pause.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;
const D3DXCOLOR DEFAULT_COLOR = D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.6f);

/*******************************************************************************
* グローバル変数
*******************************************************************************/
LPDIRECT3DVERTEXBUFFER9 CPause::m_VtxBuff;	// 頂点バッファ
float CPause::m_fWidth;						// ポリゴン幅
float CPause::m_fHeight;					// ポリゴン高さ
D3DXCOLOR CPause::m_Col;					// 色
CScene2D *CPause::m_pLogo[PAUSE_LOGO_MAX];
int CPause::m_nCurrent;

/*******************************************************************************
* 関数名：CPause::CPause()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CPause::CPause()
{
}

/*******************************************************************************
* 関数名：CPause::~CPause()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CPause::~CPause()
{
}

/*******************************************************************************
* 関数名：void CPause::Init( void )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CPause::Init(void)
{
	// 値の格納
	m_fWidth = WIDTH;
	m_fHeight = HEIGHT;
	m_Col = DEFAULT_COLOR;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f - m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f - m_fHeight * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f + m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f - m_fHeight * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f - m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f + m_fHeight * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f + m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f + m_fHeight * 0.5f,
		0.0f);

	pVtx[0].rhw =
	pVtx[1].rhw =
	pVtx[2].rhw =
	pVtx[3].rhw = 1.0f;

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = m_Col;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	m_VtxBuff->Unlock();

	m_pLogo[0] = CScene2D::Create(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f - 200.0f, 0.0f), 400.0f, 200.0f, TEXTURE_TYPE_PAUSE_000);
	m_pLogo[0]->SetPauseFlg(true);
	m_pLogo[0]->SetDrawFlg(false);
	m_pLogo[1] = CScene2D::Create(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), 400.0f, 200.0f, TEXTURE_TYPE_RESTART_000);
	m_pLogo[1]->SetPauseFlg(true);
	m_pLogo[1]->SetDrawFlg(false);
	m_pLogo[2] = CScene2D::Create(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f + 200.0f, 0.0f), 400.0f, 200.0f, TEXTURE_TYPE_EXIT_000);
	m_pLogo[2]->SetPauseFlg(true);
	m_pLogo[2]->SetDrawFlg(false);
}

/*******************************************************************************
* 関数名：void CPause::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CPause::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
}

/*******************************************************************************
* 関数名：void CPause::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CPause::Update(void)
{
	// ポーズ画面操作
	if (CInput::GetKeyboardTrigger(DIK_W))
	{
		m_nCurrent--;
	}
	if (CInput::GetKeyboardTrigger(DIK_S))
	{
		m_nCurrent++;
	}
	if (m_nCurrent < 0)
	{
		m_nCurrent = PAUSE_LOGO_MAX - 1;
	}
	if (m_nCurrent >= PAUSE_LOGO_MAX)
	{
		m_nCurrent = 0;
	}
	// 選択ごとの動作
	CGame *game = (CGame*)CManager::GetMode();
	if(CInput::GetKeyboardTrigger(DIK_RETURN))
	{
		switch (m_nCurrent)
		{
		case 0:
			game->SetPause(false);
			break;
		case 1:
			CFade::Start(new CLoad);
			break;
		case 2:
			CFade::Start(new CTitle);
			break;
		default:
			break;
		}
	}

	// ロゴの更新
	for (int i = 0; i < PAUSE_LOGO_MAX; i++)
	{
		m_pLogo[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
	}
	m_pLogo[m_nCurrent]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	for (int i = 0; i < PAUSE_LOGO_MAX; i++)
	{
		m_pLogo[i]->Update();
	}
}

/*******************************************************************************
* 関数名：void CPause::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CPause::Draw(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ストリームにバインド
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//プリミティブの種類
		0,							//最初の頂点のインデックス
		2);							//描画するプリミティブ数
	for (int i = 0; i < PAUSE_LOGO_MAX; i++)
	{
		m_pLogo[i]->Draw();
	}
}