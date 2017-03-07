/*******************************************************************************
* タイトル名：
* ファイル名：sceneX.cpp
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
#include "sceneX.h"
#include "input.h"
#include "scene3D.h"
#include "meshField.h"
#include "camera.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MOVE_SPEED		( 2.0f )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CSceneX::CSceneX()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CSceneX::CSceneX( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene( DrawOrder, ObjType )
{
	m_Scl = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Texture = NULL;
	m_fCollision = 0.0f;
}

/*******************************************************************************
* 関数名：CSceneX::~CSceneX()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CSceneX::~CSceneX()
{
}

/*******************************************************************************
* 関数名：void CSceneX::Init( Vector3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CSceneX::Init( Vector3 pos )
{
	m_Pos = pos;

	/* 変数定義 */
	int nNumVertices;	// 頂点数
	DWORD sizeFVF;		// 頂点フォーマットのサイズ
	BYTE *pVtxBuff;		// 頂点情報へのポインタ

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// Xファイルの読み込み
	D3DXLoadMeshFromX(	"data\\MODEL\\boss_TypeFortress\\All.x",		// 読み込みファイル名
						D3DXMESH_SYSTEMMEM,
						pDevice,
						NULL,
						&m_pBuffMat,		// マテリアル情報
						NULL,
						&m_numMat,			// マテリアルの数
						&m_pMesh );			// メッシュ情報

	nNumVertices = m_pMesh->GetNumVertices();
	sizeFVF = D3DXGetFVFVertexSize( m_pMesh->GetFVF() );

	// 頂点情報の取得
	m_pMesh->LockVertexBuffer( D3DLOCK_READONLY, ( void** )&pVtxBuff );
}

/*******************************************************************************
* 関数名：void CSceneX::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CSceneX::Uninit( void )
{
	// 解放
	SAFE_RELEASE( m_pMesh );
	SAFE_RELEASE( m_pBuffMat );
}

/*******************************************************************************
* 関数名：void CSceneX::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CSceneX::Update( void )
{

}

/*******************************************************************************
* 関数名：void CSceneX::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CSceneX::Draw( void )
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;		// スケール、向き、ポジション

	D3DXMATERIAL	*pMat;						// マテリアルポインタ
	D3DMATERIAL9	matDef;						// デフォルトのマテリアル

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	SetWorldMatrix();
	
	// ワールドマトリックスを設定
	pDevice->SetTransform( D3DTS_WORLD, &m_MtxWorld );

	pDevice->GetMaterial( &matDef );							// 現在のマテリアルを取得

	pMat = ( D3DXMATERIAL* )m_pBuffMat->GetBufferPointer();

	for( int nCntMat = 0; nCntMat < ( int )m_numMat; nCntMat++ )
	{
		// マテリアルの設定
		pDevice->SetMaterial( &pMat[ nCntMat ].MatD3D );

		// テクスチャの設定
		pDevice->SetTexture( 0, m_Texture );

		// メッシュの描画
		m_pMesh->DrawSubset( nCntMat );
	}
	// マテリアルを元に戻す
	pDevice->SetMaterial( &matDef );
}

/*******************************************************************************
* 関数名：void CSceneX::Uninit( void )
* 
* 引数	：
* 戻り値：CSceneX型のポインタ
* 説明	：生成処理
*******************************************************************************/
CSceneX *CSceneX::Create( Vector3 pos )
{
	CSceneX *sceneX;
	sceneX = new CSceneX;
	sceneX->Init( pos );
	return sceneX;
}

/*******************************************************************************
* 関数名：void CMeshCylinder::SetWorldMatrix( void )
*
* 引数	：
* 戻り値：
* 説明	：ワールドマトリックス設定処理
*******************************************************************************/
void CSceneX::SetWorldMatrix(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_MtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl,
		m_Scl.x,
		m_Scl.y,
		m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_Rot.y,
		m_Rot.x,
		m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_Pos.x,
		m_Pos.y,
		m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);
}