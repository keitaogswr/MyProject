/*******************************************************************************
* タイトル名：
* ファイル名：manager.cpp
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
#include "renderer.h"
#include "sound.h"
#include "input.h"
#include "fade.h"
#include "mode.h"
#include "title.h"
#include "game.h"
#include "result.h"
#include "editor.h"
#include "load.h"
#include "texture.h"
#include "manager.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* グローバル変数
*******************************************************************************/
CRenderer *CManager::m_Renderer;
CManager::OPMODE CManager::m_Opmode;
HWND *CManager::m_hWnd;
CMode *CManager::m_Mode;
CInput *CManager::m_Input;

/*******************************************************************************
* 関数名：CManager::CManager()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CManager::CManager()
{
	m_Renderer = NULL;
	m_Opmode = OPMODE_PLAYER;
}

/*******************************************************************************
* 関数名：CManager::~CManager()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CManager::~CManager()
{
}

/*******************************************************************************
* 関数名：void CManager::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CManager::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
{
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	m_hWnd = &hWnd;

	m_Renderer = new CRenderer;
	m_Renderer->Init( hInstance, hWnd, bWindow );

	m_Input = new CInput;
	m_Input->InitKeyboard( hInstance, hWnd );
	m_Input->InitJoyStick();

	CSound::Init( hWnd );

	CTexture::Load();

	CFade::Init();
#ifdef _DEBUG
	CDebugProc::Init();
#endif

	SetMode(new CTitle);
}

/*******************************************************************************
* 関数名：void CManager::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CManager::Uninit( void )
{
	m_Input->UninitKeyboard();
	SAFE_DELETE(m_Input);
	CFade::Uninit();

	m_Mode->Uninit();
	CTexture::Unload();
	m_Renderer->Uninit();
#ifdef _DEBUG
	CDebugProc::Uninit();
#endif
	// 解放処理
	SAFE_DELETE(m_Renderer);
	SAFE_DELETE(m_Mode);
	// サウンド解放
	CSound::Uninit();
}

/*******************************************************************************
* 関数名：void CManager::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CManager::Update( void )
{
	m_Input->UpdateKeyboard();
	m_Input->UpdateMouse();
	m_Input->UpdateJoyStick();

	m_Renderer->Update();
	m_Mode->Update();
	
	CFade::Update();
#ifdef _DEBUG
	CDebugProc::Update();
#endif
}

/*******************************************************************************
* 関数名：void CManager::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CManager::Draw( void )
{
	// 描画開始
	m_Renderer->Begin();
	m_Mode->Draw();
	CFade::Draw();
#ifdef _DEBUG
	CDebugProc::Draw();
#endif
	m_Renderer->End();
	// 描画終了
}

/*******************************************************************************
* 関数名：CRenderer CManager::*GetRenderer( void )
* 
* 引数	：
* 戻り値：
* 説明	：レンダラー取得処理
*******************************************************************************/
CRenderer *CManager::GetRenderer( void )
{
	return m_Renderer;
}

/*******************************************************************************
* 関数名：HWND *CManager::GetHWND( void )
* 
* 引数	：
* 戻り値：
* 説明	：ウインドウハンドル取得処理
*******************************************************************************/
HWND *CManager::GetHWND( void )
{
	return m_hWnd;
}

/*******************************************************************************
* 関数名：D3DXVECTOR3* CManager::CalcScreenToWorld( D3DXVECTOR3* pout,int Sx, int Sy, float fZ, int Screen_w, int Screen_h, D3DXMATRIX* View, D3DXMATRIX* Prj )
* 
* 引数	：
* 戻り値：
* 説明	：スクリーン→ワールド変換処理
*******************************************************************************/
D3DXVECTOR3* CManager::CalcScreenToWorld(
   D3DXVECTOR3* pout,
   int Sx,				// スクリーンX座標
   int Sy,				// スクリーンY座標
   float fZ,			// 射影空間でのZ値（0～1）
   int Screen_w,		// スクリーン幅
   int Screen_h,		// スクリーン高さ
   D3DXMATRIX* View,	// ビューマトリクス
   D3DXMATRIX* Prj )	// プロジェクションマトリクス
{
	// 各行列の逆行列を算出
	D3DXMATRIX InvView, InvPrj, VP, InvViewport;
	D3DXMatrixInverse( &InvView, NULL, View );
	D3DXMatrixInverse( &InvPrj, NULL, Prj );
	D3DXMatrixIdentity( &VP );
	VP._11 = Screen_w/2.0f; VP._22 = -Screen_h/2.0f;
	VP._41 = Screen_w/2.0f; VP._42 = Screen_h/2.0f;
	D3DXMatrixInverse( &InvViewport, NULL, &VP );
	
	// 逆変換
	D3DXMATRIX tmp = InvViewport * InvPrj * InvView;
	D3DXVec3TransformCoord( pout, &D3DXVECTOR3( ( float )Sx, ( float )Sy, ( float )fZ ), &tmp );
	
	return pout;
}


/*******************************************************************************
* 関数名：D3DXVECTOR3* CManager::CalcScreenToXZ( D3DXVECTOR3* pout, int Sx, int Sy, int Screen_w, int Screen_h, D3DXMATRIX* View, D3DXMATRIX* Prj )
* 
* 引数	：
* 戻り値：
* 説明	：XZ平面とスクリーン座標の交点算出関数
*******************************************************************************/
D3DXVECTOR3* CManager::CalcScreenToXZ(
   D3DXVECTOR3* pout,
   int Sx,				// スクリーンX座標
   int Sy,				// スクリーンY座標
   int Screen_w,		// スクリーン幅
   int Screen_h,		// スクリーン高さ
   D3DXMATRIX* View,	// ビューマトリクス
   D3DXMATRIX* Prj)		// プロジェクションマトリクス
{
   D3DXVECTOR3 nearpos;
   D3DXVECTOR3 farpos;
   D3DXVECTOR3 ray;
   CalcScreenToWorld( &nearpos, Sx, Sy, 0.0f, Screen_w, Screen_h, View, Prj );
   CalcScreenToWorld( &farpos, Sx, Sy, 1.0f, Screen_w, Screen_h, View, Prj );
   ray = farpos - nearpos;
   D3DXVec3Normalize( &ray, &ray );

   // 床との交差が起きている場合は交点を
   // 起きていない場合は遠くの壁との交点を出力
   if( ray.y <= 0 ) {
      // 床交点
      float Lray = D3DXVec3Dot( &ray, &D3DXVECTOR3( 0, 1, 0 ) );
      float LP0 = D3DXVec3Dot( &( -nearpos ), &D3DXVECTOR3( 0, 1, 0 ) );
      *pout = nearpos + ( LP0 / Lray ) * ray;
   }
   else {
      *pout = farpos;
   }

   return pout;
}

/*******************************************************************************
* 関数名：void CManager::CheckRot( float f )
* 
* 引数	：
* 戻り値：
* 説明	：回転角修正処理
*******************************************************************************/
void CManager::CheckRot( float *f )
{
	if( *f > D3DX_PI )
	{
		*f -= D3DX_PI * 2;
	}
	if( *f < -D3DX_PI )
	{
		*f += D3DX_PI * 2;
	}
}

/*******************************************************************************
* 関数名：void CManager::CheckRot( Vector3 *rot )
* 
* 引数	：
* 戻り値：
* 説明	：回転角(XYZ)修正処理
*******************************************************************************/
void CManager::CheckRot( Vector3 *rot )
{
	// x
	if( rot->x > D3DX_PI )
	{
		rot->x -= D3DX_PI * 2;
	}
	if( rot->x < -D3DX_PI )
	{
		rot->x += D3DX_PI * 2;
	}
	// y
	if( rot->y > D3DX_PI )
	{
		rot->y -= D3DX_PI * 2;
	}
	if( rot->y < -D3DX_PI )
	{
		rot->y += D3DX_PI * 2;
	}
	// z
	if( rot->z > D3DX_PI )
	{
		rot->z -= D3DX_PI * 2;
	}
	if( rot->z < -D3DX_PI )
	{
		rot->z += D3DX_PI * 2;
	}
}

/*******************************************************************************
* 関数名：void CManager::SetMode( CMode *mode )
* 
* 引数	：
* 戻り値：
* 説明	：モード設定処理
*******************************************************************************/
void CManager::SetMode( CMode *mode )
{
	if( m_Mode != NULL )
	{
		m_Mode->Uninit();
		delete m_Mode;
		m_Mode = NULL;
	}
	m_Mode = mode;
	if (m_Mode->GetMultithreadFlg() == false)
	{
		m_Mode->Init();
	}
}

/*******************************************************************************
* 関数名：void CManager::SetMode( CMode *mode )
* 
* 引数	：
* 戻り値：
* 説明	：モード設定処理
*******************************************************************************/
CMode *CManager::GetMode( void )
{
	return m_Mode;
}