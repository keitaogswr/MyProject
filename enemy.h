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
class CAfterBurner;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CEnemy : public CDynamicModel
{
public:
	typedef enum
	{
		STATE_NORMAL,		// 通常
		STATE_MOVE,			// 移動
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
	float GetCollisionLength(void) { return m_fCollisionLength; }
	void SetState(int state) { m_State = state; m_nStateCnt = 0; m_nAttCnt = 0; }
	int GetState(void) { return m_State; }

protected:
	CStencilShadow *m_Shadow;		// 影
	int m_nLife;					// ライフ
	bool m_bSearch;					// 索敵フラグ
	int m_nAttCnt;					// 攻撃カウンタ
	Vector3 m_TargetPos;			// ロックオンされる座標
	float m_fCollisionLength;		// あたり判定距離
	int m_State;					// 状態
	int m_nStateCnt;				// 状態カウンタ
	int m_nSearchCnt;				// 索敵カウンタ
	CAfterBurner *m_pAfterBurner;

	void UpdateState(void);			// 状態更新
	void UpdateSearch(void);		// 索敵更新
	void UpdateAttack(void);		// 攻撃更新
};

#endif