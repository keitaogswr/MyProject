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

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CEnemy : public CDynamicModel
{
public:
	typedef enum
	{
		STATE_NORMAL,		// �ʏ�
		STATE_ATTACK,		// �U����
		STATE_DAMAGE,		// ��e
		STATE_DEATH			// ���S��
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
	Vector3 m_TargetPos;			// ���b�N�I���������W
	int m_nCollisionLength;
	STATE m_State;
	int m_nStateCnt;

	void UpdateState(void);
};

#endif