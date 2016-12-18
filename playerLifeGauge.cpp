/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene2D.cpp
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
#include "scene2D.h"
#include "texture.h"
#include "playerLifeGauge.h"

/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene2D.cpp
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
#include "scene2D.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 600 )
#define HEIGHT			( 60 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCPlayerLifeGauge::CPlayerLifeGauge()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CPlayerLifeGauge::CPlayerLifeGauge(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene2D(DrawOrder, ObjType)
{
	m_fMax = 1.0f;
	m_fCurrent = 1.0f;
}

/*******************************************************************************
* �֐����FCPlayerLifeGauge::~CPlayerLifeGauge()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CPlayerLifeGauge::~CPlayerLifeGauge()
{
}

/*******************************************************************************
* �֐����Fvoid CPlayerLifeGauge::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CPlayerLifeGauge::Init(Vector3 pos)
{
	// �Q�[�W�̐���
	CScene2D::Create(pos, WIDTH, HEIGHT, TEXTURE_TYPE_LIFEBAR_GAUGE_000);
	// �l�̊i�[
	m_Pos = pos;
	m_fWidth = WIDTH;
	m_fHeight = HEIGHT;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_LIFEBAR_000);

	// ���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * VERTEX_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_VtxBuff,
		NULL)))
	{
		return;
	}

	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight) * 0.5f;
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// ���_�o�b�t�@�̐ݒ�
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CPlayerLifeGauge::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CPlayerLifeGauge::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CPlayerLifeGauge::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CPlayerLifeGauge::Update(void)
{
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CPlayerLifeGauge::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CPlayerLifeGauge::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, *m_pTexture);

	// �|���S���̕`��
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
		0,							//�ŏ��̒��_�̃C���f�b�N�X
		2);							//�`�悷��v���~�e�B�u��
}

/*******************************************************************************
* �֐����FCPlayerLifeGauge *CPlayerLifeGauge::Create( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CPlayerLifeGauge *CPlayerLifeGauge::Create(Vector3 pos)
{
	CPlayerLifeGauge *scene2D;
	scene2D = new CPlayerLifeGauge;
	scene2D->Init(pos);
	return scene2D;
}

/*******************************************************************************
* �֐����Fvoid CPlayerLifeGauge::SetVertex(void)
*
* ����	�F
* �߂�l�F
* ����	�F���_�ݒ菈��
*******************************************************************************/
void CPlayerLifeGauge::SetVertex(void)
{
	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f,
		m_Pos.y - m_fHeight * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f + m_fWidth * m_fCurrent / m_fMax,
		m_Pos.y - m_fHeight * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f,
		m_Pos.y + m_fHeight * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_Pos.x - m_fWidth * 0.5f + m_fWidth * m_fCurrent / m_fMax,
		m_Pos.y + m_fHeight * 0.5f,
		0.0f);

	pVtx[0].rhw =
	pVtx[1].rhw =
	pVtx[2].rhw =
	pVtx[3].rhw = 1.0f;

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = m_Col;

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_fCurrent / m_fMax, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(m_fCurrent / m_fMax, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}