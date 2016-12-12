/*******************************************************************************
* タイトル名：
* ファイル名：bg.cpp
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
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( SCREEN_WIDTH )
#define HEIGHT			( SCREEN_HEIGHT )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CBg::CBg()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CBg::CBg( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene2D( DrawOrder, ObjType )
{
	pFileName = NULL;
}

/*******************************************************************************
* 関数名：CBg::~CBg()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CBg::~CBg()
{
}

/*******************************************************************************
* 関数名：void CBg::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CBg::Init( Vector3 pos, int nIdx)
{
	m_Pos = pos;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(nIdx);

	// 頂点バッファの生成
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_2D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_2D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	m_fLength = sqrtf( WIDTH * WIDTH + HEIGHT * HEIGHT ) * 0.5f;
	m_fAngle = atan2f( WIDTH, HEIGHT );

	// 頂点バッファの設定
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( m_Pos.x + sinf( D3DX_PI + m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( D3DX_PI + m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( m_Pos.x + sinf( D3DX_PI - m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( D3DX_PI - m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( m_Pos.x + sinf( -m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( -m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( m_Pos.x + sinf( m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( m_fAngle + m_Rot.z ) * m_fLength,
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
* 関数名：void CBg::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CBg::Uninit( void )
{
	// 解放
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* 関数名：void CBg::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBg::Update( void )
{

}

/*******************************************************************************
* 関数名：void CBg::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CBg::Draw( void )
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// ZテストのON
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// ストリームにバインド
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_2D ) );

	// 頂点フォーマットの設定
	pDevice->SetFVF( FVF_VERTEX_2D );

	// テクスチャの設定
	pDevice->SetTexture( 0, *m_pTexture);

	// ポリゴンの描画
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//プリミティブの種類
					0,							//最初の頂点のインデックス
					2 );						//描画するプリミティブ数

	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

/*******************************************************************************
* 関数名：CBg *CBg::Create( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CBg *CBg::Create( Vector3 pos, int nIdx )
{
	CBg *bg;
	bg = new CBg;
	bg->Init( pos, nIdx );
	return bg;
}