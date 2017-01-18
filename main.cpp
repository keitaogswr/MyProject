/*******************************************************************************
* タイトル名：
* ファイル名：main.cpp
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
#include "scene.h"
#include "input.h"
#include "manager.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
HRESULT Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow );
void Uninit( void );
void Update( void );
void Draw( void );
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

/*******************************************************************************
* グローバル変数
*******************************************************************************/
int g_nCountFPS;							// FPSポインタ

/*******************************************************************************
* 関数名：WINAPI WinMain( HINSTANCE hInstance,
*					HINSTANCE hPrevInstance,
*					LPSTR lpCmdLine,
*					int nCmdShow )
* 
* 引数	：
* 戻り値：
* 説明	：メイン関数
*******************************************************************************/
int WINAPI WinMain( HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow )
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//変数定義
	DWORD dwFrameCount;
	DWORD dwCurrentTime;
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;

	//ウィンドウクラスの登録
	WNDCLASSEX wcex = 
	{
		sizeof( WNDCLASSEX ),					//メモリサイズを設定
		CS_CLASSDC,								//ウィンドウのスタイルを設定
		WndProc,								//ウィンドウプロシージャのアドレス（関数名）を指定
		0,
		0,
		hInstance,								//インスタントハンドルを指定
		NULL,									//使用するアイコンを指定
		LoadCursor( NULL, IDC_ARROW ),			//マウスカーソルを指定
		( HBRUSH )( COLOR_WINDOW + 1 ),			//クライアント領域の背景色を設定
		NULL,									//メニューを設定
		WindowClassName,						//ウインドウクラスの名前
		NULL									//小さいアイコンが設定された場合の情報
	};

	HWND hWnd;									//ウィンドウハンドル
	MSG msg;									//メッセージ
	RegisterClassEx( &wcex );

	
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, false );

	//ウィンドウクラスの設定
	hWnd = CreateWindowEx( 0,
							WindowClassName,			//ウィンドウクラスの名前
							"SWIPE OUT Mk.II",			//ウィンドウの名前
							WS_OVERLAPPEDWINDOW,		
							CW_USEDEFAULT,				//ウィンドウの左上X座標
							CW_USEDEFAULT,				//ウィンドウの左上Y座標
							rect.right - rect.left,		//幅
							rect.bottom - rect.top,		//高さ
							NULL,						//親ウィンドウのハンドル
							NULL,						//メニューハンドルまたは子ウィンドウID
							hInstance,
							NULL );						//ウィンドウ作成データ
	

	/* ウィンドウの表示 */
	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	timeBeginPeriod( 1 );
	dwFrameCount =
	dwCurrentTime = 0;
	dwExecLastTime =
	dwFPSLastTime = timeGetTime();

	// 初期化
	Init( hInstance, hWnd, TRUE );

	//メッセージループ
	while( 1 )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) != 0 )
		{
			if( msg.message == WM_QUIT )
			{
				break;
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if( ( dwCurrentTime - dwFPSLastTime ) >= 500 )
			{
				g_nCountFPS = ( dwFrameCount * 1000 ) / ( dwCurrentTime - dwFPSLastTime );
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			if( ( dwCurrentTime - dwExecLastTime ) >= ( 1000 / 60 ) )
			{
				dwExecLastTime = dwCurrentTime;
				Update();		//更新処理
				Draw();			//描画処理
				dwFrameCount++;
			}
		}
	}
	UnregisterClass( WindowClassName,		//ウインドウクラスの名前
					wcex.hInstance
					);
	Uninit();			// 終了処理
	timeEndPeriod( 1 );
	return ( int )msg.wParam;
}

/*******************************************************************************
* 関数名：HRESULT Init( HINSTANCE hInstance, HWND, BOOL bWindow )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
HRESULT Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
{
	CManager::Init(hInstance, hWnd, TRUE);
	return S_OK;
}

/*******************************************************************************
* 関数名：void Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void Uninit( void )
{
	CManager::Uninit();
}

/*******************************************************************************
* 関数名：void Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void Update( void )
{
	CManager::Update();
	CDebugProc::Print("FPS:%d\n\n", g_nCountFPS);
}

/*******************************************************************************
* 関数名：void Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void Draw( void )
{
	CManager::Draw();			//描画処理
}

/*******************************************************************************
* 関数名：LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
* 
* 引数	：
* 戻り値：
* 説明	：ウインドウプロシージャ
*******************************************************************************/
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	/* 変数定義 */
	int nID;

	switch( uMsg )
	{
		case WM_DESTROY :						//ウィンドウが破棄された
			PostQuitMessage( 0 );
			break;
		case WM_KEYDOWN :						//キーが押された
			switch( wParam )
			{
				case VK_ESCAPE :				//escキーが押された
					DestroyWindow( hWnd );
			}
			break;
		case WM_CLOSE :							//ウインドウを閉じる
			nID = MessageBox( hWnd, "終了しますか？", "警告", MB_YESNO );
			if( nID == IDYES )
			{
				DestroyWindow( hWnd );
			}
			else if( nID == IDNO )
			{
				return 0;
			}
			break;
		default :
			break;
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}