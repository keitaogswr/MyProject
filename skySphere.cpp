/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FskyDome.cpp
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
#include "renderer.h"
#include "scene.h"
#include "scene3D.h"
#include "skyDome.h"
#include "input.h"
#include "camera.h"
#include "skySphere.h"
#include "texture.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define MESHDOME_ANGLE	( 80.0f )			// �h�[�������̊p�x
#define ROT_SPEED		( 0.01f )			// 

/*******************************************************************************
* �֐����FCSkySphere::CSkySphere()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CSkySphere::CSkySphere(DRAWORDER DrawOrder, OBJTYPE ObjType) :CSkyDome(DrawOrder, ObjType)
{
	ZeroMemory(&m_MeshFan, sizeof(m_MeshFan));
	ZeroMemory(&m_MeshDome, sizeof(m_MeshDome));
}

/*******************************************************************************
* �֐����FCSkySphere::~CSkySphere()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CSkySphere::~CSkySphere()
{
}

/*******************************************************************************
* �֐����Fvoid CSkySphere::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CSkySphere::Init(Vector3 pos, float rad, int row, int column)
{
	m_Pos = pos;

	// �h�[������
	m_MeshDome.nRowBlock = row;
	m_MeshDome.nColumnBlock = column;
	m_MeshDome.nRowNumVtx = m_MeshDome.nRowBlock + 1;
	m_MeshDome.nColumnNumVtx = m_MeshDome.nColumnBlock + 1;
	m_MeshDome.fRadius = rad;
	m_MeshDome.nNumVtxMax = m_MeshDome.nRowNumVtx * m_MeshDome.nColumnNumVtx;
	m_MeshDome.nNumIdx = (m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2) + (m_MeshDome.nRowBlock - 1) * 2;
	m_MeshDome.nNumPrim = (m_MeshDome.nColumnBlock * m_MeshDome.nRowBlock * 2) + (m_MeshDome.nRowBlock - 1) * 4;

	// �t�@������
	m_MeshFan[0].nNumVtxMax = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[0].nNumIdx = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[0].nNumPrim = m_MeshDome.nColumnBlock;
	m_MeshFan[1].nNumVtxMax = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[1].nNumIdx = m_MeshDome.nColumnNumVtx + 1;
	m_MeshFan[1].nNumPrim = m_MeshDome.nColumnBlock;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_SKY_000);

	// ���_�o�b�t�@�̊m�� /////
	// �h�[������
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * m_MeshDome.nNumVtxMax,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_MeshDome.pVtxBuff,
		NULL)))
	{
		return;
	}
	// �t�@������
	for (int i = 0; i < 2; i++)
	{
		if (FAILED(pDevice->CreateVertexBuffer(
			sizeof(VERTEX_3D) * m_MeshFan[i].nNumVtxMax,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&m_MeshFan[i].pVtxBuff,
			NULL)))
		{
			return;
		}

	}
	
	// �C���f�b�N�X�o�b�t�@�̊m�� /////
	// �h�[������
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_MeshDome.nNumIdx,		// �m�ۂ���o�b�t�@�T�C�Y
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_MeshDome.pIdxBuff,
		NULL);
	// �t�@������
	for (int i = 0; i < 2; i++)
	{
		pDevice->CreateIndexBuffer(sizeof(WORD) * m_MeshFan[i].nNumIdx,		// �m�ۂ���o�b�t�@�T�C�Y
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_MeshFan[i].pIdxBuff,
			NULL);
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;
	int nIdxCnt = 0;
	int rowVtxMax;
	/// �h�[������
	m_MeshDome.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntRow = (m_MeshDome.nRowNumVtx - 1) / 2; nCntRow >= 0 ; nCntRow--)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
											sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow),
											cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// �e�N�X�`���u���b�N�\��
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// �e�N�X�`���S�ʕ\��
			pVtx[nIdxCnt].tex.x = (1.0f / m_MeshDome.nColumnBlock) * nCntColumn;
			pVtx[nIdxCnt].tex.y = (1.0f / m_MeshDome.nRowBlock * 0.5f) * (m_MeshDome.nRowNumVtx - 1 - nCntRow);
		}
	}
	rowVtxMax = (m_MeshDome.nRowNumVtx - 1) / 2 + 1;
	for (int nCntRow = 1; nCntRow < rowVtxMax; nCntRow++)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) * -m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
									cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
											sinf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow),
											cosf(MESHDOME_ANGLE / (m_MeshDome.nRowBlock * 0.5f) * RAD * nCntRow) *
											cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// �e�N�X�`���u���b�N�\��
			//pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			//pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			// �e�N�X�`���S�ʕ\��
			pVtx[nIdxCnt].tex.x = (1.0f / m_MeshDome.nColumnBlock) * nCntColumn;
			pVtx[nIdxCnt].tex.y = (1.0f / m_MeshDome.nRowBlock * 0.5f) * nCntRow + 0.5f;
		}
	}

	m_MeshDome.pVtxBuff->Unlock();

	/// �t�@������
	m_MeshFan[0].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_MeshDome.fRadius;
	pVtx[0].pos.z = 0.0f;

	pVtx[0].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 0.0f;

	for (int i = 1, nIdxCnt = 0; i < m_MeshFan[0].nNumVtxMax; i++)
	{
		pVtx[i].pos.x = cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i) * m_MeshDome.fRadius;

		pVtx[i].pos.y = sinf(MESHDOME_ANGLE * RAD) * m_MeshDome.fRadius;
		pVtx[i].pos.z = cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i) * m_MeshDome.fRadius;

		pVtx[i].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i),
			sinf(MESHDOME_ANGLE * RAD),
			cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * i));
		pVtx[i].col = D3DCOLOR_RGBA(255, 255, 255, 255);

		pVtx[i].tex.x = 0.0f;
		pVtx[i].tex.y = 0.1f;
	}

	m_MeshFan[0].pVtxBuff->Unlock();

	m_MeshFan[1].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = -m_MeshDome.fRadius;
	pVtx[0].pos.z = 0.0f;

	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 1.0f;

	for (int i = 1, nIdxCnt = 0; i < m_MeshFan[1].nNumVtxMax; i++)
	{
		pVtx[i].pos.x = cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)) * m_MeshDome.fRadius;

		pVtx[i].pos.y = -sinf(MESHDOME_ANGLE * RAD) * m_MeshDome.fRadius;
		pVtx[i].pos.z = cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)) * m_MeshDome.fRadius;

		pVtx[i].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE * RAD) *
			sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)),
			-sinf(MESHDOME_ANGLE * RAD),
			cosf(MESHDOME_ANGLE * RAD) *
			cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * (i - 1)));
		pVtx[i].col = D3DCOLOR_RGBA(255, 255, 255, 255);

		pVtx[i].tex.x = 0.0f;
		pVtx[i].tex.y = 0.9f;
	}

	m_MeshFan[1].pVtxBuff->Unlock();


	// �C���f�b�N�X���ݒ� /////
	WORD *pIdx;

	m_MeshDome.pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// �ϐ���`
	int x, y, cnt;

	for (y = 0, cnt = 0; y < m_MeshDome.nRowBlock; y++)
	{
		for (x = 0; x < m_MeshDome.nColumnNumVtx; x++, cnt++, pIdx += 2)
		{
			pIdx[0] = x + (y + 1) * m_MeshDome.nColumnNumVtx;
			pIdx[1] = x + (y + 0) * m_MeshDome.nColumnNumVtx;
			if (x == m_MeshDome.nColumnNumVtx - 1 && cnt * 2 < m_MeshDome.nNumIdx - 2)
			{
				cnt++;
				pIdx += 2;
				pIdx[0] = x + (y + 0) * m_MeshDome.nColumnNumVtx;
				pIdx[1] = (y + 1 + 1) * m_MeshDome.nColumnNumVtx;
			}
		}
	}

	m_MeshDome.pIdxBuff->Unlock();
	// �t�@������
	m_MeshFan[0].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int i = 0; i < m_MeshFan[0].nNumIdx; i++)
	{
		pIdx[i] = i;
	}

	m_MeshFan[0].pIdxBuff->Unlock();

	m_MeshFan[1].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int i = 0; i < m_MeshFan[1].nNumIdx; i++)
	{
		pIdx[i] = i;
	}

	m_MeshFan[1].pIdxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CSkySphere::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CSkySphere::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_MeshDome.pVtxBuff);
	SAFE_RELEASE(m_MeshDome.pIdxBuff);
	for (int i = 0; i < 2; i++)
	{
		SAFE_RELEASE(m_MeshFan[i].pVtxBuff);
		SAFE_RELEASE(m_MeshFan[i].pIdxBuff);
	}
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CSkySphere::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CSkySphere::Update(void)
{
	// �J������Ǐ]
	CMode *game = CManager::GetMode();
	CCamera *camera = game->GetCamera();
	m_Pos.x = camera->GetPosition().x;
	m_Pos.z = camera->GetPosition().z;
	//m_Rot.y += ROT_SPEED;
}

/*******************************************************************************
* �֐����Fvoid CSkySphere::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CSkySphere::Draw(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e��ݒ� /////
	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
	// �t�H�O��OFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// �J�����O�̐ݒ�
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

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


	/// �h�[���̕`��
	// �f�[�^�X�g���[���Ƀo�C���h����
	pDevice->SetStreamSource(0, m_MeshDome.pVtxBuff, 0, sizeof(VERTEX_3D));
	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices(m_MeshDome.pIdxBuff);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, *m_pTexture);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,	// �v���~�e�B�u�̎��
		0,
		0,
		m_MeshDome.nNumVtxMax,	// ���_��
		0,
		m_MeshDome.nNumPrim);	// �`�悷��v���~�e�B�u��

	/// �t�@���̕`��
	// �J�����O�ݒ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// �f�[�^�X�g���[���Ƀo�C���h����
	pDevice->SetStreamSource(0, m_MeshFan[0].pVtxBuff, 0, sizeof(VERTEX_3D));
	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices(m_MeshFan[0].pIdxBuff);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, *m_pTexture);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLEFAN,			// �v���~�e�B�u�̎��
		0,
		0,
		m_MeshFan[0].nNumVtxMax,	// ���_��
		0,
		m_MeshFan[0].nNumPrim);		// �`�悷��v���~�e�B�u��

	// �J�����O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// �f�[�^�X�g���[���Ƀo�C���h����
	pDevice->SetStreamSource(0, m_MeshFan[1].pVtxBuff, 0, sizeof(VERTEX_3D));
	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices(m_MeshFan[1].pIdxBuff);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, *m_pTexture);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLEFAN,			// �v���~�e�B�u�̎��
		0,
		0,
		m_MeshFan[1].nNumVtxMax,	// ���_��
		0,
		m_MeshFan[1].nNumPrim);		// �`�悷��v���~�e�B�u��

	// ���C�g��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �A���t�@�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	// �t�H�O��ON
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// �J�����O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/*******************************************************************************
* �֐����FCSkySphere *CSkySphere::Create( Vector3 pos, float rad, int row, int column )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CSkySphere *CSkySphere::Create(Vector3 pos, float rad, int row, int column)
{
	CSkySphere *scene3D;
	scene3D = new CSkySphere;
	scene3D->Init(pos, rad, row, column);
	return scene3D;
}