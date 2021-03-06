/*******************************************************************************
* タイトル名：
* ファイル名：player.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _PLAYER_H_
#define _PLAYER_H_

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
class CScene;
class CSceneX;
class CDynamicModel;
class CMotionManager;
class CShadow;
class COrbit;
class CStencilShadow;
class CPlayerLifeGauge;
class CAfterBurner;

/*******************************************************************************
* クラス宣言
*******************************************************************************/

/* プレイヤーステート */
typedef enum PLAYERSTATE
{
	PLAYERSTATE_WAIT = 0,				// 待機
	PLAYERSTATE_WALK,					// 前進
	PLAYERSTATE_DAMP,					// 着地
	PLAYERSTATE_ATTACK,					// 攻撃
	PLAYERSTATE_TRANSFORM,				// 変形
	PLAYERSTATE_JUMP,					// ジャンプ
	PLAYERSTATE_BOMBED,					// 被弾時
	PLAYERSTATE_VEHICLE_WAIT,			// 待機(乗り物)
	PLAYERSTATE_VEHICLE_WALK,			// 歩行(乗り物)
	PLAYERSTATE_VEHICLE_TRANSFORM,		// 変形(乗り物)
	PLAYERSTATE_MAX
}PLAYERSTATE;

/* プレイヤーモード */
typedef enum
{
	PLAYERMODE_HUMAN = 0,				// 人
	PLAYERMODE_VEHICLE,					// 乗り物
	PLAYERMODE_TRANSFORM,				// 変形中
	PLAYERMODE_MAX
}PLAYERMODE;

class CPlayer : public CDynamicModel
{
protected:
	PLAYERSTATE m_State;				// プレイヤーの状態
	PLAYERMODE m_Mode;					// プレイヤーのモード
	bool m_bMove;						// 移動フラグ
	bool m_bJump;						// ジャンプフラグ
	bool m_bRockOn;						// ロックオンフラグ
	bool m_bBullet;						// 射撃フラグ
	CStencilShadow *m_Shadow;			// 影ポインタ
	COrbit *m_Orbit[2];					// オービットポインタ
	int m_nLife;						// ライフ
	int m_nTarget;						// ターゲットしている敵ID
	int m_nExplosionCnt;				// 爆発カウンタ
	int m_nKeep;						// 仰け反り耐久値

	virtual void SetMode(PLAYERMODE mode);
	virtual void UpdateState(void);
	virtual void SetState(PLAYERSTATE state);
	virtual void SetStateJoyStick(PLAYERSTATE state);
	virtual void Operate(void);
	virtual void OperateJoyStick(void);
	virtual void UpdateMode(void);
	virtual void SetOrbit(void);
	virtual void UpdateRockOn(void);
	virtual void UpdateKeep(void);
public:
	CPlayer(DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_PLAYER);
	~CPlayer();

	static void Load(void);
	static void Unload(void);
	void Init(Vector3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CPlayer *Create(Vector3 pos);
	virtual void SetDamage(int nDamage) { m_nLife -= nDamage; }
	virtual void SetKeep(int keep) { m_nKeep -= keep; }
	int GetTargetId(void) { return m_nTarget; }
	void SetTargetId(int id) { m_nTarget = id; }
};

#endif