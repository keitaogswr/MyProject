/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FanimationBoard.cpp
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
#include "mode.h"
#include "game.h"
#include "camera.h"
#include "scene.h"
#include "scene3D.h"
#include "meshField.h"
#include "billboard.h"
#include "input.h"
#include "effect.h"
#include "animationBoard.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float	WIDTH = 30.0f;			// �|���S����
const float	HEIGHT = 30.0f;			// �|���S������
const int	TEXTURE_ROW = 1;		// �e�N�X�`���񕪊���
const int	TEXTURE_COLUMN = 8;		// �e�N�X�`���s������
const float	MOVE_SPEED = 10.0f;		// �ړ����x
const int	LIFE_MAX = 1000;		// ����
const float	ATTEN_DEF = 0.0001f;	// ���������W��
const float	ATTEN_RISE = 2.0f;		// �����W���̏㏸�{��
const int	DRAW_SPEED = 5;			// �`�摬�x

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCAnimationBoard::CAnimationBoard()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CAnimationBoard::CAnimationBoard(DRAWORDER DrawOrder, OBJTYPE ObjType) : CEffect(DrawOrder, ObjType)
{
	m_nCntAnim = 0;
	m_nPatternAnim = 0;
	m_nRowAnim = 0;
	m_nTexColumn = TEXTURE_COLUMN;
	m_nTexRow = TEXTURE_ROW;
	m_nLoopCnt = 0;
}

/*******************************************************************************
* �֐����FCAnimationBoard::~CAnimationBoard()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CAnimationBoard::~CAnimationBoard()
{
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CAnimationBoard::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_Pos = pos;
	m_Col = col;
	m_fWidth = width;
	m_fHeight = height;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_SMOKE_000);

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

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
		m_fHeight * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f,
		m_fHeight * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f,
		-m_fHeight * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f,
		-m_fHeight * 0.5f,
		0.0f);

	pVtx[0].nor =
	pVtx[1].nor =
	pVtx[2].nor =
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = m_Col;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_COLUMN, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_ROW);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_COLUMN, 1.0f / TEXTURE_ROW);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CAnimationBoard::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
	// �폜�t���O
	m_Delete = true;
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CAnimationBoard::Update(void)
{
	// ���������炷
	m_nLife--;

	// �p�^�[���`��X�V
	m_nCntAnim++;
	if ((m_nCntAnim % DRAW_SPEED) == 0)
	{
		m_nPatternAnim++;
		if ((m_nPatternAnim % TEXTURE_COLUMN) == 0)
		{
			m_nRowAnim++;
			if ((m_nRowAnim % TEXTURE_ROW) == 0)
			{
				SetDeleteFlg(true);
			}
		}
		// ���_�ݒ�
		SetVertex();
	}
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CAnimationBoard::Draw(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// �J�����̎擾
	CGame *game = (CGame*)CManager::GetMode();
	CCamera *camera = game->GetCamera();

	// �e��ݒ� /////
	SetRenderStateBegin();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �r���[�}�g���b�N�X�̋t�s������߂�
	D3DXMatrixInverse(&m_MtxWorld,
		NULL,
		&mtxView);

	// �}�g���N�X�̈ړ��Ɋւ���ϐ��N���A
	m_MtxWorld._41 = 0.0f;
	m_MtxWorld._42 = 0.0f;
	m_MtxWorld._43 = 0.0f;

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);
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
	SetRenderStateEnd();
}

/*******************************************************************************
* �֐����FCAnimationBoard *CAnimationBoard::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CAnimationBoard *CAnimationBoard::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CAnimationBoard *effect;
	effect = new CAnimationBoard;

	effect->Init(pos, col, width, height);
	return effect;
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::SetVertex( void )
*
* ����	�F
* �߂�l�F
* ����	�F���_�ݒ菈��
*******************************************************************************/
void CAnimationBoard::SetVertex(void)
{
	// ���_�o�b�t�@�̐ݒ�
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-m_fWidth * 0.5f, m_fHeight * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth * 0.5f, m_fHeight * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth * 0.5f, -m_fHeight * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth * 0.5f, -m_fHeight * 0.5f, 0.0f);

	pVtx[0].nor =
	pVtx[1].nor =
	pVtx[2].nor =
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = m_Col;


	pVtx[0].tex.x = m_nPatternAnim * (1.0f / m_nTexColumn);
	pVtx[0].tex.y = m_nRowAnim * (1.0f / m_nTexRow);
	pVtx[1].tex.x = m_nPatternAnim * (1.0f / m_nTexColumn) + (1.0f / m_nTexColumn);
	pVtx[1].tex.y = m_nRowAnim * (1.0f / m_nTexRow);
	pVtx[2].tex.x = m_nPatternAnim * (1.0f / m_nTexColumn);
	pVtx[2].tex.y = m_nRowAnim * (1.0f / m_nTexRow) + (1.0f / m_nTexRow);
	pVtx[3].tex.x = m_nPatternAnim * (1.0f / m_nTexColumn) + (1.0f / m_nTexColumn);
	pVtx[3].tex.y = m_nRowAnim * (1.0f / m_nTexRow) + (1.0f / m_nTexRow);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::SetRenderStateBegin( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�J�n����
*******************************************************************************/
void CAnimationBoard::SetRenderStateBegin(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �t�H�O��OFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
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
* �֐����Fvoid CAnimationBoard::SetRenderStateEnd( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�I������
*******************************************************************************/
void CAnimationBoard::SetRenderStateEnd(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}