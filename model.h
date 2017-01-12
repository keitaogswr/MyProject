/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fmodel.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _MODEL_H_
#define _MODEL_H_

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
class CMotionManager;
class CScene;
class CSceneX;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
// ���f�����
class CModel
{
private:
	LPD3DXMESH		*m_pMesh;			// ���b�V�����
	LPD3DXBUFFER	*m_pBuffMat;		// �}�e���A�����
	DWORD			*m_pNumMat;			// �}�e���A����

	Vector3			m_Pos;				// ���f���̌��݂̈ʒu
	Vector3			m_Rot;				// ���f���̌��݂̉�]�p

	Vector3			m_PosOld;			// ���f���̑O���[�V�����̈ʒu
	Vector3			m_RotOld;			// ���f���̑O���[�V�����̉�]�p

	Vector3			m_PosOffset;		// ���f���̈ʒu�I�t�Z�b�g
	Vector3			m_RotOffset;		// ���f���̉�]�p�I�t�Z�b�g

	Vector3			m_PosTemp;			// ���[�V�����̈ʒu�ۑ�
	Vector3			m_RotTemp;			// ���[�V�����̉�]�p�ۑ�

	Vector3			m_PosNext;			// �����[�V�����̈ʒu
	Vector3			m_RotNext;			// �����[�V�����̉�]�p

	D3DXMATRIX		m_MtxWorld;			// ���[���h�}�g���b�N�X
	int				m_nIdx;				// ���f��ID
	int				m_nParent;			// �e��ID
	D3DXMATRIX		*m_pParentMtx;		// �e�̃}�g���N�X�|�C���^

	CMotionManager	*m_pMotionManager;	// ���[�V�����}�l�[�W���[�̃|�C���^
	D3DXCOLOR		m_DiffuseCol;		// �f�B�t�[�Y�J���[
	bool			m_bChangeColFlg;	// �F�ύX�t���O
public:
	CModel();
	~CModel();

	void Init(CMotionManager *pMotionManager);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Switch(void);

	void SetMatrix();

	void SetIndex(int nIdx) { m_nIdx = nIdx; }									// �C���f�b�N�X�Z�b�g
	void SetParent(int parent) { m_nParent = parent; }							// �e�C���f�b�N�X�Z�b�g
	D3DXMATRIX *GetMatrix(void) { return &m_MtxWorld; }							// ���[���h�}�g���N�X�̎擾
	void SetMatrix(D3DXMATRIX *pMatrix) { m_pParentMtx = pMatrix; }				// �e�̃��[���h�}�g���N�X�̃Z�b�g
	void SetPosition(Vector3 pos) { m_Pos = pos; }								// �ʒu�̃Z�b�g
	void SetRotation(Vector3 rot) { m_Rot = rot; }								// ��]�p�̃Z�b�g
	Vector3 GetPosition(void) { return m_Pos; }									// ���݂̈ʒu�̎擾
	Vector3 GetRotation(void) { return m_Rot; }									// ���݂̉�]�p�̎擾
	Vector3 GetPosOffset(void) { return m_PosOffset; }							// �ʒu�I�t�Z�b�g�̃Z�b�g
	Vector3 GetRotOffset(void) { return m_RotOffset; }							// ��]�p�I�t�Z�b�g�̃Z�b�g
	void SetPosNext(Vector3 pos) { m_PosNext = pos; }							// ���̃L�[�̈ʒu�̃Z�b�g
	void SetRotNext(Vector3 rot) { m_RotNext = rot; }							// ���̃L�[�̉�]�p�̃Z�b�g

	void SetDiffuseCol(D3DXCOLOR col) { m_DiffuseCol = col; m_bChangeColFlg = true; }
	void SetChangeColFlg(bool flg) { m_bChangeColFlg = flg; }
};

#endif