/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FmeshCylinder.cpp
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
#include "scene3D.h"
#include "meshCylinder.h"
#include "debugProc.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float MAGNI = 0.005f;			// �����ɂ��e�̔{��
const float RADIUS_ATTEN = 0.05f;	// ���a�̊��������W��
const float HEIGHT = 5000.0f;		// ����
const float HEIGHT_ATTEN = 0.1f;	// �����̊��������W��
const float ROT_ATTEN = 0.05f;		// ��]�̊��������W��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCMeshCylinder::CMeshCylinder(DRAWORDER DrawOrder, OBJTYPE ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CMeshCylinder::CMeshCylinder(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene3D(DrawOrder, ObjType)
{
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_Scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	m_fRadius = 0.0f;
	m_fRadiusN = 0.0f;
	m_fHeight = 0.0f;
	m_fHeightN = 0.0f;
	m_nRowBlock = 0;
	m_nColumnBlock = 0;
	m_nRowNumVtx = 0;
	m_nColumnNumVtx = 0;
	m_nNumVtxMax = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;
	m_pParentMtx = NULL;
}

/*******************************************************************************
* �֐����FCMeshCylinder::~CMeshCylinder()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CMeshCylinder::~CMeshCylinder()
{
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::Init(Vector3 pos, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CMeshCylinder::Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
{
	m_Pos = pos;
	m_Rot = rot;
	m_pParentMtx = matrix;
	m_nRowBlock = 1;
	m_nColumnBlock = 16;
	m_nRowNumVtx = m_nRowBlock + 1;
	m_nColumnNumVtx = m_nColumnBlock + 1;
	m_fRadiusN = radius;
	m_fHeightN = HEIGHT;
	m_nNumVtxMax = m_nRowNumVtx * m_nColumnNumVtx;

	m_nNumIdx = m_nColumnNumVtx * m_nRowBlock * 2 + (m_nRowBlock - 1) * 2;
	m_nNumPrim = (m_nColumnBlock * m_nRowBlock * 2) + (m_nRowBlock - 1) * 4;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_LAZER_000);

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;

	// ���_�o�b�t�@�̊m��
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * m_nNumVtxMax,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL)))
	{
		return;
	}

	// �C���f�b�N�X�o�b�t�@�̊m��
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,		// �m�ۂ���o�b�t�@�T�C�Y
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	int nIdxCnt = 0;
	for (int nCntRow = 0; nCntRow < m_nRowNumVtx; nCntRow++)
	{
		for (int nCntColumn = m_nColumnNumVtx - 1; nCntColumn >= 0; nCntColumn--, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn),
											0.0f,
											cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = col;

			// �e�N�X�`���u���b�N�\��
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// �e�N�X�`���S�ʕ\��
			pVtx[nIdxCnt].tex.x = (1.0f / m_nColumnBlock) * nCntColumn;
			pVtx[nIdxCnt].tex.y = (1.0f / m_nRowBlock) * (m_nRowNumVtx - 1 - nCntRow);
		}
	}

	m_pVtxBuff->Unlock();


	// �C���f�b�N�X���ݒ�
	WORD *pIdx;

	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// �ϐ���`
	int x, y, cnt;

	for (y = 0, cnt = 0; y < m_nRowBlock; y++)
	{
		for (x = 0; x < m_nColumnNumVtx; x++, cnt++, pIdx += 2)
		{
			pIdx[0] = x + (y + 1) * m_nColumnNumVtx;
			pIdx[1] = x + (y + 0) * m_nColumnNumVtx;
			if (x == m_nColumnNumVtx - 1 && cnt * 2 < m_nNumIdx - 2)
			{
				cnt++;
				pIdx += 2;
				pIdx[0] = x + (y + 1) * m_nColumnNumVtx;
				pIdx[1] = (y + 1 + 1) * m_nColumnNumVtx;
			}
		}
	}

	m_pIdxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::Uninit(void)
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CMeshCylinder::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_pVtxBuff);
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::Update(void)
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CMeshCylinder::Update(void)
{
	m_fRadius += (m_fRadiusN - m_fRadius) * RADIUS_ATTEN;
	m_fHeight += (m_fHeightN - m_fHeight) * HEIGHT_ATTEN;
	m_Rot.x += (-D3DX_PI * 0.55f - m_Rot.x) * ROT_ATTEN;
	//m_Rot.x += 0.1f;
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::Draw(void)
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CMeshCylinder::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �����_�[�X�e�[�g�̐ݒ�
	SetRenderStateBegin();

	// ���[���h�}�g���b�N�X��ݒ�
	SetWorldMatrix();

	// ���[���h�}�g���b�N�X���o�C���h
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, *m_pTexture);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);
	// �f�[�^�X�g���[���Ƀo�C���h����
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));
	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices(m_pIdxBuff);
	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,		// �v���~�e�B�u�̎��
		0,
		0,
		m_nNumVtxMax,				// ���_��
		0,
		m_nNumPrim);				// �`�悷��v���~�e�B�u��

	// �����_�[�X�e�[�g�̐ݒ�
	SetRenderStateEnd();
}

/*******************************************************************************
* �֐����FCMeshCylinder *CMeshCylinder::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CMeshCylinder *CMeshCylinder::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
{
	CMeshCylinder *cylinder;
	cylinder = new CMeshCylinder;
	cylinder->Init(pos, rot, col, radius, matrix);
	return cylinder;
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::SetVertex(void)
*
* ����	�F
* �߂�l�F
* ����	�F���_�ݒ菈��
*******************************************************************************/
void CMeshCylinder::SetVertex(void)
{
	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;
	// �V�����_�[����
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++)
	{
		for (int nCntColumn = m_nColumnNumVtx - 1; nCntColumn >= 0; nCntColumn--, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
		}
	}

	m_pVtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::SetRenderStateBegin( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�J�n����
*******************************************************************************/
void CMeshCylinder::SetRenderStateBegin(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �t�H�O��OFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// �J�����O�Ȃ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// Z�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
	// ���Z�����ɂ��A���t�@�u�����h�̃����_�[�X�e�[�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::SetRenderStateEnd( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�I������
*******************************************************************************/
void CMeshCylinder::SetRenderStateEnd(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���C�g��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �t�H�O��ON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// �J�����O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// �A���t�@�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	// Z�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// ���̃A���t�@�u�����h�̐ݒ�ɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::SetWorldMatrix( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[���h�}�g���b�N�X�ݒ菈��
*******************************************************************************/
void CMeshCylinder::SetWorldMatrix(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl,
		m_Scl.x,
		m_Scl.y,
		m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_Rot.y,
		m_Rot.x,
		m_Rot.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans,
		m_Pos.x,
		m_Pos.y,
		m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);

	if (m_pParentMtx)
	{
		// �e�̃}�g���N�X�Ɗ|�����킹��
		D3DXMatrixMultiply(&m_MtxWorld,
			&m_MtxWorld,
			m_pParentMtx);
	}
}