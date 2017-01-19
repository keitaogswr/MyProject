/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FmotionManager.cpp
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
#include "scene.h"
#include "sceneX.h"
#include "model.h"
#include "motionManager.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define SWITCH_FLAME	( 5 )			// �؂�ւ��t���[��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
DYNAMICMODEL_INFO CMotionManager::m_DModel[DYNAMICMODEL_TYPE_MAX] =
{
	{ "data\\MOTION\\motion_b_type_origin.txt" },
	{ "data\\MOTION\\motion_haitatsuoh.txt" },
	{ "data\\MOTION\\motion_enemyFly.txt" },
	{ "data\\MOTION\\motion_enemySpeed.txt" },
	{ "data\\MOTION\\motion_enemyBoss_TypeFortress.txt" }
};

/*******************************************************************************
* �֐����FCMotionManager::CMotionManager()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CMotionManager::CMotionManager()
{
	m_nFlameCnt = 0;
	m_nKeyInfoId = 0;
	m_bSwitch = false;
	m_nMotionId = -1;

	m_nOldFlameCnt = 0;
	m_nOldKeyInfoId = 0;
	m_nOldMotionId = -1;

	m_bEndMotion = false;

	m_ModelType = DYNAMICMODEL_TYPE_MAX;
}

/*******************************************************************************
* �֐����FCMotionManager::~CMotionManager()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CMotionManager::~CMotionManager()
{
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::Init( Vector3 pos, Vector3 rot )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
HRESULT CMotionManager::Init()
{
	// ���f���̊m�ہA������
	m_pModel = new CModel*[m_DModel[m_ModelType].nNumModel];
	for (int nCnt = 0; nCnt < m_DModel[m_ModelType].nNumModel; nCnt++)
	{
		m_pModel[nCnt] = new CModel;
	}
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		m_pModel[i]->SetIndex(i);		// �C���f�b�N�X�̐ݒ�
		m_pModel[i]->Init(this);
	}

	return S_OK;
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CMotionManager::Uninit(void)
{
	// ���
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
 		m_pModel[i]->Uninit();
		SAFE_DELETE(m_pModel[i]);
	}
	SAFE_DELETE_ARRAY(m_pModel);
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CMotionManager::Update(void)
{
	MOTION_INFO **pMotion, **pOldMotion;
	pMotion = &m_DModel[m_ModelType].pMotion[m_nMotionId];
	pOldMotion = &m_DModel[m_ModelType].pMotion[m_nOldMotionId];

	if (m_bEndMotion == true)
	{// ���[�V�������I�����Ă����烂�[�V�������Ȃ�
		return;
	}

	// �e�p�[�c�̍X�V
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		m_pModel[i]->Update();
	}
	m_nFlameCnt++;			// �t���[���J�E���g
	if (m_bSwitch == true && (*pOldMotion)->nLoop == 1)
	{
		m_nOldFlameCnt++;
	}

	if (SWITCH_FLAME < m_nFlameCnt && m_bSwitch == true)
	{// ���[�V�����؂�ւ���Ԓ��ŁA���t���[�����w�肳�ꂽ�t���[���ɒB������
	 // �e�p�[�c�̍��L�[�̎n�_����O�L�[�̎n�_���ɕۑ�
		m_bSwitch = false;		// ���[�V�����؂�ւ�OFF
	}
	// �ʏ탂�[�V�������ŁA���t���[�������[�V�����Ɏw�肳�ꂽ�t���[�����ɒB������
	else if ((*pMotion)->pKeyInfo[m_nKeyInfoId]->nFlame <= m_nFlameCnt && m_bSwitch == false)
	{
		// �e�p�[�c�̍��L�[�̎n�_����O�L�[�̎n�_���ɕۑ�
		m_nKeyInfoId++;		// ���̃L�[�Z�b�g��
		m_nFlameCnt = 0;	// �t���[���J�E���^��������
	}
	else if (m_bSwitch == true)
	{
		if ((*pMotion)->pKeyInfo[m_nKeyInfoId]->nFlame < m_nFlameCnt)
		{
			m_nKeyInfoId++;			// ���̃L�[�Z�b�g��
			m_nFlameCnt = 0;		// �t���[���J�E���^��������
		}
		if ((*pOldMotion)->pKeyInfo[m_nOldKeyInfoId]->nFlame < m_nOldFlameCnt)
		{
			m_nOldKeyInfoId++;
			m_nOldFlameCnt = 0;
		}
	}
	// �L�[�Z�b�g�����K��𒴂�����
	if (m_nKeyInfoId >= (*pMotion)->nNumKey)
	{
		m_nKeyInfoId = (*pMotion)->nNumKey - 1;
		if ((*pMotion)->nLoop == 0)
		{// ���[�v�t���O���ݒ肳��Ă��Ȃ���
			m_bEndMotion = true;	// ���[�V�����I���t���OON
		}
		else if ((*pMotion)->nLoop == 1)
		{// ���[�v�t���O���ݒ肳��Ă��鎞
			m_nKeyInfoId = 0;
			m_nFlameCnt = 0;		// �t���[���J�E���^��������
		}
	}
	if (m_nOldKeyInfoId >= (*pOldMotion)->nNumKey)
	{
		m_nOldKeyInfoId = (*pOldMotion)->nNumKey - 1;
	}
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CMotionManager::Draw(void)
{
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		m_pModel[i]->Draw();
	}
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::Load( void )
*
* ����	�F
* �߂�l�F
* ����	�F�ǂݍ��ݏ���
*******************************************************************************/
void CMotionManager::Load(void)
{
	// �A�j���[�V�������f���̃��\�[�X�ǂݍ���
	for (int i = 0; i < DYNAMICMODEL_TYPE_MAX; i++)
	{
		FileOpen(m_DModel[i].fileName, i);
	}
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::Unload( void )
*
* ����	�F
* �߂�l�F
* ����	�F�������
*******************************************************************************/
void CMotionManager::Unload(void)
{
	// �f�o�C�X�A���\�[�X�̉��
	for (int i = 0; i < DYNAMICMODEL_TYPE_MAX; i++)
	{
		for (int j = 0; j < m_DModel[i].nNumModel; j++)
		{
			SAFE_RELEASE(*m_DModel[i].pParts[j]->pMesh);
			SAFE_DELETE(m_DModel[i].pParts[j]->pMesh);
			SAFE_RELEASE(*m_DModel[i].pParts[j]->pBuffMat);
			SAFE_DELETE(m_DModel[i].pParts[j]->pBuffMat);
			SAFE_DELETE(m_DModel[i].pParts[j]->pNumMat);
			SAFE_DELETE(m_DModel[i].pParts[j]);
		}
		for (int j = 0; j < m_DModel[i].nNumMotion; j++)
		{
			for (int k = 0; k < m_DModel[i].pMotion[j]->nNumKey; k++)
			{
				for (int l = 0; l < m_DModel[i].nNumModel; l++)
				{
					SAFE_DELETE(m_DModel[i].pMotion[j]->pKeyInfo[k]->pKey[l]);
				}
				SAFE_DELETE_ARRAY(m_DModel[i].pMotion[j]->pKeyInfo[k]->pKey);
				SAFE_DELETE(m_DModel[i].pMotion[j]->pKeyInfo[k]);
			}
			SAFE_DELETE_ARRAY(m_DModel[i].pMotion[j]->pKeyInfo);
			SAFE_DELETE(m_DModel[i].pMotion[j]);
		}
		SAFE_DELETE_ARRAY(m_DModel[i].pMotion);
		SAFE_DELETE_ARRAY(m_DModel[i].pParts);
		m_DModel[i].nNumModel = 0;
		m_DModel[i].nNumMotion = 0;
	}
}

/*******************************************************************************
* �֐����Fint CMotionManager::FileOpen( char *FileName, int nData )
*
* ����	�F
* �߂�l�F
* ����	�F�e�L�X�g�t�@�C���i�[����
*******************************************************************************/
int CMotionManager::FileOpen(const char *filename, int nData)
{
	FILE *fp;
	char str[256];
	// �e�L�X�g�t�@�C���̓ǂݍ���
	if ((fp = fopen(filename, "r")) == NULL)
	{// �e�L�X�g�t�@�C���̓ǂݍ��݂̎��s
		return EXIT_FAILURE;
	}

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	int nId = 0,			// ���f��ID
		nMotionId = 0,		// ���[�V����ID
		nKeysetId = 0,		// �L�[�Z�b�gID
		nKeyId = 0;			// �L�[�t���[��ID

	while (fscanf(fp, "%s", str) != EOF)
	{
		if (strcmp(str, "NUM_MODEL") == 0)
		{// ���f���̓ǂݍ��ݐ�
			fscanf(fp, "%*[NUM_MODEL =]%d", &m_DModel[nData].nNumModel);
			m_DModel[nData].pParts = new MODEL_PARTS*[m_DModel[nData].nNumModel];
			for (int nCnt = 0; nCnt < m_DModel[nData].nNumModel; nCnt++)
			{
				m_DModel[nData].pParts[nCnt] = new MODEL_PARTS;
				m_DModel[nData].pParts[nCnt]->pMesh = new LPD3DXMESH;
				m_DModel[nData].pParts[nCnt]->pBuffMat = new LPD3DXBUFFER;
				m_DModel[nData].pParts[nCnt]->pNumMat = new DWORD;
			}
		}
		else if (strcmp(str, "MODEL_FILENAME") == 0)
		{// ���f���̃t�@�C����
			char name[256];
			ZeroMemory(&m_DModel[nData].pParts[nId]->partsName, sizeof(m_DModel[nData].pParts[nId]->partsName));
			fscanf(fp, "%*[MODEL_FILENAME =]%s", &name);
			strcpy(m_DModel[nData].pParts[nId]->partsName, name);
			// X�t�@�C���̓ǂݍ���
			D3DXLoadMeshFromX(&m_DModel[nData].pParts[nId]->partsName[0],	// �ǂݍ��݃t�@�C����
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				m_DModel[nData].pParts[nId]->pBuffMat,			// �}�e���A�����
				NULL,
				m_DModel[nData].pParts[nId]->pNumMat,			// �}�e���A���̐�
				m_DModel[nData].pParts[nId]->pMesh);			// ���b�V�����
			nId++;
		}
		// �L�����N�^�[�f�[�^ /////
		else if (strcmp(str, "CHARACTERSET") == 0)
		{
			nId = 0;
			while (strcmp(str, "END_CHARACTERSET"))
			{
				fscanf(fp, "%s", &str);
				//	�p�[�c�f�[�^ /////
				if (strcmp(str, "PARTSSET") == 0)
				{
					while (strcmp(str, "END_PARTSSET"))
					{
						fscanf(fp, "%s", &str);
						if (strcmp(str, "INDEX") == 0)
						{// ���f��ID
							fscanf(fp, "%*[INDEX =]%d", &m_DModel[nData].pParts[nId]->nIdx);
						}
						if (strcmp(str, "PARENT") == 0)
						{// �e��ID
							fscanf(fp, "%*[PARENT =]%d", &m_DModel[nData].pParts[nId]->nParent);
						}
						if (strcmp(str, "POS") == 0)
						{// �ʒu
							Vector3 *pos = &m_DModel[nData].pParts[nId]->posOff;
							fscanf(fp, "%*[POS =]%f%f%f", &pos->x, &pos->y, &pos->z);
						}
						if (strcmp(str, "ROT") == 0)
						{// ��]�l
							Vector3 *rot = &m_DModel[nData].pParts[nId]->rotOff;
							fscanf(fp, "%*[ROT =]%f%f%f", &rot->x, &rot->y, &rot->z);
						}
					}
					nId++;
				}
			}
		}
		// �L�����N�^�[�f�[�^�����܂� /////


		// ���[�V�����J�E���g /////
		else if (strcmp(str, "MOTIONSET") == 0)
		{// ���[�V������
			m_DModel[nData].nNumMotion++;
		}
	}

	// ���[�V�����̓��I�m�� /////
	m_DModel[nData].pMotion = new MOTION_INFO*[m_DModel[nData].nNumMotion];
	for (int nMotion = 0; nMotion < m_DModel[nData].nNumMotion; nMotion++)
	{
		m_DModel[nData].pMotion[nMotion] = new MOTION_INFO;
	}

	fseek(fp, SEEK_SET, 0);			// �t�@�C���|�C���^��擪�ɖ߂�

	// ���[�V�����f�[�^�̓��I�m�� /////
	nMotionId = 0;	// �ϐ�������
	while (fscanf(fp, "%s", str) != EOF)
	{
		// ���[�V�����f�[�^ /////
		if (strcmp(str, "MOTIONSET") == 0)
		{// ���[�V������
			nKeysetId = 0;	// �ϐ�������
			while (strcmp(str, "END_MOTIONSET"))
			{
				fscanf(fp, "%s", &str);
				if (strcmp(str, "NUM_KEY") == 0)
				{// �L�[�t���[����
					int nNumKey;
					fscanf(fp, "%*[NUM_KEY =]%d", &m_DModel[nData].pMotion[nMotionId]->nNumKey);
					nNumKey = m_DModel[nData].pMotion[nMotionId]->nNumKey;
					m_DModel[nData].pMotion[nMotionId]->pKeyInfo = new KEY_INFO*[nNumKey];
					for (int i = 0; i < nNumKey; i++)
					{
						m_DModel[nData].pMotion[nMotionId]->pKeyInfo[i] = new KEY_INFO;
						m_DModel[nData].pMotion[nMotionId]->pKeyInfo[i]->nNumKeyParts = m_DModel[nData].nNumModel;
					}
				}
				if (strcmp(str, "KEYSET") == 0)
				{
					while (strcmp(str, "END_KEYSET"))
					{
						fscanf(fp, "%s", &str);
					}
					int nNumKeyParts = m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->nNumKeyParts;
					m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey = new KEY*[nNumKeyParts];
					for (int i = 0; i < nNumKeyParts; i++)
					{
						m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[i] = new KEY;
					}
					nKeysetId++;
				}
			}
			nMotionId++;
		}
	}
	// ���[�V�����f�[�^�̓��I�m�ۂ����܂� /////

	fseek(fp, SEEK_SET, 0);			// �t�@�C���|�C���^��擪�ɖ߂�

	// ���[�V�����̒l�i�[ /////
	nMotionId = 0;	// �ϐ�������
	while (fscanf(fp, "%s", str) != EOF)
	{
		// ���[�V�����f�[�^ /////
		if (strcmp(str, "MOTIONSET") == 0)
		{// ���[�V������
			nKeysetId = 0;	// �ϐ�������
			while (strcmp(str, "END_MOTIONSET"))
			{
				fscanf(fp, "%s", &str);
				if (strcmp(str, "LOOP") == 0)
				{// ���[�v���邩�ǂ���
					fscanf(fp, "%*[LOOP =]%d", &m_DModel[nData].pMotion[nMotionId]->nLoop);
				}
				if (strcmp(str, "KEYSET") == 0)
				{
					nKeyId = 0;	// �ϐ�������
					while (strcmp(str, "END_KEYSET"))
					{
						fscanf(fp, "%s", &str);
						if (strcmp(str, "END_KEYSET") == 0)
						{// ���[�v�I��
							break;
						}
						while (strcmp(str, "END_KEY"))
						{
							fscanf(fp, "%s", &str);
							if (strcmp(str, "FRAME") == 0)
							{// �t���[����
								fscanf(fp, "%*[FRAME =]%d", &m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->nFlame);
							}
							if (strcmp(str, "POS") == 0)
							{// �ʒu
								fscanf(fp, "%*[POS =]%f %f %f", &m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->pos.x,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->pos.y,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->pos.z);
							}
							if (strcmp(str, "ROT") == 0)
							{// ��]�p
								fscanf(fp, "%*[ROT =]%f %f %f", &m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->rot.x,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->rot.y,
									&m_DModel[nData].pMotion[nMotionId]->pKeyInfo[nKeysetId]->pKey[nKeyId]->rot.z);
							}
						}
						nKeyId++;
					}
					nKeysetId++;
				}
			}
			nMotionId++;
		}
	}
	// ���[�V�����̒l�̊i�[�����܂� /////

	fclose(fp);

	return 0;
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::ResetMotion( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����Đݒ菈��
*******************************************************************************/
void CMotionManager::ResetMotion(void)
{
	CModel **model;

	// �\���̃|�C���^�̎擾
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		// ���f�����̍Đݒ�
		model = &m_pModel[i];
		(*model)->Switch();
	}
}

/*******************************************************************************
* �֐����FCMotionManager *CMotionManager::Create(DYNAMICMODEL_TYPE type, D3DXMATRIX *pMtx)
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����}�l�[�W���[��������
*******************************************************************************/
CMotionManager *CMotionManager::Create(DYNAMICMODEL_TYPE type, D3DXMATRIX *pMtx)
{
	CMotionManager *manager = new CMotionManager;
	manager->SetMatrix(pMtx);
	manager->SetModelType(type);
	manager->Init();
	return manager;
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::SetMotion( int nId )
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����ݒ菈��
*******************************************************************************/
void CMotionManager::SetMotion(int nId)
{
	if (m_nMotionId != nId)
	{
		m_nOldFlameCnt = m_nFlameCnt;
		m_nOldKeyInfoId = m_nKeyInfoId;
		if (m_nMotionId != -1)
		{
			m_nOldMotionId = m_nMotionId;
		}
		else
		{
			m_nOldMotionId = nId;
		}

		// ���[�V�����ԍ��̐ݒ�
		m_nMotionId = nId;
		ResetMotion();
		// �t���[���A�L�[�Z�b�g�̃��Z�b�g
		m_nFlameCnt = 0;
		m_nKeyInfoId = 0;
		// �؂�ւ��t���O�A�I���t���O�̃Z�b�g
		m_bSwitch = true;
		m_bEndMotion = false;
	}
}

/*******************************************************************************
* �֐����Fbool CMotionManager::EndMotion( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[�V�����I�����菈��
*******************************************************************************/
bool CMotionManager::EndMotion(void)
{
	return m_bEndMotion;
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::SetModelColorAll(D3DXCOLOR color)
*
* ����	�F
* �߂�l�F
* ����	�F�S���f���F�ݒ菈��
*******************************************************************************/
void CMotionManager::SetModelColorAll(D3DXCOLOR color)
{
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		SetModelColor(i, color);
	}
}

/*******************************************************************************
* �֐����Fvoid CMotionManager::SetModelColFlgAll(bool flg)
*
* ����	�F
* �߂�l�F
* ����	�F�S���f���F�ύX�t���O����
*******************************************************************************/
void CMotionManager::SetModelColFlgAll(bool flg)
{
	for (int i = 0; i < m_DModel[m_ModelType].nNumModel; i++)
	{
		SetModelColFlg(i, flg);
	}
}