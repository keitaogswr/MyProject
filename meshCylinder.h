/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FmeshCylinder.h
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
#include "scene3D.h"

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
class CMeshCylinder : public CScene3D
{
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;				// ���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DINDEXBUFFER9	m_pIdxBuff;				// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^

	float m_fRadius;								// �����a
	float m_fRadiusN;								// �����a�ڕW�l
	float m_fHeight;								// 1�u���b�N�̍���
	float m_fHeightN;								// 1�u���b�N�̍���
	int m_nRowBlock;								// �s�u���b�N��
	int m_nColumnBlock;								// ��u���b�N��
	int m_nRowNumVtx;								// �s���_��
	int m_nColumnNumVtx;							// �񒸓_��
	int m_nNumVtxMax;								// ���_�ő吔
	int m_nNumIdx;									// �C���f�b�N�X�ő吔
	int m_nNumPrim;									// �v���~�e�B�u�ő吔
	D3DXMATRIX *m_pParentMtx;						// �e�̃}�g���N�X

	void SetVertex(void);							// ���_�ݒ�

	virtual void SetRenderStateBegin(void);			// �����_���[�X�e�[�g�ݒ�J�n
	virtual void SetRenderStateEnd(void);			// �����_���[�X�e�[�g�ݒ�I��
public:
	CMeshCylinder(DRAWORDER DrawOrder = DRAWORDER_EFFECT, OBJTYPE ObjType = OBJTYPE_EFFECT);
	~CMeshCylinder();

	void Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CMeshCylinder *Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix);

	virtual void SetWorldMatrix(void);				// ���[���h�}�g���b�N�X�ݒ�
};

