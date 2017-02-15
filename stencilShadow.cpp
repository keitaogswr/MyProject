/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FstencilShadow.cpp
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
#include "stencilShadow.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float MAGNI = 0.005f;			// �����ɂ��e�̔{��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCStencilShadow::CStencilShadow(DRAWORDER DrawOrder, OBJTYPE ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CStencilShadow::CStencilShadow(DRAWORDER DrawOrder, OBJTYPE ObjType):CScene(DrawOrder, ObjType)
{
	m_pVtxBuff = NULL;
	m_pIdxBuff = NULL;
	m_pVtxBuff2D = NULL;
	m_pVtxBuffFan[0] = NULL;
	m_pVtxBuffFan[1] = NULL;
	m_Scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	m_fRadius = 0.0f;
	m_fRadiusN = 0.0f;
	m_fHeight = 0.0f;
	m_nRowBlock = 0;
	m_nColumnBlock = 0;
	m_nRowNumVtx = 0;
	m_nColumnNumVtx = 0;
	m_nNumVtxMax = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;

	m_fAlfa = 0.7f;
}

/*******************************************************************************
* �֐����FCStencilShadow::~CStencilShadow()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CStencilShadow::~CStencilShadow()
{
}

/*******************************************************************************
* �֐����Fvoid CStencilShadow::Init(Vector3 pos)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CStencilShadow::Init(Vector3 pos)
{
	m_Pos = pos;
	m_nRowBlock = 1;
	m_nColumnBlock = 16;
	m_nRowNumVtx = m_nRowBlock + 1;
	m_nColumnNumVtx = m_nColumnBlock + 1;
	m_fRadius = m_fRadiusN = 50.0f;
	m_fHeight = 100.0f;
	m_nNumVtxMax = m_nRowNumVtx * m_nColumnNumVtx;

	m_nNumIdx = m_nColumnNumVtx * m_nRowBlock * 2 + (m_nRowBlock - 1) * 2;
	m_nNumPrim = (m_nColumnBlock * m_nRowBlock * 2) + (m_nRowBlock - 1) * 4;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock * 0.5f - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
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

	// �t�@������ ///
	// ���_�o�b�t�@�̊m��
	for(int i = 0; i < 2; i++)
	{
		if (FAILED(pDevice->CreateVertexBuffer(
			sizeof(VERTEX_3D) * (m_nColumnNumVtx + 1),
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&m_pVtxBuffFan[i],
			NULL)))
		{
			return;
		}
	}

	// ���_�o�b�t�@�̐ݒ�
	m_pVtxBuffFan[0]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * 0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = 1; nCntColumn < m_nColumnNumVtx; nCntColumn++)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * 0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[0]->Unlock();

	m_pVtxBuffFan[1]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * -0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = m_nColumnNumVtx; nCntColumn >= 1; nCntColumn--)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * -0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[1]->Unlock();

	// �S���2D�|���S��
	// ���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff2D,
		NULL)))
	{
		return;
	}

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx2D;

	m_pVtxBuff2D->Lock(0, 0, (void**)&pVtx2D, 0);

	pVtx2D[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx2D[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx2D[2].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx2D[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	pVtx2D[0].rhw =
	pVtx2D[1].rhw =
	pVtx2D[2].rhw =
	pVtx2D[3].rhw = 1.0f;

	pVtx2D[0].col =
	pVtx2D[1].col =
	pVtx2D[2].col =
	pVtx2D[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_fAlfa);

	m_pVtxBuff2D->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CStencilShadow::Uninit(void)
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CStencilShadow::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pVtxBuff2D);
	SAFE_RELEASE(m_pVtxBuffFan[0]);
	SAFE_RELEASE(m_pVtxBuffFan[1]);
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CStencilShadow::Update(void)
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CStencilShadow::Update(void)
{
	m_fRadius += (m_fRadiusN - m_fRadius) * 0.5f;
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CStencilShadow::Draw(void)
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CStencilShadow::Draw(void)
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

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_MtxWorld);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, NULL);

	// �e��ݒ� /////
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Z�o�b�t�@
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);				// �X�e���V���o�b�t�@
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);				// �o�b�N�o�b�t�@
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILREF, 1);

	// �~��(�\)��+1�ŕ`�� ///
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
	// �t�@����`�� ///
	for (int i = 0; i < 2; i++)
	{
		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_3D);
		// �f�[�^�X�g���[���Ƀo�C���h����
		pDevice->SetStreamSource(0, m_pVtxBuffFan[i], 0, sizeof(VERTEX_3D));
		// �|���S���̕`��
		pDevice->DrawPrimitive(
			D3DPT_TRIANGLEFAN,			// �v���~�e�B�u�̎��
			0,							// �ŏ��̒��_�̃C���f�b�N�X
			m_nColumnBlock);			// �`�悷��v���~�e�B�u��
	}
	
	// �~��(��)��-1�ŕ`�� ///
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
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

	// �t�@����`�� ///
	for (int i = 0; i < 2; i++)
	{
		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_3D);
		// �f�[�^�X�g���[���Ƀo�C���h����
		pDevice->SetStreamSource(0, m_pVtxBuffFan[i], 0, sizeof(VERTEX_3D));
		// �|���S���̕`��
		pDevice->DrawPrimitive(
			D3DPT_TRIANGLEFAN,			// �v���~�e�B�u�̎��
			0,							// �ŏ��̒��_�C���f�b�N�X
			m_nColumnBlock);			// �`�悷��v���~�e�B�u��
	}

	// �e��ݒ� /////
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);			// �o�b�N�o�b�t�@
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);					// Z�o�b�t�@

	// �S���2D�|���S���`�� ///
	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, m_pVtxBuff2D, 0, sizeof(VERTEX_2D));
	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);
	// �|���S���̕`��
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
		0,							//�ŏ��̒��_�̃C���f�b�N�X
		2);							//�`�悷��v���~�e�B�u��

	// �e��ݒ� /////
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);					// Z�o�b�t�@
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);			// �X�e���V���o�b�t�@
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Z�o�b�t�@
}

/*******************************************************************************
* �֐����FCStencilShadow *CStencilShadow::Create(Vector3 pos)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CStencilShadow *CStencilShadow::Create(Vector3 pos)
{
	CStencilShadow *stencilShadow;
	stencilShadow = new CStencilShadow;
	stencilShadow->Init(pos);
	return stencilShadow;
}

/*******************************************************************************
* �֐����Fvoid SetShadow(Vector3 pos, Vector3 obj, float radius, float heigh)
*
* ����	�FVector3 pos		// �|���S���̌��_�ʒu
*		�@Vector3 obj		// �I�u�W�F�N�g�̈ʒu
*		�@float radius		// �e�̔��a
*		�@float height		// �V�����_�[�̍���
* �߂�l�F
* ����	�F�e�ݒ菈��
*******************************************************************************/
void CStencilShadow::Set(Vector3 pos, Vector3 obj, float radius, float height)
{
	// �\���̂ɒl��ݒ�
	m_Pos = pos;
	m_fRadiusN = radius;
	m_fHeight = height;
	float diff = obj.y - pos.y;
	m_Scl.x = 1.0f + (diff * MAGNI);
	m_Scl.z = 1.0f + (diff * MAGNI);

	m_fAlfa = 0.7f - (diff * MAGNI);
}

/*******************************************************************************
* �֐����Fvoid SetVertex(void)
*
* ����	�F
* �߂�l�F
* ����	�F�e���_�ݒ菈��
*******************************************************************************/
void CStencilShadow::SetVertex(void)
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
			pVtx[nIdxCnt].pos.y = m_fHeight * (m_nRowBlock * 0.5f - nCntRow);
			pVtx[nIdxCnt].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * nCntColumn) * m_fRadius;
		}
	}

	m_pVtxBuff->Unlock();
	// �t�@������
	m_pVtxBuffFan[0]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * 0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = 1; nCntColumn <= m_nColumnNumVtx; nCntColumn++)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * 0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[0]->Unlock();

	m_pVtxBuffFan[1]->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos.x = 0.0f;
	pVtx[0].pos.y = m_fHeight * -0.5f;
	pVtx[0].pos.z = 0.0f;
	for (int nCntColumn = m_nColumnNumVtx; nCntColumn >= 1; nCntColumn--)
	{
		pVtx[nCntColumn].pos.x = sinf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
		pVtx[nCntColumn].pos.y = m_fHeight * -0.5f;
		pVtx[nCntColumn].pos.z = cosf(-D3DX_PI + 360.0f / m_nColumnBlock * RAD * (nCntColumn - 1)) * m_fRadius;
	}

	m_pVtxBuffFan[1]->Unlock();

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx2D;

	m_pVtxBuff2D->Lock(0, 0, (void**)&pVtx2D, 0);


	pVtx2D[0].col =
	pVtx2D[1].col =
	pVtx2D[2].col =
	pVtx2D[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_fAlfa);

	m_pVtxBuff2D->Unlock();
}