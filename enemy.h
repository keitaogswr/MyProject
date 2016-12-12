/*******************************************************************************
* タイトル名：
* ファイル名：enemy.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _ENEMY_H_
#define _ENEMY_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CSceneX;
class CDynamicModel;
class CStencilShadow;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CEnemy : public CDynamicModel
{
public:
	typedef enum
	{
		STATE_NORMAL,		// 通常
		STATE_ATTACK,		// 攻撃中
		STATE_DAMAGE,		// 被弾
		STATE_DEATH			// 死亡時
	}STATE;
	CEnemy(DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_ENEMY);
	~CEnemy();
	void Init(Vector3 pos);
	void Uninit(void);
	void Update(void);

	static int Load(void);

	static CEnemy *Create(Vector3 pos);

	void SetDamage(int nDamage) { m_nLife -= nDamage; }
	Vector3 GetTargetPos(void) { return m_TargetPos; }
	int GetCollisionLength(void) { return m_nCollisionLength; }
	void SetState(STATE state) { m_State = state; }

protected:
	CStencilShadow *m_Shadow;
	int m_nLife;
	bool m_bSearch;
	int m_nAttCnt;
	Vector3 m_TargetPos;			// ロックオンされる座標
	int m_nCollisionLength;
	STATE m_State;
	int m_nStateCnt;

	void UpdateState(void);
};

#endif