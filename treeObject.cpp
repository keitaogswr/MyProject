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
#include "mode.h"
#include "game.h"
#include "load.h"
#include "renderer.h"
#include "camera.h"
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "billboard.h"
#include "treeObject.h"
#include "input.h"
#include "texture.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define TEXTFILENAME	( "data\\EDITDATA\\BillboardData.txt" )

/*******************************************************************************
* グローバル変数
*******************************************************************************/
CTreeObject::TREEOBJ_PARAM CTreeObject::treeObjParam[TREEOBJ_TYPE_MAX] =
{//		テクスチャ名				幅		高さ
	{ TEXTURE_TYPE_TREE_000, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_001, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_002, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_003, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_004, 150.0f, 150.0f },
	{ TEXTURE_TYPE_WEED_000, 50.0f, 50.0f },
	{ TEXTURE_TYPE_WEED_001, 50.0f, 50.0f },
	{ TEXTURE_TYPE_WEED_002, 50.0f, 50.0f }
};
int CTreeObject::m_nTexNum = 0;

/*******************************************************************************
* 関数名：CTreeObject::CTreeObject()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CTreeObject::CTreeObject(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBillboard(DrawOrder, ObjType)
{
	m_nTreeType = 0;
	billboardType = TREEOBJ_TYPE_MAX;
}

/*******************************************************************************
* 関数名：CTreeObject::~CTreeObject()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CTreeObject::~CTreeObject()
{
}

/*******************************************************************************
* 関数名：void CTreeObject::Init( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：初期化処理
*******************************************************************************/
void CTreeObject::Init(Vector3 pos, int num)
{
	m_Pos = pos;
	m_nTreeType = num;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// テクスチャの生成
	m_pTexture = CTexture::GetTexture(treeObjParam[m_nTreeType].nIdx);

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	// 頂点バッファの設定
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-treeObjParam[m_nTreeType].width * 0.5f,
								treeObjParam[m_nTreeType].height,
								0.0f);
	pVtx[1].pos = D3DXVECTOR3(treeObjParam[m_nTreeType].width * 0.5f,
								treeObjParam[m_nTreeType].height,
								0.0f);
	pVtx[2].pos = D3DXVECTOR3(-treeObjParam[m_nTreeType].width * 0.5f,
								0.0f,
								0.0f);
	pVtx[3].pos = D3DXVECTOR3(treeObjParam[m_nTreeType].width * 0.5f,
								0.0f,
								0.0f);

	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* 関数名：void CTreeObject::Uninit( void )
*
* 引数	：
* 戻り値：
* 説明	：終了処理
*******************************************************************************/
void CTreeObject::Uninit(void)
{
	// 解放
	SAFE_RELEASE(m_VtxBuff);
}

/*******************************************************************************
* 関数名：void CTreeObject::Update( void )
*
* 引数	：
* 戻り値：
* 説明	：更新処理
*******************************************************************************/
void CTreeObject::Update(void)
{
}

/*******************************************************************************
* 関数名：void CTreeObject::Draw( void )
*
* 引数	：
* 戻り値：
* 説明	：描画処理
*******************************************************************************/
void CTreeObject::Draw(void)
{
	/* 変数定義 */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// スケール、向き、ポジション
	D3DXMATRIX mtxView;

	// デバイスの取得
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();

	// 各種設定 /////
	// ライトのOFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ZテストのON
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	// アルファテストのON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);

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

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// ストリームにバインド
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, *m_pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//プリミティブの種類
		0,							//最初の頂点のインデックス
		2);							//描画するプリミティブ数

	// 設定を元に戻す /////
	// ライトのON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// アルファテストのOFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// ZテストのOFF
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

/*******************************************************************************
* 関数名：CTreeObject *CTreeObject::Create( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
CTreeObject *CTreeObject::Create(Vector3 pos, int num)
{
	CLoad *load = (CLoad*)CManager::GetMode();
	CGame *game = (CGame*)load->GetMode();
	//CGame *game = (CGame*)CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();
	if (meshField == NULL)
	{
		return NULL;
	}
	pos.y = meshField->GetHeight(pos);

	CTreeObject *object;
	object = new CTreeObject;
	object->Init(pos, num);
	return object;
}

/*******************************************************************************
* 関数名：void CTreeObject::Set( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：生成処理
*******************************************************************************/
void CTreeObject::Set(Vector3 pos, int num)
{
	CScene *scene = NULL;
	if (CManager::GetOperateMode() == CManager::OPMODE_PLAYER)
	{
		if (CInput::GetMouseTrigger(DIM_LEFT))
		{
			scene = Create(pos, num);
		}
		if (CInput::GetMouseTrigger(DIM_RIGHT))
		{
			m_nTexNum++;
			if (m_nTexNum >= TREEOBJ_TYPE_MAX)
			{
				m_nTexNum = 0;
			}
		}
	}
}

/*******************************************************************************
* 関数名：int CTreeObject::SaveFile( void )
*
* 引数	：
* 戻り値：
* 説明	：ファイルセーブ処理
*******************************************************************************/
int CTreeObject::SaveFile(void)
{
	FILE *fp;

	// ファイル読み込み開始
	if ((fp = fopen(TEXTFILENAME, "w")) == NULL)
	{
		return EXIT_FAILURE;
	}

	int nNumBill = 0;
	CScene *scene = GetList(DRAWORDER_3DLATE);

	while (scene != NULL)
	{
		if (scene->GetObjType() == OBJTYPE_BILLBOARD)
		{
			nNumBill++;
			CTreeObject *tree;
			tree = (CTreeObject*)scene;
			fprintf(fp, "POSXZ %f %f %d\n", tree->GetPosition().x, tree->GetPosition().z, tree->GetTreeType());
		}
		scene = scene->m_Next;
	}

	// ファイル読み込み終了
	fclose(fp);

	return 0;
}

/*******************************************************************************
* 関数名：int CTreeObject::LoadFile( void )
*
* 引数	：
* 戻り値：
* 説明	：ファイルロード処理
*******************************************************************************/
int CTreeObject::LoadFile(void)
{
	FILE *fp;

	// ファイル読み込み開始
	if ((fp = fopen(TEXTFILENAME, "r")) == NULL)
	{
		return EXIT_FAILURE;
	}

	int nNumBill = 0;
	CTreeObject *bill = NULL;
	int num = 0;
	Vector3 vector;
	char str[256];

	while (fscanf(fp, "%s", str) != EOF)
	{
		if (strcmp(str, "POSXZ") == 0)
		{
			fscanf(fp, "%f %f %d", &vector.x, &vector.z, &num);
			bill = Create(vector, num);
		}
	}


	// ファイル読み込み終了
	fclose(fp);

	return 0;
}