/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Flazer.cpp
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
#include "debugProc.h"
#include "texture.h"
#include "lazer.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float MAGNI = 0.005f;			// �����ɂ��e�̔{��
const float RADIUS_ATEEN = 0.1f;	// ���a�̊��������W��
const float HEIGHT = 5000.0f;		// ����
const int	DELETE_CNT = 150;		// ���ŃJ�E���^
const int	DAMAGE_VALUE = 1;		// �_���[�W
const int	KEEP_VALUE = 20;		// ��ϋv�l

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCLazer::CLazer(DRAWORDER DrawOrder, OBJTYPE ObjType) :CMeshCylinder(DrawOrder, ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CLazer::CLazer(DRAWORDER DrawOrder, OBJTYPE ObjType) :CMeshCylinder(DrawOrder, ObjType)
{
	m_pCylinder = NULL;
	m_nDeleteCnt = 0;
}

/*******************************************************************************
* �֐����FCLazer::~CLazer()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CLazer::~CLazer()
{
}

/*******************************************************************************
* �֐����Fvoid CLazer::Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CLazer::Init(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
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

	m_pCylinder = CMeshCylinder::Create(pos, rot, D3DXCOLOR(0.7f, 0.0f, 1.0f, 0.8f), radius * 0.5f, matrix);
}

/*******************************************************************************
* �֐����Fvoid CLazer::Uninit(void)
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CLazer::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_pVtxBuff);
	// �폜�t���O
	m_Delete = true;
	if (m_pCylinder)
	{
		m_pCylinder->SetDeleteFlg(true);
		m_pCylinder = NULL;
	}
}

/*******************************************************************************
* �֐����Fvoid CLazer::Update(void)
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CLazer::Update(void)
{
	CMeshCylinder::Update();
	Collision();
	m_nDeleteCnt++;
	if ((m_nDeleteCnt % DELETE_CNT) == 0)
	{
		SetDeleteFlg(true);
		m_pCylinder->SetDeleteFlg(true);
	}
}

/*******************************************************************************
* �֐����Fvoid CLazer::Draw(void)
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CLazer::Draw(void)
{
	CMeshCylinder::Draw();
}

/*******************************************************************************
* �֐����FCLazer *CLazer::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CLazer *CLazer::Create(Vector3 pos, Vector3 rot, D3DXCOLOR col, float radius, D3DXMATRIX* matrix)
{
	CLazer *lazer;
	lazer = new CLazer;
	lazer->Init(pos, rot, col, radius, matrix);
	return lazer;
}

/*******************************************************************************
* �֐����Fvoid CLazer::SetRenderStateBegin( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�J�n����
*******************************************************************************/
void CLazer::SetRenderStateBegin(void)
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
* �֐����Fvoid CLazer::SetRenderStateEnd( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�I������
*******************************************************************************/
void CLazer::SetRenderStateEnd(void)
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
void CLazer::SetWorldMatrix(void)
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

	// �e�̃}�g���N�X�Ɗ|�����킹��
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		m_pParentMtx);
}

/*******************************************************************************
* �֐����Fvoid CMeshCylinder::Collision( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����蔻�菈��
*******************************************************************************/
void CLazer::Collision(void)
{
	/* �ϐ���` */
	Vector3 pos, out;
	Vector3 vec0, vec1;
	float length;
	float work;

	// ���W�ϊ�
	SetWorldMatrix();
	D3DXVec3TransformCoord(&pos,
		&Vector3(0.0f, m_fHeight, 0.0f),
		&m_MtxWorld);

	// �x�N�g���v�Z
	vec0 = pos - m_Pos;				// ���[�U�[�̃x�N�g��
	length = vec0.LengthSq();		// ���[�U�[�̒���

	// �����蔻�菈��
	if (length > 0.0f)
	{
		CScene *scene = CScene::GetList(DRAWORDER_3D);
		CScene *next = NULL;
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
			if (scene->GetObjType() == OBJTYPE_PLAYER)
			{
				// �x�N�g���v�Z
				vec1 = scene->GetPosition() - m_Pos;					// ���[�U�[�ƃv���C���[�̋���
				// �����̌v�Z
				work = D3DXVec3Dot(&vec1, &vec0) / length;				// ���[�U�[�̒����ɑ΂���ŒZ�����̊���
				work = work < 0.0f ? 0.0f : work > 1.0f ? 1.0f : work;	// ���[�U�[�̒����̊O�̏ꍇ�C��
				// �ߎ��_�̌v�Z
				out = m_Pos + (vec0 * work);
				// �Փ˔���
				if ((scene->GetPosition() - out).Length() < m_fRadius + scene->GetCollision())
				{
					scene->SetDamage(DAMAGE_VALUE);
					scene->SetKeep(KEEP_VALUE);
				}
			}
			scene = next;
		}
	}
}