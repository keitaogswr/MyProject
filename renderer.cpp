/*******************************************************************************
* タイトル名：
* ファイル名：renderer.cpp
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "renderer.h"
#include "main.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define STARTPOS	( 300 )
#define ENDPOS		( 3000 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CRenderer::CRenderer()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CRenderer::CRenderer()
{
	m_Wireframe = WIREFRAME_NONE;
	m_bFog = true;
}

/*******************************************************************************
* 関数名：CRenderer::~CRenderer()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CRenderer::~CRenderer()
{
}

/*******************************************************************************
* 関数名：void CRenderer::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
HRESULT CRenderer::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
{
	/*D3DXCreateFont( m_D3DDevice, 18, 0, 0, 0, FALSE,
					SHIFTJIS_CHARSET,
					OUT_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH,
					"Terminal",
					&m_Font );*/

	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;


	m_D3D = Direct3DCreate9( D3D_SDK_VERSION );					//Direct3Dオブジェクトの生成

	if( m_D3D == NULL )
	{
		return E_FAIL;
	}
	if( FAILED( m_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		return E_FAIL;
	};

	ZeroMemory( &d3dpp, sizeof( d3dpp ) );							//ゼロクリア
	
	d3dpp.BackBufferWidth = SCREEN_WIDTH;							//画面の幅
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;							//高さ
	d3dpp.BackBufferFormat = d3ddm.Format;							//バックバッファ形式
	d3dpp.BackBufferCount = 1;										//バックバッファの数

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;							//デプスバッファとステンシルバッファの生成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					//デプスバッファとして24ビットを使用
	d3dpp.Windowed = bWindow;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	if( FAILED ( m_D3D->CreateDevice(
					D3DADAPTER_DEFAULT,
					D3DDEVTYPE_HAL,
					hWnd,
					D3DCREATE_HARDWARE_VERTEXPROCESSING/* | D3DCREATE_MULTITHREADED*/,
					&d3dpp,
					&m_D3DDevice ) ) )
	{
		if( FAILED ( m_D3D->CreateDevice(
						D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						hWnd,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						&d3dpp,
						&m_D3DDevice ) ) )
		{
			if( FAILED ( m_D3D->CreateDevice(
							D3DADAPTER_DEFAULT,
							D3DDEVTYPE_REF,
							hWnd,
							D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							&d3dpp,
							&m_D3DDevice ) ) )
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	m_D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_D3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_D3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// サンプラーステートの設定
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );	// テクスチャ(U値),繰り返しに設定
	m_D3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );	// テクスチャ(U値),繰り返しに設定

	// テクスチャステージステートの設定
	m_D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );	// 下2つのテクスチャのアルファ値を混ぜる
	m_D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	// テクスチャのアルファ値
	m_D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );	// 現在のアルファ値

	// ZテストのON
	m_D3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// フォグの設定
	FLOAT StartPos   = STARTPOS;	//開始位置
	FLOAT EndPos     = ENDPOS;		//終了位置
	m_D3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	m_D3DDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0x77, 0x88, 0x99, 0xff));
	m_D3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);      //頂点モード
	m_D3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);     //テーブルモード
	m_D3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&StartPos));	//開始位置
	m_D3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&EndPos));		//終了位置

	return S_OK;
}

/*******************************************************************************
* 関数名：void CRenderer::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CRenderer::Uninit( void )
{
	SAFE_RELEASE( m_D3DDevice );
	SAFE_RELEASE( m_D3D );
}

/*******************************************************************************
* 関数名：void CRenderer::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CRenderer::Update( void )
{
#ifdef _DEBUG
	// ワイヤーフレーム切替
	if( CInput::GetKeyboardTrigger( DIK_F8 ) )
	{
		m_Wireframe == WIREFRAME_NONE? m_Wireframe = WIREFRAME_ON: m_Wireframe = WIREFRAME_NONE;
	}
	if (CInput::GetKeyboardTrigger(DIK_F9))
	{
		m_bFog = m_bFog == false ? true : false;
	}
#endif
}

/*******************************************************************************
* 関数名：void CRenderer::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CRenderer::Draw( void )
{
	
}

/*******************************************************************************
* 関数名：void CRenderer::Begin( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画開始処理
*******************************************************************************/
void CRenderer::Begin( void )
{
	// 画面クリア
	m_D3DDevice->Clear( 0, NULL, ( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL ), D3DCOLOR_RGBA( 50, 50, 50, 255 ), 1.0f, 0 );

	m_D3DDevice->BeginScene();							// 描画開始

	// ワイヤーフレーム描画
	m_Wireframe == WIREFRAME_ON ?
	m_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) :
	m_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	// フォグ描画
	m_bFog == true ?
	m_D3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE) :
	m_D3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

/*******************************************************************************
* 関数名：void CRenderer::End( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画終了処理
*******************************************************************************/
void CRenderer::End( void )
{
	m_D3DDevice->EndScene();							// 描画終了
	m_D3DDevice->Present( NULL, NULL, NULL, NULL );		// バックバッファとフロントバッファの切り替え
}

/*******************************************************************************
* 関数名：LPDIRECT3DDEVICE9 GetDevice( void )
* 
* 引数	：
* 戻り値：
* 説明	：
*******************************************************************************/
LPDIRECT3DDEVICE9 CRenderer::GetDevice( void )
{
	// デバイスを返す
	return m_D3DDevice;
}