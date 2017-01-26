/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FmotionManager.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _MOTIONMANAGER_H_
#define _MOTIONMANAGER_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "model.h"
/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define DATA_MAX		( 10000 )		// �e�L�X�g�f�[�^�̍s��

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CModel;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
// ���[�h���[�h
typedef enum
{
	MOTIONLOAD_CHAR,			// �L�����N�^�[�ǂݍ���
	MOTIONLOAD_MOTIONCOUNT,		// ���[�V�������J�E���g
	MOTIONLOAD_MOTIONDATA,		// ���[�V�����f�[�^�ǂݍ���
	MOTIONLOAD_KEYSETCOUNT,		// �L�[�t���[�����J�E���g
	MOTIONLOAD_KEYSETDATA,		// �L�[�t���[���f�[�^�ǂݍ���
	MOTIONLOAD_KEYCOUNT,		// �L�[�p�[�c���J�E���g
	MOTIONLOAD_KEYDATA,			// �L�[�p�[�c�f�[�^�ǂݍ���
	MOTIONLOAD_MAX
}MOTIONLOAD;

// �L�[�v�f
typedef struct
{
	Vector3		pos;
	Vector3		rot;
	//float			fPosX;				// X���ʒu
	//float			fPosY;				// Y���ʒu
	//float			fPosZ;				// Z���ʒu
	//float			fRotX;				// X����]�p
	//float			fRotY;				// Y����]�p
	//float			fRotZ;				// Z����]�p
}KEY;

// �L�[���
typedef struct
{
	int				nFlame;				// �t���[����
	int				nNumKeyParts;		// �L�[�p�[�c�� 
	KEY				**pKey;
}KEY_INFO;

// ���[�V�������
typedef struct
{
	int				nLoop;				// ���[�v���邩�ǂ���
	int				nNumKey;			// �L�[�Z�b�g��
	KEY_INFO		**pKeyInfo;
}MOTION_INFO;

// �A�j���[�V�������f���񋓏��
typedef enum
{
	DYNAMICMODEL_TYPE_PLAYER_00,
	DYNAMICMODEL_TYPE_PLAYER_01,
	DYNAMICMODEL_TYPE_ENEMY_00,
	DYNAMICMODEL_TYPE_ENEMY_01,
	DYNAMICMODEL_TYPE_BOSS_00,
	DYNAMICMODEL_TYPE_MAX
}DYNAMICMODEL_TYPE;
// �p�[�c���f�����
typedef struct
{
	LPD3DXMESH		*pMesh;							// ���b�V�����
	LPD3DXBUFFER	*pBuffMat;						// �}�e���A�����
	DWORD			*pNumMat;						// �}�e���A����

	int nIdx;										// �C���f�b�N�X
	int nParent;									// �e�̃C���f�b�N�X
	Vector3 posOff;									// �ʒu�̃I�t�Z�b�g
	Vector3 rotOff;									// ��]�p�̃I�t�Z�b�g
	char partsName[256];							// �p�[�c�t�@�C����
}MODEL_PARTS;
// �A�j���[�V�������f�����
typedef struct
{
	char fileName[256];								// �t�@�C����
	int nNumModel;									// ���f����
	int nNumMotion;									// ���[�V������
	MOTION_INFO		**pMotion;						// ���[�V�����|�C���^
	MODEL_PARTS		**pParts;						// �p�[�c���f���|�C���^
}DYNAMICMODEL_INFO;

// ���[�V�����}�l�[�W���N���X
class CMotionManager
{
private:
	int m_nFlameCnt;													// �����[�V�����̃t���[���J�E���^
	int m_nKeyInfoId;													// �����[�V�����̃L�[�Z�b�gID
	int m_nMotionId;													// �����[�V�����ԍ�

	int m_nOldFlameCnt;													// �O���[�V�����̃t���[���J�E���^
	int m_nOldKeyInfoId;												// �O���[�V�����̃L�[�Z�b�gID
	int m_nOldMotionId;													// �O���[�V�����ԍ�

	bool m_bSwitch;														// ���[�V�����؂�ւ��t���O

	CModel **m_pModel;													// ���f���|�C���^
	D3DXMATRIX *m_pObjMtx;												// �I�u�W�F�N�g�̃}�g���N�X�|�C���^

	bool m_bEndMotion;													// ���[�V�����I���t���O

	static DYNAMICMODEL_INFO m_DModel[DYNAMICMODEL_TYPE_MAX];			// �A�j���[�V�������f�����
	DYNAMICMODEL_TYPE m_ModelType;										// ���f���^�C�v
public:
	CMotionManager();
	~CMotionManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void Load(void);
	static void Unload(void);

	static int FileOpen(const char *fileName, int nData);

	void ResetMotion(void);
	static CMotionManager *Create(DYNAMICMODEL_TYPE type, D3DXMATRIX *pMtx);

	void SetMotion(int nId);																// ���[�V�����Z�b�g
	void SetMatrix(D3DXMATRIX *pMtx) { m_pObjMtx = pMtx; }									// ���̃}�l�[�W����ێ����Ă���I�u�W�F�N�g�̃��[���h�}�g���N�X���󂯎��
	D3DXMATRIX *GetMatrix(void) { return m_pObjMtx; }										// ���̃}�l�[�W����ێ����Ă���I�u�W�F�N�g�̃��[���h�}�g���N�X��Ԃ�
	MOTION_INFO **GetMotion(void) { return m_DModel[m_ModelType].pMotion; }					// ���[�V�����̎擾
	int GetMotionId(void) { return m_nMotionId; }											// ���[�V�����ԍ��̎擾
	int GetFrame(void) { return m_nFlameCnt; }												// ���݂̃t���[�����擾
	int GetKeyInfoId(void) { return m_nKeyInfoId; }											// ���݂̃L�[�t���[�����擾

	int GetOldMotionId(void) { return m_nOldMotionId; }										// ��O�̃��[�V�����ԍ��̎擾
	int GetOldFrame(void) { return m_nOldFlameCnt; }										// ��O�̃t���[�����擾
	int GetOldKeyInfoId(void) { return m_nOldKeyInfoId; }									// ��O�̃L�[�t���[�����擾

	bool GetSwitch(void) { return m_bSwitch; }												// �؂�ւ��t���O�̎擾
	CModel *GetModel(int nId) { return m_pModel[nId]; }										// ���f���̃|�C���^�擾
	D3DXMATRIX *GetModelMatrix(int id) { return m_pModel[id]->GetMatrix(); }				// ���f���̃}�g���N�X�̎擾

	bool EndMotion(void);																	// ���[�V�������I�����Ă��邩�ǂ���
	static DYNAMICMODEL_INFO *GetDynamicModelInfo(int nType) { return &m_DModel[nType]; }	// �A�j���[�V�������f�����̎擾
	void SetModelType(DYNAMICMODEL_TYPE nType) { m_ModelType = nType; }						// ���f���^�C�v�̃Z�b�g
	DYNAMICMODEL_TYPE GetModelType(void) { return m_ModelType; }							// ���f���^�C�v�̎擾

	void SetModelColor(int num, D3DXCOLOR color){ m_pModel[num]->SetDiffuseCol(color); }
	void SetModelColorAll(D3DXCOLOR color);
	void SetModelColFlg(int num, bool flg) { m_pModel[num]->SetChangeColFlg(flg); }
	void SetModelColFlgAll(bool flg);
};

#endif