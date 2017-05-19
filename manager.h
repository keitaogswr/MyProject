/*******************************************************************************
* タイトル名：
* ファイル名：manager.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _MANAGER_H_
#define _MANAGER_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
/*******************************************************************************
* ライブラリのリンク
*******************************************************************************/

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define SCENE_MAX		( 4 )
#define MODE_DEFAULT	( MODE_GAME )

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CRenderer;
class CMode;
class CInput;
class HewNetLib;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
// マネージャークラス
class CManager
{
public:
	typedef enum
	{
		OPMODE_PLAYER,
		OPMODE_MESH,
		OPMODE_MAX
	}OPMODE;
	CManager();
	~CManager();
	static void Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	static void Uninit(void);
	static void Update(void);
	static void Draw(void);
	static CRenderer *GetRenderer(void);
	static HWND *GetHWND(void);

	static void SetOperateMode(OPMODE opmode) { m_Opmode = opmode; }
	static OPMODE GetOperateMode(void) { return m_Opmode; }

	static D3DXVECTOR3* CalcScreenToWorld(D3DXVECTOR3* pout, int Sx, int Sy, float fZ,
		int Screen_w, int Screen_h, D3DXMATRIX* View, D3DXMATRIX* Prj);
	static D3DXVECTOR3* CalcScreenToXZ(D3DXVECTOR3* pout, int Sx, int Sy, int Screen_w, int Screen_h, D3DXMATRIX* View, D3DXMATRIX* Prj);
	static void CheckRot(float *f);
	static void CheckRot(Vector3 *rot);
	static void SetMode(CMode *mode);
	static CMode *GetMode(void);
	static CInput *GetInput(void) { return m_Input; }
private:
	static CRenderer *m_Renderer;
	static HWND *m_hWnd;
	static CMode *m_Mode;
	static OPMODE m_Opmode;
	static CInput *m_Input;
};

#endif