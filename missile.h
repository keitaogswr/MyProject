/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fbullet.h
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
#include "bullet.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CScene;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CMissile : public CBullet
{
private:
	int m_nSmokeCnt;				// ���J�E���^
	int m_nSearchCnt;				// ���G�J�E���^
	int m_nTargetId;				// �^�[�Q�b�g��ID
	D3DXQUATERNION m_Quaternion;	// �N�H�[�^�j�I��

	void UpdateCollision(void);		// �����蔻��
	void UpdateQuaternion(void);	// �N�H�[�^�j�I���X�V
public:
	CMissile(DRAWORDER DrawOrder = DRAWORDER_3DLATE, OBJTYPE ObjType = OBJTYPE_BILLBOARD);
	~CMissile();

	void Init(Vector3 pos, Vector3 vec, int id);
	void Update(void);
	void Draw(void);

	static CMissile *Create(Vector3 pos, Vector3 vec, int id);
};
