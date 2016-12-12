/*******************************************************************************
* タイトル名：
* ファイル名：skyDome.cpp
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
#include "game.h"
#include "renderer.h"
#include "scene.h"
#include "scene3D.h"
#include "skyDome.h"
#include "input.h"
#include "camera.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MESHDOME_ANGLE	( 50.0f )			// ドーム部分の角度
#define ROT_SPEED		( 0.01f )			// 

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CSkyDome::CSkyDome()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CSkyDome::CSkyDome( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene3D( DrawOrder, ObjType )
{
	m_Pos = Vector3( 0.0f, 0.0f, 0.0f );
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Trans = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_pTexture = NULL;
	m_VtxBuff = NULL;

	ZeroMemory( &m_MeshFan, sizeof( m_MeshFan ) );
	ZeroMemory( &m_MeshDome, sizeof( m_MeshDome ) );
}

/*******************************************************************************
* 関数名：CSkyDome::~CSkyDome()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CSkyDome::~CSkyDome()
{
}

/*******************************************************************************
* 関数名：void CSkyDome::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CSkyDome::Init( Vector3 pos, float rad, int row, int column )
{
	m_Pos = pos;

	// ドーム部分
	m_MeshDome.nRowBlock = row;
	m_MeshDome.nColumnBlock = column;
	m_MeshDome.nRowNumVtx = m_MeshDome.nRowBlock + 1;
	m_MeshDome.nColumnNumVtx =m_MeshDome.nColumnBlock + 1;
	m_MeshDome.fRadius = rad;
	m_MeshDome.nNumVtxMax = m_MeshDome.nRowNumVtx * m_MeshDome.nColumnNumVtx;
	m_MeshDome.nNumIdx = m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 + ( m_MeshDome.nRowBlock - 1 ) * 2;
	m_MeshDome.nNumPrim = ( m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 ) + ( m_MeshDome.nRowBlock - 1 ) * 4;

	// ファン部分
	m_MeshFan.nNumVtxMax = m_MeshDome.nColumnNumVtx;
	m_MeshFan.nNumIdx = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan.nNumPrim = m_MeshDome.nColumnBlock;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_SKY_000);

	// 頂点バッファの確保 /////
	// ドーム部分
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * m_MeshDome.nNumVtxMax,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_MeshDome.pVtxBuff,
							NULL ) ) )
	{
		return;
	}
	// ファン部分
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * m_MeshFan.nNumVtxMax,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_MeshFan.pVtxBuff,
							NULL ) ) )
	{
		return;
	}

	// インデックスバッファの確保 /////
	// ドーム部分
	pDevice->CreateIndexBuffer( sizeof( WORD ) * m_MeshDome.nNumIdx,		// 確保するバッファサイズ
								D3DUSAGE_WRITEONLY,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&m_MeshDome.pIdxBuff,
								NULL );
	// ファン部分
	pDevice->CreateIndexBuffer( sizeof( WORD ) * m_MeshFan.nNumIdx,		// 確保するバッファサイズ
								D3DUSAGE_WRITEONLY,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&m_MeshFan.pIdxBuff,
								NULL );

	// 頂点バッファの設定
	VERTEX_3D *pVtx;
	// ドーム部分
	m_MeshDome.pVtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	for (int nCntRow = m_MeshDome.nRowNumVtx - 1, nIdxCnt = 0; nCntRow >= 0; nCntRow--)
	{
		for( int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			pVtx[ nIdxCnt ].pos.x = cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
									sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ) * m_MeshDome.fRadius;
			pVtx[ nIdxCnt ].pos.y = sinf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) * m_MeshDome.fRadius;
			pVtx[ nIdxCnt ].pos.z = cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
									cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ) * m_MeshDome.fRadius;

			pVtx[ nIdxCnt ].nor = D3DXVECTOR3( cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
												sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ), 
												sinf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ),
												cosf( MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow ) *
												cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn ) );

			pVtx[ nIdxCnt ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

			// テクスチャブロック表示
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// テクスチャ全面表示
			pVtx[ nIdxCnt ].tex.x = ( 1.0f / m_MeshDome.nColumnBlock ) * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = ( 1.0f / m_MeshDome.nRowBlock ) * (m_MeshDome.nRowNumVtx - 1 - nCntRow);
		}
	}

	m_MeshDome.pVtxBuff->Unlock();

	// ファン部分
	m_MeshFan.pVtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos.x = 0.0f;
	pVtx[ 0 ].pos.y = m_MeshDome.fRadius;
	pVtx[ 0 ].pos.z = 0.0f;

	pVtx[ 0 ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	pVtx[ 0 ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

	pVtx[ 0 ].tex.x = 0.0f;
	pVtx[ 0 ].tex.y = 0.0f;

	for( int i = 1, nIdxCnt = 0; i < m_MeshFan.nNumVtxMax; i++ )
	{
		pVtx[ i ].pos.x = cosf( MESHDOME_ANGLE * RAD ) *
								sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ) * m_MeshDome.fRadius;

		pVtx[ i ].pos.y = sinf( MESHDOME_ANGLE * RAD ) * m_MeshDome.fRadius;
		pVtx[ i ].pos.z = cosf( MESHDOME_ANGLE * RAD ) *
								cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ) * m_MeshDome.fRadius;

		pVtx[ i ].nor = D3DXVECTOR3( cosf( MESHDOME_ANGLE * RAD ) *
										sinf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ), 
										sinf( MESHDOME_ANGLE * RAD ),
										cosf( MESHDOME_ANGLE * RAD ) *
										cosf( -D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * ( i - 1 ) ) );
		pVtx[ i ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

		pVtx[ i ].tex.x = 0.0f;
		pVtx[ i ].tex.y = 1.0f;
	}

	m_MeshFan.pVtxBuff->Unlock();


	// インデックス情報設定 /////
	WORD *pIdx;

	m_MeshDome.pIdxBuff->Lock( 0, 0, ( void** )&pIdx, 0 );

	// 変数定義
	int x, y, cnt;

	for( y = 0, cnt = 0; y < m_MeshDome.nRowBlock; y++ )
	{
		for( x = 0; x < m_MeshDome.nColumnNumVtx; x++, cnt++, pIdx += 2 )
		{
			pIdx[ 0 ] = x + ( y + 1 ) * m_MeshDome.nColumnNumVtx;
			pIdx[ 1 ] = x + ( y + 0 ) * m_MeshDome.nColumnNumVtx;
			if( x == m_MeshDome.nColumnNumVtx - 1 && cnt * 2 < m_MeshDome.nNumIdx - 2 )
			{
				cnt++;
				pIdx += 2;
				pIdx[ 0 ] = x + ( y + 0 ) * m_MeshDome.nColumnNumVtx;
				pIdx[ 1 ] = ( y + 1 + 1 ) * m_MeshDome.nColumnNumVtx;
			}
		}
	}

	m_MeshDome.pIdxBuff->Unlock();

	m_MeshFan.pIdxBuff->Lock( 0, 0, ( void** )&pIdx, 0 );

	for( int i = 0; i < m_MeshFan.nNumIdx; i++ )
	{
		pIdx[ i ] = i;
	}
	pIdx[ m_MeshFan.nNumIdx - 1 ] = 1;

	m_MeshFan.pIdxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CSkyDome::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CSkyDome::Uninit( void )
{
	// 解放
	SAFE_RELEASE( m_MeshDome.pVtxBuff );
	SAFE_RELEASE( m_MeshDome.pIdxBuff );
	SAFE_RELEASE( m_MeshFan.pVtxBuff );
	SAFE_RELEASE( m_MeshFan.pIdxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* 関数名：void CSkyDome::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CSkyDome::Update( void )
{
	// カメラを追従
	CGame *game = ( CGame* )CManager::GetMode();
	CCamera *camera = game->GetCamera();
	m_Pos.x = camera->GetPosition().x;
	m_Pos.z = camera->GetPosition().z;
	m_Rot.y += ROT_SPEED;
}

/*******************************************************************************
* 関数名：void CSkyDome::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CSkyDome::Draw( void )
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// 各種設定 /////
	// ライトのOFF
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// アルファテストのON
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
	pDevice->SetRenderState( D3DRS_ALPHAREF, 50 );
	
	// フォグのOFF
	pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// カリングの設定
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

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

	// データストリームにバインドする
	pDevice->SetStreamSource( 0, m_MeshDome.pVtxBuff, 0, sizeof( VERTEX_3D ) );
	// インデックスバッファをバインド
	pDevice->SetIndices( m_MeshDome.pIdxBuff );

	// 頂点フォーマットの設定
	pDevice->SetFVF( FVF_VERTEX_3D );

	// テクスチャの設定
	pDevice->SetTexture( 0, *m_pTexture );

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLESTRIP,	// プリミティブの種類
					0,
					0,							
					m_MeshDome.nNumVtxMax,	// 頂点数
					0,
					m_MeshDome.nNumPrim );	// 描画するプリミティブ数

	// データストリームにバインドする
	pDevice->SetStreamSource( 0, m_MeshFan.pVtxBuff, 0, sizeof( VERTEX_3D ) );
	// インデックスバッファをバインド
	pDevice->SetIndices( m_MeshFan.pIdxBuff );

	// 頂点フォーマットの設定
	pDevice->SetFVF( FVF_VERTEX_3D );

	// テクスチャの設定
	pDevice->SetTexture( 0, *m_pTexture );

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLEFAN,		// プリミティブの種類
					0,
					0,							
					m_MeshFan.nNumVtxMax,	// 頂点数
					0,
					m_MeshFan.nNumPrim );	// 描画するプリミティブ数

	// ライトのON
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	// アルファテストのOFF
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	// フォグのON
	pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	// カリングをもとに戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/*******************************************************************************
* 関数名：CSkyDome *CSkyDome::Create( Vector3 pos, float rad, int row, int column )
* 
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CSkyDome *CSkyDome::Create( Vector3 pos, float rad, int row, int column )
{
	CSkyDome *scene3D;
	scene3D = new CSkyDome;
	scene3D->Init( pos, rad, row, column );
	return scene3D;
}