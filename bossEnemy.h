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
#pragma once

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
class CBarrier;
class CChargeEffect;

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CBossEnemy : public CEnemy
{
public:
	typedef enum
	{
		STATE_NORMAL,		// 通常
		STATE_MOVE,			// 移動
		STATE_ATTACK_0,		// 攻撃1
		STATE_DAMAGE,		// 被弾
		STATE_DEATH,		// 死亡時
		STATE_GUARD,		// ガード時
		STATE_ATTACK_1,		// 攻撃2
		STATE_SUMMON,		// 召喚時
		STATE_MAX
	}STATE;
	CBossEnemy(DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_ENEMY);
	~CBossEnemy();
	void Init(Vector3 pos);
	void Uninit(void);
	void Update(void);

	static CBossEnemy *Create(Vector3 pos);
protected:
	CBarrier *m_pBarrier;

	void SetBullet(CScene *scene);
	void UpdateState(void);
	void UpdateRot(void);
	void SetCharge(void);
	void SetLazer(void);
	void SetSummon(void);
};

