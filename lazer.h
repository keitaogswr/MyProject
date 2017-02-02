/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Flazer.h
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
#include "meshCylinder.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CLazer : public CMeshCylinder
{
protected:
	virtual void SetRenderStateBegin(void);			// �����_���[�X�e�[�g�ݒ�J�n
	virtual void SetRenderStateEnd(void);			// �����_���[�X�e�[�g�ݒ�I��

	CMeshCylinder *m_pCylinder;
	int m_nDeleteCnt;								// ���ŃJ�E���^
public:
	CLazer(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~CLazer();

	void Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	virtual void SetWorldMatrix(void);				// ���[���h�}�g���b�N�X�ݒ�

	static CLazer *Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix);

	void Collision(void);
};