/*******************************************************************************
* タイトル名：
* ファイル名：billboard.cpp
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
#include "mode.h"
#include "game.h"
#include "camera.h"
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "billboard.h"
#include "input.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define TEXTFILENAME	( "BillboardData.txt" )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CBillboard::CBillboard()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CBillboard::CBillboard( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene3D( DrawOrder, ObjType )
{
}

/*******************************************************************************
* 関数名：CBillboard::~CBillboard()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CBillboard::~CBillboard()
{
}

/*******************************************************************************
* 関数名：void CBillboard::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CBillboard::Init( Vector3 pos )
{
	m_Pos = pos;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
								 HEIGHT,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 HEIGHT,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( -WIDTH * 0.5f,
								 0.0f,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( WIDTH * 0.5f,
								 0.0f,
								 0.0f );

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
* 関数名：void CBillboard::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CBillboard::Uninit( void )
{
	// 解放
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
	// 削除フラグ
	m_Delete = true;
}

/*******************************************************************************
* 関数名：void CBillboard::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CBillboard::Update( void )
{
}

/*******************************************************************************
* 関数名：CBillboard *CBillboard::Create( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CBillboard *CBillboard::Create( Vector3 pos )
{
	CBillboard *billboard;
	billboard = new CBillboard;

	CGame *game = ( CGame* )CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();
	pos.y = meshField->GetHeight( pos );

	billboard->Init( pos );
	return billboard;
}

/*******************************************************************************
* 関数名：void CBillboard::SetRenderStateBegin( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定開始処理
*******************************************************************************/
void CBillboard::SetRenderStateBegin(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 各種設定 /////
	// ライトのOFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ZテストのON
	/*pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );*/
	// アルファテストのON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
}

/*******************************************************************************
* 関数名：void CBillboard::SetRenderStateEnd( void )
*
* 引数	：
* 戻り値：
* 説明	：レンダラーステート設定終了処理
*******************************************************************************/
void CBillboard::SetRenderStateEnd(void)
{
	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 設定を元に戻す /////
	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// ZテストのOFF
	//pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
}

/*******************************************************************************
* 関数名：void CBillboard::SetWorldMatrix( void )
*
* 引数	：
* 戻り値：
* 説明	：ワールドマトリックス設定処理
*******************************************************************************/
void CBillboard::SetWorldMatrix(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ビューマトリックスの逆行列を求める
	/*D3DXMatrixInverse( &m_MtxWorld,
	NULL,
	&mtxView);*/

	// マトリクスの移動に関する変数クリア
	m_MtxWorld._41 = 0.0f;
	m_MtxWorld._42 = 0.0f;
	m_MtxWorld._43 = 0.0f;

	D3DXMATRIX matBillboard;
	D3DXVECTOR3 vDir = camera->GetVecRV();

	if (vDir.x >= 0.0f)
	{
		D3DXMatrixRotationY(&matBillboard, -atanf(vDir.z / vDir.x) + D3DX_PI / 2.0f);
	}
	else
	{
		D3DXMatrixRotationY(&matBillboard, -atanf(vDir.z / vDir.x) - D3DX_PI / 2.0f);
	}
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&matBillboard);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);
	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);
}