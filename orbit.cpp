/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Forbit.cpp
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
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "input.h"
#include "texture.h"
#include "orbit.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCOrbit::COrbit()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
COrbit::COrbit(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene3D(DrawOrder, ObjType)
{
	for (int i = 0; i < ORBIT_VERTEX; i++)
	{
		m_OldPos[i] = Vector3(0.0f, 0.0f, 0.0f);
	}
	m_ParentPos = Vector3(0.0f, 0.0f, 0.0f);
	m_HeadPos = Vector3(0.0f, 0.0f, 0.0f);
}

/*******************************************************************************
* �֐����FCOrbit::~COrbit()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
COrbit::~COrbit()
{
}

/*******************************************************************************
* �֐����Fvoid COrbit::Init(void)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void COrbit::Init(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���_�o�b�t�@�̊m��
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * ORBIT_VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid COrbit::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void COrbit::Uninit(void)
{
	// �������
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid COrbit::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void COrbit::Update(void)
{
	for (int i = 0; i < ORBIT_VERTEX * 0.5f - 1; i++)
	{// �ߋ��̃f�[�^��ۑ�
		m_OldPos[i * 2] = m_OldPos[(i + 1) * 2];
		m_OldPos[i * 2 + 1] = m_OldPos[(i + 1) * 2 + 1];
	}

	// ���W�ϊ�
	D3DXVec3TransformCoord(&m_ParentPos,
		&m_ParentPos,
		&m_ParentMtx);
	m_OldPos[ORBIT_VERTEX - 1] = m_ParentPos;
	// ���W�ϊ�
	D3DXVec3TransformCoord(&m_HeadPos,
		&m_HeadPos,
		&m_ParentMtx);
	m_OldPos[ORBIT_VERTEX - 2] = m_HeadPos;

	SetVertex();
}
/*******************************************************************************
* �֐����Fvoid COrbit::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void COrbit::Draw(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();


	// �e��ݒ� /////
	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �t�H�O��OFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// Z�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	// ���Z�����ɂ��A���t�@�u�����h�̃����_�[�X�e�[�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	// �J�����O�ݒ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);
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
	pDevice->SetTexture(0, NULL);

	//�|���S���̕`��
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
		0,							//�ŏ��̒��_�̃C���f�b�N�X
		ORBIT_VERTEX - 2);			//�`�悷��v���~�e�B�u��

	// �ݒ�����ɖ߂� /////
	// ���C�g��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �t�H�O��ON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// �A���t�@�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// Z�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// ���̃A���t�@�u�����h�̐ݒ�ɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/*******************************************************************************
* �֐����FCOrbit *COrbit::Create(void)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
COrbit *COrbit::Create(void)
{
	COrbit *meshField;
	meshField = new COrbit;
	meshField->Init();
	return meshField;
}

/*******************************************************************************
* �֐����Fvoid COrbit::SetVertex( void )
*
* ����	�F
* �߂�l�F
* ����	�F���_�ݒ菈��
*******************************************************************************/
void COrbit::SetVertex(void)
{
	// ���_���ݒ�
	VERTEX_3D *pVtx;
	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < ORBIT_VERTEX; i++)
	{
		pVtx[i].pos = m_OldPos[i];
		pVtx[i].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[i].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f / ORBIT_VERTEX * i);
	}

	m_VtxBuff->Unlock();
}