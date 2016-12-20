/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene3D.cpp
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
#include "mode.h"
#include "game.h"
#include "load.h"
#include "renderer.h"
#include "camera.h"
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "billboard.h"
#include "treeObject.h"
#include "input.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define TEXTFILENAME	( "data\\EDITDATA\\BillboardData.txt" )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
CTreeObject::TREEOBJ_PARAM CTreeObject::treeObjParam[TREEOBJ_TYPE_MAX] =
{//		�e�N�X�`����				��		����
	{ TEXTURE_TYPE_TREE_000, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_001, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_002, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_003, 150.0f, 150.0f },
	{ TEXTURE_TYPE_TREE_004, 150.0f, 150.0f },
	{ TEXTURE_TYPE_WEED_000, 50.0f, 50.0f },
	{ TEXTURE_TYPE_WEED_001, 50.0f, 50.0f },
	{ TEXTURE_TYPE_WEED_002, 50.0f, 50.0f }
};
int CTreeObject::m_nTexNum = 0;

/*******************************************************************************
* �֐����FCTreeObject::CTreeObject()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CTreeObject::CTreeObject(DRAWORDER DrawOrder, OBJTYPE ObjType) :CBillboard(DrawOrder, ObjType)
{
	m_nTreeType = 0;
	billboardType = TREEOBJ_TYPE_MAX;
}

/*******************************************************************************
* �֐����FCTreeObject::~CTreeObject()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CTreeObject::~CTreeObject()
{
}

/*******************************************************************************
* �֐����Fvoid CTreeObject::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CTreeObject::Init(Vector3 pos, int num)
{
	m_Pos = pos;
	m_nTreeType = num;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̐���
	m_pTexture = CTexture::GetTexture(treeObjParam[m_nTreeType].nIdx);

	// ���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-treeObjParam[m_nTreeType].width * 0.5f,
								treeObjParam[m_nTreeType].height,
								0.0f);
	pVtx[1].pos = D3DXVECTOR3(treeObjParam[m_nTreeType].width * 0.5f,
								treeObjParam[m_nTreeType].height,
								0.0f);
	pVtx[2].pos = D3DXVECTOR3(-treeObjParam[m_nTreeType].width * 0.5f,
								0.0f,
								0.0f);
	pVtx[3].pos = D3DXVECTOR3(treeObjParam[m_nTreeType].width * 0.5f,
								0.0f,
								0.0f);

	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CTreeObject::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CTreeObject::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
}

/*******************************************************************************
* �֐����Fvoid CTreeObject::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CTreeObject::Update(void)
{
}

/*******************************************************************************
* �֐����Fvoid CTreeObject::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CTreeObject::Draw(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();

	// �e��ݒ� /////
	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// Z�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �r���[�}�g���b�N�X�̋t�s������߂�
	/*D3DXMatrixInverse( &m_MtxWorld,
	NULL,
	&mtxView);*/

	// �}�g���N�X�̈ړ��Ɋւ���ϐ��N���A
	m_MtxWorld._41 = 0.0f;
	m_MtxWorld._42 = 0.0f;
	m_MtxWorld._43 = 0.0f;

	D3DXMATRIX matBillboard;
	D3DXVECTOR3 vDir = camera->GetVecRV();

	if (vDir.x >= 0.0f)
	{
		D3DXMatrixRotationY(&matBillboard, -atanf(vDir.z / vDir.x) + D3DX_PI / 2.0f);
	}
	else
	{
		D3DXMatrixRotationY(&matBillboard, -atanf(vDir.z / vDir.x) - D3DX_PI / 2.0f);
	}
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&matBillboard);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
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

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, *m_pTexture);

	//�|���S���̕`��
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
		0,							//�ŏ��̒��_�̃C���f�b�N�X
		2);							//�`�悷��v���~�e�B�u��

	// �ݒ�����ɖ߂� /////
	// ���C�g��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �A���t�@�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// Z�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

/*******************************************************************************
* �֐����FCTreeObject *CTreeObject::Create( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CTreeObject *CTreeObject::Create(Vector3 pos, int num)
{
	CLoad *load = (CLoad*)CManager::GetMode();
	CGame *game = (CGame*)load->GetMode();
	//CGame *game = (CGame*)CManager::GetMode();
	CMeshField *meshField = game->GetMeshField();
	if (meshField == NULL)
	{
		return NULL;
	}
	pos.y = meshField->GetHeight(pos);

	CTreeObject *object;
	object = new CTreeObject;
	object->Init(pos, num);
	return object;
}

/*******************************************************************************
* �֐����Fvoid CTreeObject::Set( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
void CTreeObject::Set(Vector3 pos, int num)
{
	CScene *scene = NULL;
	if (CManager::GetOperateMode() == CManager::OPMODE_PLAYER)
	{
		if (CInput::GetMouseTrigger(DIM_LEFT))
		{
			scene = Create(pos, num);
		}
		if (CInput::GetMouseTrigger(DIM_RIGHT))
		{
			m_nTexNum++;
			if (m_nTexNum >= TREEOBJ_TYPE_MAX)
			{
				m_nTexNum = 0;
			}
		}
	}
}

/*******************************************************************************
* �֐����Fint CTreeObject::SaveFile( void )
*
* ����	�F
* �߂�l�F
* ����	�F�t�@�C���Z�[�u����
*******************************************************************************/
int CTreeObject::SaveFile(void)
{
	FILE *fp;

	// �t�@�C���ǂݍ��݊J�n
	if ((fp = fopen(TEXTFILENAME, "w")) == NULL)
	{
		return EXIT_FAILURE;
	}

	int nNumBill = 0;
	CScene *scene = GetList(DRAWORDER_3DLATE);

	while (scene != NULL)
	{
		if (scene->GetObjType() == OBJTYPE_BILLBOARD)
		{
			nNumBill++;
			CTreeObject *tree;
			tree = (CTreeObject*)scene;
			fprintf(fp, "POSXZ %f %f %d\n", tree->GetPosition().x, tree->GetPosition().z, tree->GetTreeType());
		}
		scene = scene->m_Next;
	}

	// �t�@�C���ǂݍ��ݏI��
	fclose(fp);

	return 0;
}

/*******************************************************************************
* �֐����Fint CTreeObject::LoadFile( void )
*
* ����	�F
* �߂�l�F
* ����	�F�t�@�C�����[�h����
*******************************************************************************/
int CTreeObject::LoadFile(void)
{
	FILE *fp;

	// �t�@�C���ǂݍ��݊J�n
	if ((fp = fopen(TEXTFILENAME, "r")) == NULL)
	{
		return EXIT_FAILURE;
	}

	int nNumBill = 0;
	CTreeObject *bill = NULL;
	int num = 0;
	Vector3 vector;
	char str[256];

	while (fscanf(fp, "%s", str) != EOF)
	{
		if (strcmp(str, "POSXZ") == 0)
		{
			fscanf(fp, "%f %f %d", &vector.x, &vector.z, &num);
			bill = Create(vector, num);
		}
	}


	// �t�@�C���ǂݍ��ݏI��
	fclose(fp);

	return 0;
}