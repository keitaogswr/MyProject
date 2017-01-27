/*******************************************************************************
* タイトル名：
* ファイル名：scene3D.cpp
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
#include "scene3D.h"
#include "input.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CScene3D::CScene3D()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CScene3D::CScene3D( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene( DrawOrder, ObjType )
{
	m_Pos = Vector3( 0.0f, 0.0f, 0.0f );
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Trans = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_pTexture = NULL;
	m_VtxBuff = NULL;
}

/*******************************************************************************
* 関数名：CScene3D::~CScene3D()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CScene3D::~CScene3D()
{
}

/*******************************************************************************
* 関数名：void CScene3D::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CScene3D::Init( Vector3 pos )
{
	m_Pos = pos;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(
		pDevice,
		"data\\TEXTURE\\ore.jpg",
		m_pTexture);

	// 頂点バッファの生成
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	// 頂点バッファの設定
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( -WIDTH * 0.5f,
								 m_Pos.y,
								 DEPTH * 0.5f );
	pVtx[ 1 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 m_Pos.y,
								 DEPTH * 0.5f );
	pVtx[ 2 ].pos = D3DXVECTOR3( -WIDTH * 0.5f,
								 m_Pos.y,
								 -DEPTH * 0.5f );
	pVtx[ 3 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 m_Pos.y,
								 -DEPTH * 0.5f );

	pVtx[ 0 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 1 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 2 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 3 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

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
* 関数名：void CScene3D::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CScene3D::Uninit( void )
{
	// 解放
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* 関数名：void CScene3D::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CScene3D::Update( void )
{
}

/*******************************************************************************
* 関数名：void CScene3D::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CScene3D::Draw( void )
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// レンダラーステートの設定
	SetRenderStateBegin();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity( &m_MtxWorld );
	// スケールを反映
	D3DXMatrixScaling( &mtxScl, m_Scl.x, m_Scl.y, m_Scl.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxScl );
	// 回転を反映
	D3DXMatrixRotationYawPitchRoll( &mtxRot, m_Rot.y, m_Rot.x, m_Rot.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxRot );
	// 位置を反映
	D3DXMatrixTranslation( &mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxTrans );
	
	// ワールドマトリックスを設定
	pDevice->SetTransform( D3DTS_WORLD, &m_MtxWorld );

	// ストリームにバインド
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_3D ) );

	//頂点フォーマットの設定
	pDevice->SetFVF( FVF_VERTEX_3D );

	//テクスチャの設定
	pDevice->SetTexture( 0, *m_pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//プリミティブの種類
					0,							//最初の頂点のインデックス
					2 );						//描画するプリミティブ数

	// レンダラーステートの設定
	SetRenderStateEnd();
}

/*******************************************************************************
* 関数名：CScene3D *CScene3D::Create( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CScene3D *CScene3D::Create( Vector3 pos )
{
	CScene3D *scene3D;
	scene3D = new CScene3D;
	scene3D->Init( pos );
	return scene3D;
}