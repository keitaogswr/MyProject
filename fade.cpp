/*******************************************************************************
* タイトル名：
* ファイル名：fade.cpp
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
#include "fade.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( SCREEN_WIDTH )
#define HEIGHT			( SCREEN_HEIGHT )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define FADECOUNT		( 30 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/
CMode *CFade::m_NextMode;
FADE CFade::m_State = FADE_IN;
int CFade::m_Count;
LPDIRECT3DVERTEXBUFFER9 CFade::m_VtxBuff;
float CFade::m_Alfa;

/*******************************************************************************
* 関数名：CFade::CFade()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CFade::CFade()
{
}

/*******************************************************************************
* 関数名：CFade::~CFade()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CFade::~CFade()
{
}

/*******************************************************************************
* 関数名：void CFade::Init( void )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CFade::Init( void )
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	//D3DXCreateTextureFromFile(
	//	pDevice,
	//	"data\\TEXTURE\\reticle001.png",
	//	&m_Texture );

	// 頂点バッファの生成
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_2D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_2D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		
	}

	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( 0.0f,
								 0.0f,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( SCREEN_WIDTH,
								 0.0f,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( 0.0f,
								 SCREEN_HEIGHT,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( SCREEN_WIDTH,
								 SCREEN_HEIGHT,
								 0.0f );

	pVtx[ 0 ].rhw =
	pVtx[ 1 ].rhw =
	pVtx[ 2 ].rhw =
	pVtx[ 3 ].rhw = 1.0f;

	pVtx[ 0 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );
	pVtx[ 1 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );
	pVtx[ 2 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );
	pVtx[ 3 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

	pVtx[ 0 ].tex = D3DXVECTOR2( 0.0f, 0.0f );
	pVtx[ 1 ].tex = D3DXVECTOR2( 1.0f / TEXTURE_ROW, 0.0f );
	pVtx[ 2 ].tex = D3DXVECTOR2( 0.0f, 1.0f / TEXTURE_COLUMN );
	pVtx[ 3 ].tex = D3DXVECTOR2( 1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN );

	m_VtxBuff->Unlock();

}

/*******************************************************************************
* 関数名：void CFade::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CFade::Uninit( void )
{
	// 解放
	SAFE_RELEASE( m_VtxBuff );
}

/*******************************************************************************
* 関数名：void CFade::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CFade::Update( void )
{

	if( m_State == FADE_IN )
	{
		m_Alfa -= 1.0f / FADECOUNT;
		if( m_Count > FADECOUNT )
		{
			m_State = FADE_NONE;
			m_Count = 0;
			m_Alfa = 0.0f;
		}
	}
	else if( m_State == FADE_OUT )
	{
		m_Alfa += 1.0f / FADECOUNT;
		if( m_Count > FADECOUNT )
		{
			CManager::SetMode( m_NextMode );
			m_State = FADE_IN;
			m_Count = 0;
			m_Alfa = 1.0f;
		}
	}

	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].col = 
	pVtx[ 1 ].col = 
	pVtx[ 2 ].col = 
	pVtx[ 3 ].col = D3DXCOLOR( 0.0f, 0.0f, 0.0f, m_Alfa );

	m_VtxBuff->Unlock();

	m_Count++;
}

/*******************************************************************************
* 関数名：void CFade::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CFade::Draw( void )
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ストリームにバインド
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_2D ) );

	// 頂点フォーマットの設定
	pDevice->SetFVF( FVF_VERTEX_2D );

	// テクスチャの設定
	pDevice->SetTexture( 0, NULL );

	// ポリゴンの描画
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//プリミティブの種類
					0,							//最初の頂点のインデックス
					2 );						//描画するプリミティブ数
}

/*******************************************************************************
* 関数名：void CFade::Start( CMode *NextMode )
* 
* 引数	：
* 戻り値：
* 説明	：フェード開始処理
*******************************************************************************/
void CFade::Start( CMode *NextMode )
{
	if( m_State == FADE_NONE )
	{
		m_NextMode = NextMode;
		m_State = FADE_OUT;
		m_Count = 0;
		m_Alfa = 0.0f;
	}
}