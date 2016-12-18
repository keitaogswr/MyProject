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
		STATE_DEATH,		// 死亡時
		STATE_GUARD,		// ガード時
		STATE_MAX
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
	void SetState(STATE state) { m_State = state; m_nStateCnt = 0; }
	int GetState(void) { return m_State; }

protected:
	CStencilShadow *m_Shadow;		// 影
	int m_nLife;					// ライフ
	bool m_bSearch;					// 索敵フラグ
	int m_nAttCnt;					// 攻撃カウンタ
	Vector3 m_TargetPos;			// ロックオンされる座標
	int m_nCollisionLength;			// あたり判定距離
	STATE m_State;					// 状態
	int m_nStateCnt;				// 状態カウンタ

	void UpdateState(void);
};

#endif