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
#include "barrier.h"
#include "texture.h"


/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )				// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN	( 1 )				// �e�N�X�`���s������
#define MESHDOME_ANGLE	( 80.0f )			// �h�[�������̊p�x
#define ROT_SPEED		( 0.01f )			// 
const float MAGNIFICATION = 0.1f;

/*******************************************************************************
* �֐����FCBarrier::CBarrier()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CBarrier::CBarrier(DRAWORDER DrawOrder, OBJTYPE ObjType) :CSkySphere(DrawOrder, ObjType)
{
	ZeroMemory(&m_MeshFan, sizeof(m_MeshFan));
	ZeroMemory(&m_MeshDome, sizeof(m_MeshDome));
	m_nAlfa = m_nAlfaN = 1.0f;
}

/*******************************************************************************
* �֐����FCBarrier::~CBarrier()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CBarrier::~CBarrier()
{
}

/*******************************************************************************
* �֐����Fvoid CBarrier::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CBarrier::Init(Vector3 pos, float rad, int row, int column)
{
	m_Pos = pos;

	// �h�[������
	m_MeshDome.nRowBlock = row;
	m_MeshDome.nColumnBlock = column;
	m_MeshDome.nRowNumVtx = m_MeshDome.nRowBlock + 1;
	m_MeshDome.nColumnNumVtx = m_MeshDome.nColumnBlock + 1;
	m_MeshDome.fRadius = rad;
	m_MeshDome.nNumVtxMax = (m_MeshDome.nRowNumVtx * 2 - 1) * m_MeshDome.nColumnNumVtx;
	m_MeshDome.nNumIdx = (m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 * 2) + (m_MeshDome.nRowBlock * 2 - 1) * 2;
	m_MeshDome.nNumPrim = (m_MeshDome.nColumnNumVtx * m_MeshDome.nRowBlock * 2 * 2) + (m_MeshDome.nRowBlock * 2 - 1) * 4;

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
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_BARRIER_000);

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
	/// �h�[������
	m_MeshDome.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntRow = m_MeshDome.nRowNumVtx - 1; nCntRow >= 0; nCntRow--)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
				sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow),
				cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// �e�N�X�`���u���b�N�\��
			pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;
		}
	}
	for (int nCntRow = 1; nCntRow < m_MeshDome.nRowNumVtx; nCntRow++)
	{
		for (int nCntColumn = 0; nCntColumn < m_MeshDome.nColumnNumVtx; nCntColumn++, nIdxCnt++)
		{
			pVtx[nIdxCnt].pos.x = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.y = sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) * -m_MeshDome.fRadius;
			pVtx[nIdxCnt].pos.z = cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn) * m_MeshDome.fRadius;

			pVtx[nIdxCnt].nor = D3DXVECTOR3(cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				sinf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn),
				sinf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow),
				cosf(MESHDOME_ANGLE / m_MeshDome.nRowBlock * RAD * nCntRow) *
				cosf(-D3DX_PI + 360.0f / m_MeshDome.nColumnBlock * RAD * nCntColumn));

			pVtx[nIdxCnt].col = D3DCOLOR_RGBA(255, 255, 255, 255);

			// �e�N�X�`���u���b�N�\��
			pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;
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

	for (y = 0, cnt = 0; y < m_MeshDome.nRowBlock * 2; y++)
	{
		for (x = 0; x < m_MeshDome.nColumnNumVtx; x++, cnt += 2, pIdx += 2)
		{
			pIdx[0] = x + (y + 1) * m_MeshDome.nColumnNumVtx;
			pIdx[1] = x + (y + 0) * m_MeshDome.nColumnNumVtx;
			if (x == m_MeshDome.nColumnNumVtx - 1 && y != m_MeshDome.nRowBlock * 2 - 1)
			{
				cnt += 2;
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
* �֐����Fvoid CBarrier::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CBarrier::Uninit(void)
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
* �֐����Fvoid CBarrier::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CBarrier::Update(void)
{
	m_nAlfa += (m_nAlfaN - m_nAlfa) * MAGNIFICATION;

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;
	int nIdxCnt = 0;
	/// �h�[������
	m_MeshDome.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < m_MeshDome.nNumVtxMax; i++)
	{
		pVtx[i].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_nAlfa);
	}

	m_MeshDome.pVtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CBarrier::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CBarrier::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// �J�����O�̐ݒ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	CSkySphere::Draw();
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/*******************************************************************************
* �֐����FCBarrier *CBarrier::Create( Vector3 pos, float rad, int row, int column )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CBarrier *CBarrier::Create(Vector3 pos, float rad, int row, int column)
{
	CBarrier *scene3D;
	scene3D = new CBarrier;
	scene3D->Init(pos, rad, row, column);
	return scene3D;
}