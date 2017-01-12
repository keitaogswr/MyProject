/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fmodel.cpp
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "motionManager.h"
#include "model.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define SWITCH_FLAME	( 5 )			// �؂�ւ��t���[��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCModel::CModel()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CModel::CModel()
{
	m_pMesh = 0;
	m_pBuffMat = 0;
	m_pNumMat = 0;

	m_Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_Rot = Vector3(0.0f, 0.0f, 0.0f);

	m_PosOld = Vector3(0.0f, 0.0f, 0.0f);
	m_RotOld = Vector3(0.0f, 0.0f, 0.0f);

	m_PosOffset = Vector3(0.0f, 0.0f, 0.0f);
	m_RotOffset = Vector3(0.0f, 0.0f, 0.0f);

	m_PosNext = Vector3(0.0f, 0.0f, 0.0f);
	m_RotNext = Vector3(0.0f, 0.0f, 0.0f);

	m_PosTemp = Vector3(0.0f, 0.0f, 0.0f);
	m_RotTemp = Vector3(0.0f, 0.0f, 0.0f);

	m_nIdx = 0;
	m_nParent = -1;
	m_pParentMtx = NULL;

	m_pMotionManager = NULL;

	m_bChangeColFlg = false;
}

/*******************************************************************************
* �֐����FCModel::~CModel()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CModel::~CModel()
{
}

/*******************************************************************************
* �֐����Fvoid CModel::Init( CMotionManager *pMotionManager )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CModel::Init(CMotionManager *pMotionManager)
{
	//BYTE *pVtxBuff;		// ���_���ւ̃|�C���^

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	m_pMotionManager = pMotionManager;

	DYNAMICMODEL_TYPE type = m_pMotionManager->GetModelType();
	DYNAMICMODEL_INFO *info = m_pMotionManager->GetDynamicModelInfo(type);

	m_pMesh = info->pParts[m_nIdx]->pMesh;
	m_pBuffMat = info->pParts[m_nIdx]->pBuffMat;
	m_pNumMat = info->pParts[m_nIdx]->pNumMat;
	// �I�t�Z�b�g�̐ݒ�
	m_PosOffset = m_Pos = info->pParts[m_nIdx]->posOff;
	m_RotOffset = m_Rot = info->pParts[m_nIdx]->rotOff;

	// �f�t�H���g�̃��[�V����(0�Ԗ�)��ݒ�
	MOTION_INFO **pMotion = m_pMotionManager->GetMotion();	// �f�t�H���g���[�V�����|�C���^
	KEY_INFO *keyInfo = pMotion[0]->pKeyInfo[0];			// �f�t�H���g�L�[�t���[��
	KEY *key = pMotion[0]->pKeyInfo[0]->pKey[m_nIdx];		// �f�t�H���g�L�[

	m_Pos += key->pos;
	m_Rot += key->rot;
	CManager::CheckRot(&m_Rot);

	// �e�ԍ�
	m_nParent = info->pParts[m_nIdx]->nParent;

	if (m_nParent >= 0)
	{// �e������Ƃ�
		m_pParentMtx = m_pMotionManager->GetModel(m_nParent)->GetMatrix();
	}
	else
	{// �e�����Ȃ��Ƃ�
		m_pParentMtx = m_pMotionManager->GetMatrix();
	}
}

/*******************************************************************************
* �֐����Fvoid CModel::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CModel::Uninit(void)
{
}

/*******************************************************************************
* �֐����Fvoid CModel::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CModel::Update(void)
{
	Vector3 fDiffNow, fDiffOld;		// �~�����p�ϐ�
	Vector3 pos, rot;

	// �ϐ����
	int nMotionId = m_pMotionManager->GetMotionId();								// ���݂̃��[�V�����ԍ�
	int nKeyInfoId = m_pMotionManager->GetKeyInfoId();								// ���݂̃L�[�t���[���ԍ�
	int nFrame = m_pMotionManager->GetFrame();										// ���݂̃t���[����
	// �\���̑��
	MOTION_INFO **pMotion = m_pMotionManager->GetMotion();							// ���[�V�����|�C���^
	KEY_INFO *keyInfo = pMotion[nMotionId]->pKeyInfo[nKeyInfoId];					// ���݂̃L�[�t���[��
	KEY *key = pMotion[nMotionId]->pKeyInfo[nKeyInfoId]->pKey[m_nIdx];				// ���݂̃L�[
	KEY *keyNext = pMotion[nMotionId]->pKeyInfo[(nKeyInfoId + 1) % pMotion[nMotionId]->nNumKey]->pKey[m_nIdx];				// ���̃L�[

	// �u�����h�p���[�V�����ϐ�
	int nOldMotionId = m_pMotionManager->GetOldMotionId();							// ���[�V�����ԍ�
	int nOldKeyInfoId = m_pMotionManager->GetOldKeyInfoId();						// �L�[�t���[���ԍ�
	int nOldFrame = m_pMotionManager->GetOldFrame();								// �t���[����
	KEY_INFO *oldInfo = pMotion[nOldMotionId]->pKeyInfo[nOldKeyInfoId];				// �L�[�t���[��
	KEY *oldKey = pMotion[nOldMotionId]->pKeyInfo[nOldKeyInfoId]->pKey[m_nIdx];		// �L�[
	KEY *oldKeyNext = pMotion[nOldMotionId]->pKeyInfo[(nOldKeyInfoId + 1) % pMotion[nOldMotionId]->nNumKey]->pKey[m_nIdx];	// ���̃L�[

	// ���[�V������Ԕ䗦
	float rate;
	rate = (float)nFrame / (float)keyInfo->nFlame;
	// ���[�V�����u�����h�䗦
	float switchRate;
	switchRate = (float)nFrame / SWITCH_FLAME;

	if (pMotion[nMotionId]->nLoop == 0 && nKeyInfoId == pMotion[nMotionId]->nNumKey - 1)
	{// ���[�v�t���O��0���A�Ō�̃L�[�t���[���ɒB���Ă�����
		return;
	}

	// ���L�[�̎n�_�ƑO�L�[�̎n�_�̍������t���[���Ŋ���A���ݒl�ɉ��Z
	// �ʒu
	if (m_pMotionManager->GetSwitch() == false)
	{// �ʏ탂�[�V������
		pos = (keyNext->pos - key->pos) / (float)keyInfo->nFlame;
		//pos = key->pos * (1.0f - rate) + keyNext->pos * rate;

		m_Pos += pos;
		//m_Pos = m_PosOffset + pos;
	}
	else
	{// ���[�V�����؂�ւ���
		//pos = (keyNext->pos - key->pos) / (float)keyInfo->nFlame * (1.0f - switchRate) + (oldKeyNext->pos - oldKey->pos) / (float)oldInfo->nFlame * switchRate;
		if (pMotion[nOldMotionId]->nLoop == 1)
		{// ���[�v�t���O��1�̂Ƃ��ߋ����[�V�������X�V
			m_PosOld += (oldKeyNext->pos - oldKey->pos) / (float)oldInfo->nFlame;
		}
		m_PosTemp += (keyNext->pos - key->pos) / (float)keyInfo->nFlame;

		m_Pos = m_PosOld * (1.0f - switchRate) + m_PosTemp * switchRate;
	}

	// ��]�l
	if (m_pMotionManager->GetSwitch() == false)
	{// �ʏ탂�[�V������
		fDiffNow = keyNext->rot - key->rot;
		CManager::CheckRot(&fDiffNow);				// �~�����`�F�b�N
		rot = fDiffNow / (float)keyInfo->nFlame;
		//rot = key->rot * (1.0f - rate) + keyNext->rot * rate;
		CManager::CheckRot(&rot);					// �~�����`�F�b�N

		m_Rot += rot;
		//m_Rot = m_RotOffset + rot;
		CManager::CheckRot(&m_Rot);					// �~�����`�F�b�N
	}
	else
	{// ���[�V�����؂�ւ���
		if (pMotion[nOldMotionId]->nLoop == 1)
		{// ���[�v�t���O��1�̂Ƃ��ߋ����[�V�������X�V
			fDiffOld = oldKeyNext->rot - oldKey->rot;
			CManager::CheckRot(&fDiffOld);				// �~�����`�F�b�N
			fDiffOld /= (float)oldInfo->nFlame;
			CManager::CheckRot(&fDiffOld);				// �~�����`�F�b�N
			m_RotOld += fDiffOld;
			CManager::CheckRot(&m_RotOld);				// �~�����`�F�b�N
		}

		fDiffNow = keyNext->rot - key->rot;
		CManager::CheckRot(&fDiffNow);				// �~�����`�F�b�N
		fDiffNow /= (float)keyInfo->nFlame;
		CManager::CheckRot(&fDiffNow);				// �~�����`�F�b�N
		m_RotTemp += fDiffNow;
		CManager::CheckRot(&m_RotTemp);				// �~�����`�F�b�N

		m_Rot = m_RotOld * (1.0f - switchRate) + m_RotTemp * switchRate;
		CManager::CheckRot(&m_Rot);					// �~�����`�F�b�N
	}
}

/*******************************************************************************
* �֐����Fvoid CModel::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CModel::Draw(void)
{
	D3DXMATRIX		mtxRot, mtxTrans;				// �����A�|�W�V����
	D3DXMATERIAL	*pMat;							// �}�e���A���|�C���^
	D3DMATERIAL9	matDef;							// �f�t�H���g�̃}�e���A��

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	pDevice->GetMaterial(&matDef);							// ���݂̃}�e���A�����擾
	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);
	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	// �e�̃}�g���N�X�Ɗ|�����킹��
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		m_pParentMtx);

	// ���[���h�}�g���b�N�X���o�C���h
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	pMat = (D3DXMATERIAL*)(*m_pBuffMat)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)(*m_pNumMat); nCntMat++)
	{
		if (m_bChangeColFlg)
		{
			pMat[nCntMat].MatD3D.Diffuse = m_DiffuseCol;
		}
		// �}�e���A���̐ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);

		// ���b�V���̕`��
		(*m_pMesh)->DrawSubset(nCntMat);
	}
	// �}�e���A�������ɖ߂�
	pDevice->SetMaterial(&matDef);
}

/*******************************************************************************
* �֐����Fvoid CModel::Reset( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����؂�ւ�����
*******************************************************************************/
void CModel::Switch(void)
{
	// ���݂̒l���i�[
	m_PosOld = m_Pos;
	m_RotOld = m_Rot;

	// ���݂̒l�Ɏ����[�V�����̒l��ݒ�
	int nMotionId = m_pMotionManager->GetMotionId();				// ���݂̃��[�V�����ԍ�
	MOTION_INFO **pMotion = m_pMotionManager->GetMotion();			// ���[�V�����|�C���^
	KEY_INFO *keyInfo = pMotion[nMotionId]->pKeyInfo[0];			// �L�[�t���[��
	KEY *key = pMotion[nMotionId]->pKeyInfo[0]->pKey[m_nIdx];		// �L�[
	// �u�����h�p�Ɋi�[
	m_PosTemp = m_PosOffset + key->pos;
	m_RotTemp = key->rot;
	CManager::CheckRot(&m_Rot);	// �~�����`�F�b�N
}

/*******************************************************************************
* �֐����Fvoid CModel::Reset( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����؂�ւ�����
*******************************************************************************/
void CModel::SetMatrix(void)
{
	D3DXMATRIX		mtxRot, mtxTrans;				// �����A�|�W�V����

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Rot.y, m_Rot.x, m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);
	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	// �e�̃}�g���N�X�Ɗ|�����킹��
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		m_pParentMtx);
}