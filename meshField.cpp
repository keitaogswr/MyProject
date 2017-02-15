/*******************************************************************************
* タイトル名：
* ファイル名：meshField.cpp
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
#include "scene.h"
#include "scene3D.h"
#include "billboard.h"
#include "treeObject.h"
#include "meshField.h"
#include "camera.h"
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
#define OPARATE_SPEED	( 3.0f )
#define TEXTFILENAME	( "data\\EDITDATA\\mapdata.txt" )

/*******************************************************************************
* グローバル変数
*******************************************************************************/

/*******************************************************************************
* 関数名：CMeshField::CMeshField()
* 
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CMeshField::CMeshField( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene3D( DrawOrder, ObjType )
{
	m_IdxBuff = NULL;

	m_fWidth = 0.0f;
	m_fDepth = 0.0f;
	m_nRowBlock = 0;
	m_nColumnBlock = 0;	
	m_nRowNumVtx = 0;
	m_nColumnNumVtx = 0;
	m_nNumVtxMax = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;

	m_fWidthAll = 0.0f;
	m_fDepthAll = 0.0f;

	m_nCollNumVtx = 0;

	mesh = NULL;
}

/*******************************************************************************
* 関数名：CMeshField::~CMeshField()
* 
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CMeshField::~CMeshField()
{
}

/*******************************************************************************
* 関数名：void CMeshField::Init( D3DXVECTOR3 pos, float width, float height, int row, int column )
* 
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CMeshField::Init( Vector3 pos, float width, float height, int row, int column )
{
	int nCntRow, nCntColumn, nIdxCnt;

	/* 変数代入 */
	m_Pos = pos;
	m_nRowBlock = row;
	m_nColumnBlock = column;
	m_nRowNumVtx = m_nRowBlock + 1;
	m_nColumnNumVtx = m_nColumnBlock + 1;
	m_fWidth = width;
	m_fDepth = height;
	m_nNumVtxMax = m_nRowNumVtx * m_nColumnNumVtx;
	m_nNumIdx = ( ( m_nColumnBlock + 1 ) * m_nRowBlock * 2 + ( m_nRowBlock - 1 ) * 2 );
	m_nNumPrim = ( m_nColumnBlock * m_nRowBlock * 2 + ( m_nRowBlock - 1 ) * 4  );

	m_fWidthAll = width * column;
	m_fDepthAll = height * row;

	mesh = new Vector3[ m_nNumVtxMax ];

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].x = m_Pos.x -( m_fWidth * m_nColumnBlock * 0.5f ) + m_fWidth * nCntColumn;
			//mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].y = g_HeightMap[ nCntRow ][ nCntColumn ] * 10.0f;
			mesh[ nCntColumn + nCntRow * m_nRowNumVtx ].y = 0.0f;
			mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].z = m_Pos.z + ( m_fDepth * m_nRowBlock * 0.5f ) - m_fDepth * nCntRow;
		}
	}

	LoadFile();

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの読み込み
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_FIELD_000);

	// 頂点バッファの確保
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * m_nNumVtxMax,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	// インデックスバッファの確保
	pDevice->CreateIndexBuffer( sizeof( WORD ) * m_nNumIdx,		// 確保するバッファサイズ
								D3DUSAGE_WRITEONLY,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&m_IdxBuff,
								NULL );

	// 頂点情報設定
	SetVertex();


	// インデックス情報設定
	WORD *pIdx;

	m_IdxBuff->Lock( 0, 0, ( void** )&pIdx, 0 );

	// 変数定義
	int x, y, cnt;

	for( y = 0, cnt = 0; y < m_nRowBlock; y++ )
	{
		for( x = 0; x < m_nColumnNumVtx; x++, cnt++ )
		{
			pIdx[ cnt * 2 + 0 ] = x + ( y + 1 ) * m_nColumnNumVtx;
			pIdx[ cnt * 2 + 1 ] = x + ( y + 0 ) * m_nColumnNumVtx;
			if( x == m_nColumnNumVtx - 1 && cnt * 2 < m_nNumIdx - 2 )
			{
				cnt++;
				pIdx[ cnt * 2 + 0 ] = x + ( y + 0 ) * m_nColumnNumVtx;
				pIdx[ cnt * 2 + 1 ] = ( y + 1 + 1 ) * m_nColumnNumVtx;
			}
		}
	}

	m_IdxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CMeshField::Uninit( void )
* 
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CMeshField::Uninit( void )
{
	// 解放処理
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
	SAFE_RELEASE( m_IdxBuff );
	SAFE_DELETE( mesh );
}

/*******************************************************************************
* 関数名：void CMeshField::Update( void )
* 
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CMeshField::Update( void )
{
	if (CManager::GetOperateMode() == CManager::OPMODE_MESH)
	{// フィールドエディタモード時
		// メッシュ操作
		Operate();

		// ベクトル
		Vector3 vector;

		// マウス取得
		//MOUSESTATE *mouse = CInput::GetMouseState();
		//// カメラ取得
		//CGame *game = (CGame*)CManager::GetMode();
		//CCamera *camera = game->GetCamera();
		////  XZ平面とスクリーン座標の交点算出
		//CManager::CalcScreenToXZ(&vector, mouse->localPos.x, mouse->localPos.y,
		//	SCREEN_WIDTH, SCREEN_HEIGHT, camera->GetMtxView(), camera->GetMtxProj());
		//// 座標設定
		//Collision(&vector);
		//CTreeObject::Set(vector, CTreeObject::m_nTexNum);

		//SetVertex();
		//SetColorVertex(m_nCollNumVtx, D3DCOLOR_RGBA(255, 0, 0, 255));
		//SetColorVertex(m_nCollNumVtx + 1, D3DCOLOR_RGBA(255, 0, 0, 255));
		//SetColorVertex(m_nCollNumVtx + m_nColumnNumVtx, D3DCOLOR_RGBA(255, 0, 0, 255));
		//SetColorVertex(m_nCollNumVtx + m_nColumnNumVtx + 1, D3DCOLOR_RGBA(255, 0, 0, 255));
	}

}
/*******************************************************************************
* 関数名：void CMeshField::Draw( void )
* 
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CMeshField::Draw( void )
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
	D3DXMatrixTranslation( &mtxTrans, 0.0f, 0.0f, 0.0f );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxTrans );
	
	// ワールドマトリックスを設定
	pDevice->SetTransform( D3DTS_WORLD, &m_MtxWorld );

	// ストリームにバインド
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_3D ) );

	// インデックスバッファをバインド
	pDevice->SetIndices( m_IdxBuff );

	// 頂点フォーマットの設定
	pDevice->SetFVF( FVF_VERTEX_3D );

	// テクスチャの設定
	pDevice->SetTexture( 0, *m_pTexture );

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLESTRIP,		// プリミティブの種類
					0,
					0,							
					m_nNumVtxMax,	// 頂点数
					0,
					m_nNumPrim );	// 描画するプリミティブ数
}

/*******************************************************************************
* 関数名：CMeshField *CMeshField::Create( Vector3 pos, float width, float height, int row, int column )
* 
* 引数	：D3DXVECTOR3 pos	// ポリゴンの原点位置
*		  float width		// 1ブロックの幅
*		  float height		// 1ブロックの高さ
*		  int row			// 列ブロック数
*		  int column		// 行ブロック数
* 戻り値：
* 説明	：フィールド設定処理
*******************************************************************************/
CMeshField *CMeshField::Create( Vector3 pos, float width, float height, int row, int column )
{
	CMeshField *meshField;
	meshField = new CMeshField;
	meshField->Init( pos, width, height, row, column );
	return meshField;
}

/*******************************************************************************
* 関数名：int CMeshField::Collision( Vector3 vector )
* 
* 引数	：
* 戻り値：
* 説明	：フィールドあたり判定処理
*******************************************************************************/
int CMeshField::Collision( Vector3 *vector )
{
	// 変数定義
	Vector3 posA, posB, posC;
	Vector3 VecAB, VecBC, VecCA, VecAP, VecBP, VecCP;
	Vector3 c1, c2, c3;
	Vector3 n1, n2;
	int columnIdx, rowIdx;
	int vtxIdx = 0;
	columnIdx = (int)(vector->x / m_fWidth + m_nColumnBlock * 0.5f);
	rowIdx = (int)(m_nRowBlock * 0.5f - vector->z / m_fDepth);
	vtxIdx = columnIdx + m_nRowNumVtx * rowIdx;
	if (vtxIdx < 0 || vtxIdx > m_nNumVtxMax)
	{
		vtxIdx = 0;
	}

	float f = 0;
	float height = 0;

	// 当たり判定
	Vector3 vec1 = mesh[vtxIdx] - mesh[vtxIdx + m_nColumnNumVtx + 1];
	Vector3 vec2 = *vector - mesh[vtxIdx + m_nColumnNumVtx + 1];
	float fNor1 = vec2.x * vec1.z - vec2.z * vec1.x;

	D3DXVec3Cross(&n1, &vec1, &vec2);
	D3DXVec3Normalize(&n1, &n1);

	if (fNor1 > 0)
	{
		// 当たり判定
		// 1面目
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + 1];
		posC = mesh[vtxIdx + m_nColumnNumVtx];

		VecAB = posB - posA;
		VecAP = *vector - posA;

		VecBC = posC - posB;
		VecBP = *vector - posB;

		VecCA = posA - posC;
		VecCP = *vector - posC;

		// 外積
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// 三角形の内側に点がある
			m_nCollNumVtx = vtxIdx;

			return 0;
		}
	}
	else
	{
		// 2面目
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + m_nColumnNumVtx];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = *vector - posA;
		D3DXVec3Normalize(&VecAB, &VecAB);
		D3DXVec3Normalize(&VecAP, &VecAP);

		VecBC = posC - posB;
		VecBP = *vector - posB;
		D3DXVec3Normalize(&VecBC, &VecBC);
		D3DXVec3Normalize(&VecBP, &VecBP);

		VecCA = posA - posC;
		VecCP = *vector - posC;
		D3DXVec3Normalize(&VecCA, &VecCA);
		D3DXVec3Normalize(&VecCP, &VecCP);

		// 外積
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// 三角形の内側に点がある
			m_nCollNumVtx = vtxIdx + m_nColumnNumVtx;

			return 0;
		}
	}

	
	return 1;
}

/*******************************************************************************
* 関数名：float CMeshField::GetHeight( D3DXVECTOR3 pos )
* 
* 引数	：
* 戻り値：
* 説明	：フィールドあたり判定処理
*******************************************************************************/
float CMeshField::GetHeight( Vector3 pos )
{
	// 変数定義
	Vector3 posA, posB, posC;
	Vector3 VecAB, VecBC, VecCA, VecAP, VecBP, VecCP;
	Vector3 c1, c2, c3;
	Vector3 n1, n2;
	int columnIdx, rowIdx;
	int vtxIdx;
	columnIdx = (int)(pos.x / m_fWidth + m_nColumnBlock * 0.5f);
	rowIdx = (int)(m_nRowBlock * 0.5f - pos.z / m_fDepth);
	vtxIdx = columnIdx + m_nRowNumVtx * rowIdx;

	float height = 0;

	// 当たり判定
	Vector3 vec1 = mesh[vtxIdx] - mesh[vtxIdx + m_nColumnNumVtx + 1];
	Vector3 vec2 = pos - mesh[vtxIdx + m_nColumnNumVtx + 1];
	float fNor1 = vec2.x * vec1.z - vec2.z * vec1.x;

	D3DXVec3Cross(&n1, &vec1, &vec2);
	D3DXVec3Normalize(&n1, &n1);

	if (fNor1 > 0)
	{
		// 1面目
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + 1];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = pos - posA;

		VecBC = posC - posB;
		VecBP = pos - posB;

		VecCA = posA - posC;
		VecCP = pos - posC;

		// 外積
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// 三角形の内側に点がある
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			height = mesh[num].y - ((n1.x * (pos.x - mesh[num].x) + n1.z * (pos.z - mesh[num].z)) / n1.y);
		}
	}
	else
	{
		// 2面目
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + m_nColumnNumVtx];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = pos - posA;
		D3DXVec3Normalize(&VecAB, &VecAB);
		D3DXVec3Normalize(&VecAP, &VecAP);

		VecBC = posC - posB;
		VecBP = pos - posB;
		D3DXVec3Normalize(&VecBC, &VecBC);
		D3DXVec3Normalize(&VecBP, &VecBP);

		VecCA = posA - posC;
		VecCP = pos - posC;
		D3DXVec3Normalize(&VecCA, &VecCA);
		D3DXVec3Normalize(&VecCP, &VecCP);

		// 外積
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// 三角形の内側に点がある
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			height = mesh[num].y - ((n1.x * (pos.x - mesh[num].x) + n1.z * (pos.z - mesh[num].z)) / n1.y);
		}
	}
	
	return height;
}

/*******************************************************************************
* 関数名：void CMeshField::SetVertex( void )
* 
* 引数	：
* 戻り値：
* 説明	：フィールド頂点設定処理
*******************************************************************************/
void CMeshField::SetVertex( void )
{
	// 頂点情報設定
	VERTEX_3D *pVtx;
	int nCntRow, nCntColumn, nIdxCnt;
	D3DXVECTOR3 n1, n2, n3, n4, n5, n6;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			pVtx[ nIdxCnt ].pos.x = mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].x;
			pVtx[ nIdxCnt ].pos.y = mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].y;
			pVtx[ nIdxCnt ].pos.z = mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].z;

			pVtx[ nIdxCnt ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			pVtx[ nIdxCnt ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

			//テクスチャブロック表示
			pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			//テクスチャ全面表示
			/*pVtx[ nIdxCnt ].tex.x = ( 1.0f / m_nColumnBlock ) * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = ( 1.0f / m_nRowBlock ) * nCntRow;*/
		}
	}

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			// 法線設定 ///
			if( nCntRow == 0 )
			{
				if( nCntColumn == 0 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 ) );
				}
				else if( nCntColumn == m_nColumnNumVtx - 1 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt +  m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 ) );
				}
				else
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Cross( &n3,
									&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n3, &n3 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
				}
			}
			else if( nCntRow == m_nRowNumVtx - 1 )
			{
				if( nCntColumn == 0 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 ) );
				}
				else if( nCntColumn == m_nColumnNumVtx - 1 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 ) );
				}
				else
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Cross( &n3,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n3, &n3 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
				}
			}
			else if( nCntColumn == 0 && nCntRow != 0 && nCntRow != m_nRowNumVtx - 1 )
			{
				D3DXVec3Cross( &n1,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n1, &n1 );
				D3DXVec3Cross( &n2,
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n2, &n2 );
				D3DXVec3Cross( &n3,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n3, &n3 );
				D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
			}
			else if( nCntColumn == m_nColumnNumVtx - 1 && nCntRow != 0 && nCntRow != m_nRowNumVtx - 1 )
			{
				D3DXVec3Cross( &n1,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n1, &n1 );
				D3DXVec3Cross( &n2,
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n2, &n2 );
				D3DXVec3Cross( &n3,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n3, &n3 );
				D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
			}
			else
			{
				D3DXVec3Cross( &n1,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n1, &n1 );
				D3DXVec3Cross( &n2,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n2, &n2 );
				D3DXVec3Cross( &n3,
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n3, &n3 );
				D3DXVec3Cross( &n4,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n4, &n4 );
				D3DXVec3Cross( &n5,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n5, &n5 );
				D3DXVec3Cross( &n6,
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n6, &n6 );

				D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 + n4 + n5 + n6 ) );
			}
		}
	}

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CMeshField::SetColorVertex( int idx, D3DCOLOR col )
* 
* 引数	：
* 戻り値：
* 説明	：フィールド頂点設定処理
*******************************************************************************/
void CMeshField::SetColorVertex( int idx, D3DCOLOR col )
{
	// 頂点情報設定
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ idx ].col = col;

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CMeshField::ResetPosition( void )
* 
* 引数	：
* 戻り値：
* 説明	：フィールドリセット処理
*******************************************************************************/
void CMeshField::ResetPosition( void )
{
	int nCntRow, nCntColumn, nIdxCnt;

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].y = 0.0f;
		}
	}
}

/*******************************************************************************
* 関数名：void CMeshField::Operate( void )
* 
* 引数	：
* 戻り値：
* 説明	：頂点操作処理
*******************************************************************************/
void CMeshField::Operate( void )
{
	CInput *input = CManager::GetInput();
	if(input->GetKeyboardPress( DIK_F5 ) )
	{
		// 高さ初期化
		ResetPosition();
	}
	if(input->GetKeyboardPress( DIK_F7 ) )
	{
		// 高さ保存
		SaveFile();
	}
	if(input->GetKeyboardPress( DIK_F10 ) )
	{
		// 高さロード
		LoadFile();
	}

	// マウスクリック
	/*if(input->GetMousePress( DIM_LEFT ) )
	{
		mesh[ m_nCollNumVtx ].y += OPARATE_SPEED;
		mesh[ m_nCollNumVtx + 1 ].y += OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx ].y += OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx + 1 ].y += OPARATE_SPEED;
	}
	if(input->GetMousePress( DIM_RIGHT ) )
	{
		mesh[ m_nCollNumVtx ].y -= OPARATE_SPEED;
		mesh[ m_nCollNumVtx + 1 ].y -= OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx ].y -= OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx + 1 ].y -= OPARATE_SPEED;
	}*/
}

/*******************************************************************************
* 関数名：void CMeshField::SaveFile( void )
* 
* 引数	：
* 戻り値：
* 説明	：ファイルセーブ処理
*******************************************************************************/
int CMeshField::SaveFile( void )
{
	FILE *fp;

	// ファイル読み込み開始
	if ( ( fp = fopen( TEXTFILENAME, "w" ) ) == NULL)
	{
		return EXIT_FAILURE;
	}

	for( int i = 0; i < m_nNumVtxMax; i++ )
	{
		fprintf( fp, "POSY %f\n", mesh[ i ].y );
	}

	// ファイル読み込み終了
	fclose( fp );

	return 0;
}

/*******************************************************************************
* 関数名：void CMeshField::LoadFile( void )
* 
* 引数	：
* 戻り値：
* 説明	：ファイルロード処理
*******************************************************************************/
int CMeshField::LoadFile( void )
{
	FILE *fp;

	// ファイル読み込み開始
	if ( ( fp = fopen( TEXTFILENAME, "r" ) ) == NULL)
	{
		return EXIT_FAILURE;
	}

	for( int i = 0; i < m_nNumVtxMax; i++ )
	{
		fscanf( fp, "POSY %f\n", &mesh[ i ].y );
	}

	// ファイル読み込み終了
	fclose( fp );

	return 0;
}
/*******************************************************************************
* 関数名：Vector3 CMeshField::GetNormal( void )
* 
* 引数	：
* 戻り値：
* 説明	：法線取得処理
*******************************************************************************/
Vector3 CMeshField::GetNormal( Vector3 *vector )
{
	// 変数定義
	Vector3 posA, posB, posC;
	Vector3 VecAB, VecBC, VecCA, VecAP, VecBP, VecCP;
	Vector3 c1, c2, c3;
	Vector3 n1, n2;
	int columnIdx, rowIdx;
	int vtxIdx;
	columnIdx = (int)(vector->x / m_fWidth + m_nColumnBlock * 0.5f);
	rowIdx = (int)(m_nRowBlock * 0.5f - vector->z / m_fDepth);
	vtxIdx = columnIdx + m_nRowNumVtx * rowIdx;

	float height = 0;

	// 当たり判定
	Vector3 vec1 = mesh[vtxIdx] - mesh[vtxIdx + m_nColumnNumVtx + 1];
	Vector3 vec2 = *vector - mesh[vtxIdx + m_nColumnNumVtx + 1];
	float fNor1 = vec2.x * vec1.z - vec2.z * vec1.x;

	D3DXVec3Cross(&n1, &vec1, &vec2);
	D3DXVec3Normalize(&n1, &n1);

	if (fNor1 > 0)
	{
		// 1面目
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + 1];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = *vector - posA;

		VecBC = posC - posB;
		VecBP = *vector - posB;

		VecCA = posA - posC;
		VecCP = *vector - posC;

		// 外積
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// 三角形の内側に点がある
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			return n1;
		}
	}
	else
	{
		// 2面目
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + m_nColumnNumVtx];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = *vector - posA;
		D3DXVec3Normalize(&VecAB, &VecAB);
		D3DXVec3Normalize(&VecAP, &VecAP);

		VecBC = posC - posB;
		VecBP = *vector - posB;
		D3DXVec3Normalize(&VecBC, &VecBC);
		D3DXVec3Normalize(&VecBP, &VecBP);

		VecCA = posA - posC;
		VecCP = *vector - posC;
		D3DXVec3Normalize(&VecCA, &VecCA);
		D3DXVec3Normalize(&VecCP, &VecCP);

		// 外積
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// 三角形の内側に点がある
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			return n1;
		}
	}
	
	return n1;
}

/*******************************************************************************
* 関数名：float CMeshField::GetWidthAll( void )
*
* 引数	：
* 戻り値：
* 説明	：フィールド全体の幅取得処理
*******************************************************************************/
float CMeshField::GetWidthAll(void)
{
	return m_fWidthAll;
}

/*******************************************************************************
* 関数名：float CMeshField::GetDepthAll( void )
*
* 引数	：
* 戻り値：
* 説明	：フィールド全体の奥行取得処理
*******************************************************************************/
float CMeshField::GetDepthAll(void)
{
	return m_fDepthAll;
}