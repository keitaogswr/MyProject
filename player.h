/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fplayer.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _PLAYER_H_
#define _PLAYER_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CSceneX;
class CDynamicModel;
class CMotionManager;
class CShadow;
class COrbit;
class CStencilShadow;
class CPlayerLifeGauge;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/

/* �v���C���[�X�e�[�g */
typedef enum PLAYERSTATE
{
	PLAYERSTATE_WAIT = 0,				// �ҋ@
	PLAYERSTATE_WALK,					// �O�i
	PLAYERSTATE_DAMP,					// ���n
	PLAYERSTATE_ATTACK,					// �U��
	PLAYERSTATE_TRANSFORM,				// �ό`
	PLAYERSTATE_JUMP,					// �W�����v
	PLAYERSTATE_VEHICLE_WAIT,			// �ҋ@(��蕨)
	PLAYERSTATE_VEHICLE_WALK,			// ���s(��蕨)
	PLAYERSTATE_VEHICLE_TRANSFORM,		// �ό`(��蕨)
	PLAYERSTATE_MAX
}PLAYERSTATE;

/* �v���C���[���[�h */
typedef enum
{
	PLAYERMODE_HUMAN = 0,				// �l
	PLAYERMODE_VEHICLE,					// ��蕨
	PLAYERMODE_TRANSFORM,				// �ό`��
	PLAYERMODE_MAX
}PLAYERMODE;

class CPlayer : public CDynamicModel
{
protected:
	PLAYERSTATE m_State;				// �v���C���[�̏��
	PLAYERMODE m_Mode;					// �v���C���[�̃��[�h
	bool m_bMove;						// �ړ��t���O
	bool m_bJump;						// �W�����v�t���O
	bool m_bRockOn;						// ���b�N�I���t���O
	bool m_bBullet;						// �ˌ��t���O
	CStencilShadow *m_Shadow;			// �e�|�C���^
	COrbit *m_Orbit[2];					// �I�[�r�b�g�|�C���^
	int m_nLife;						// ���C�t
	Vector3 m_TargetPos;				// �^�[�Q�b�g�����ʒu
	int m_nExplosionCnt;				// �����J�E���^

	virtual void SetState(PLAYERSTATE state);
	virtual void SetMode(PLAYERMODE mode);
	virtual void UpdateState(void);
	virtual void Operate(void);
	virtual void UpdateMode(void);
	virtual void SetOrbit(void);
	virtual void UpdateRockOn(void);
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
	Vector3 GetTargetPos(void) { return m_TargetPos; }
};

#endif