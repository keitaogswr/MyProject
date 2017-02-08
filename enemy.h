/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fenemy.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _ENEMY_H_
#define _ENEMY_H_

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
class CStencilShadow;
class CAfterBurner;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CEnemy : public CDynamicModel
{
public:
	typedef enum
	{
		STATE_NORMAL,		// �ʏ�
		STATE_MOVE,			// �ړ�
		STATE_ATTACK,		// �U����
		STATE_DAMAGE,		// ��e
		STATE_DEATH,		// ���S��
		STATE_GUARD,		// �K�[�h��
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
	CStencilShadow *m_Shadow;		// �e
	int m_nLife;					// ���C�t
	bool m_bSearch;					// ���G�t���O
	int m_nAttCnt;					// �U���J�E���^
	Vector3 m_TargetPos;			// ���b�N�I���������W
	float m_fCollisionLength;		// �����蔻�苗��
	int m_State;					// ���
	int m_nStateCnt;				// ��ԃJ�E���^
	int m_nSearchCnt;				// ���G�J�E���^
	CAfterBurner *m_pAfterBurner;

	void UpdateState(void);			// ��ԍX�V
	void UpdateSearch(void);		// ���G�X�V
	void UpdateAttack(void);		// �U���X�V
};

#endif