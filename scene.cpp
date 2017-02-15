/*******************************************************************************
* タイトル名：
* ファイル名：scene.cpp
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
#include "list.h"
#include "renderer.h"
#include "scene.h"

/*******************************************************************************
* グローバル変数
*******************************************************************************/
CScene *CScene::m_Top[DRAWORDER_MAX] = { NULL };
CScene *CScene::m_Cur[DRAWORDER_MAX] = { NULL };

/*******************************************************************************
* 関数名：CScene::CScene()
*
* 引数	：
* 戻り値：
* 説明	：コンストラクタ
*******************************************************************************/
CScene::CScene(DRAWORDER DrawOrder, OBJTYPE ObjType)
{
	if (m_Top[DrawOrder] == NULL)
	{// 先頭に何も入っていなかったら
		m_Top[DrawOrder] = this;			// 先頭に自分のポインタを代入
		m_Cur[DrawOrder] = this;			// 現在に自分のポインタを代入
		m_Prev = NULL;						// 自分の前のポインタにNULLを設定
		m_Next = NULL;						// 自分の次のポインタにNULLを設定
	}
	else
	{// 先頭が埋まっていたら
		m_Cur[DrawOrder]->m_Next = this;	// 現在の次のポインタに自分のポインタを代入
		m_Prev = m_Cur[DrawOrder];			// 自分の前のポインタに現在のポインタを代入
		m_Cur[DrawOrder] = this;			// 現在に自分のポインタを代入
		m_Next = NULL;						// 自分の次のポインタにNULLを設定
	}

	m_Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_Rot = Vector3(0.0f, 0.0f, 0.0f);
	m_Delete = false;
	m_bPause = false;
	m_bDraw = true;
	SetObjType(ObjType);
	SetDrawOrder(DrawOrder);
}

/*******************************************************************************
* 関数名：CScene::~CScene()
*
* 引数	：
* 戻り値：
* 説明	：デストラクタ
*******************************************************************************/
CScene::~CScene()
{
}

/*******************************************************************************
* 関数名：void CScene::UninitAll( void )
*
* 引数	：
* 戻り値：
* 説明	：全終了処理
*******************************************************************************/
void CScene::UninitAll(void)
{
	for (int i = 0; i < DRAWORDER_MAX; i++)
	{
		CScene *scene = m_Top[i];
		CScene *next = NULL;
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			scene->Uninit();
			delete scene;
			scene = next;
		}
		m_Top[i] = NULL;
	}
}

/*******************************************************************************
* 関数名：void CScene::UpdateAll( void )
*
* 引数	：
* 戻り値：
* 説明	：全更新処理
*******************************************************************************/
void CScene::UpdateAll(void)
{
	for (int i = 0; i < DRAWORDER_MAX; i++)
	{
		CScene *scene = m_Top[i];
		CScene *next;

		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			if (!scene->m_Delete && !scene->m_bPause)
			{
				scene->Update();
			}
			scene = next;
		}

		scene = m_Top[i];

		while (scene != NULL)
		{
			next = scene->m_Next;	// delete時のメモリリーク回避のためにポインタを格納
			if (scene->m_Delete)
			{
				scene->Release(scene->m_DrawOrder);
			}
			scene = next;
		}
	}
}

/*******************************************************************************
* 関数名：void CScene::DrawAll( void )
*
* 引数	：
* 戻り値：
* 説明	：全描画処理
*******************************************************************************/
void CScene::DrawAll(void)
{
	for (int i = 0; i < DRAWORDER_MAX; i++)
	{
		CScene *scene = m_Top[i];
		while (scene != NULL)
		{
			if (scene->m_bDraw)
			{
				scene->Draw();
			}
			scene = scene->m_Next;
		}
	}
}

/*******************************************************************************
* 関数名：void CScene::SetPosition( Vector3 pos )
*
* 引数	：
* 戻り値：
* 説明	：位置設定処理
*******************************************************************************/
void CScene::SetPosition(Vector3 pos)
{
	m_Pos = pos;
}

/*******************************************************************************
* 関数名：void CScene::Release( DRAWORDER DrawOrder )
*
* 引数	：
* 戻り値：
* 説明	：解放処理
*******************************************************************************/
void CScene::Release(DRAWORDER DrawOrder)
{
	if (this == m_Top[DrawOrder])
	{// 自分が先頭のとき
		m_Top[DrawOrder] = this->m_Next;		// 先頭を自分の次のポインタに設定
	}
	else
	{// 自分が先頭ではないとき
		this->m_Prev->m_Next = this->m_Next;	// 自分の次のポインタを設定
	}

	if (this == m_Cur[DrawOrder])
	{// 自分が末尾のとき
		m_Cur[DrawOrder] = this->m_Prev;		// 先頭を自分の次のポインタに設定
	}
	else
	{// 自分が末尾ではないとき
		this->m_Next->m_Prev = this->m_Prev;	// 自分の前のポインタを設定
	}

	this->Uninit();
	delete this;
}

/*******************************************************************************
* 関数名：CScene *CScene::GetList( void )
*
* 引数	：
* 戻り値：
* 説明	：リスト取得処理
*******************************************************************************/
CScene *CScene::GetList(DRAWORDER DrawOrder)
{
	return m_Top[DrawOrder];
}

/*******************************************************************************
* 関数名：CScene *CScene::GetCur( void )
*
* 引数	：
* 戻り値：
* 説明	：リスト現在取得処理
*******************************************************************************/
CScene *CScene::GetCur(DRAWORDER DrawOrder)
{
	return m_Cur[DrawOrder];
}

/*******************************************************************************
* 関数名：CScene *CScene::GetNext( void )
*
* 引数	：
* 戻り値：
* 説明	：リスト次項取得処理
*******************************************************************************/
CScene *CScene::GetNext(void)
{
	return m_Next;
}