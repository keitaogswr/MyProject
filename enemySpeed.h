/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FenemySpeed.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#pragma once

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "enemy.h"

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
class CEnemySpeed : public CEnemy
{
public:
	CEnemySpeed(DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_ENEMY);
	~CEnemySpeed();

	void Init(Vector3 pos);
	void Uninit(void);
	void Update(void);

	void SetState(int state);	// ��Ԑݒ�

	static CEnemySpeed *Create(Vector3 pos);
protected:
	void UpdateState(void);		// ��ԍX�V
	void UpdateMove(void);		// �ړ��X�V
	bool CheckCollision(Vector3 pos, float collision);
};
