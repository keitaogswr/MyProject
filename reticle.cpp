/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Freticle.cpp
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
#include "reticle.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define ROT_SPEED		( -0.03f )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCReticle::CReticle(DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene( DrawOrder, ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CReticle::CReticle(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene2D(DrawOrder, ObjType)
{
	m_bRockOn = false;
}


/*******************************************************************************
* �֐����FCReticle::~CReticle()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CReticle::~CReticle()
{
}

/*******************************************************************************
* �֐����Fvoid CReticle::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CReticle::Init(Vector3 pos)
{
	m_Pos = pos;

	m_fWidth = WIDTH;
	m_fHeight = HEIGHT;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_RETICLE_000);

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
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(m_Pos.x + sinf(D3DX_PI + m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(D3DX_PI + m_fAngle + m_Rot.z) * m_fLength,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_Pos.x + sinf(D3DX_PI - m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(D3DX_PI - m_fAngle + m_Rot.z) * m_fLength,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_Pos.x + sinf(-m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(-m_fAngle + m_Rot.z) * m_fLength,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_Pos.x + sinf(m_fAngle + m_Rot.z) * m_fLength,
		m_Pos.y + cosf(m_fAngle + m_Rot.z) * m_fLength,
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
	pVtx[1].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / TEXTURE_COLUMN);
	pVtx[3].tex = D3DXVECTOR2(1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN);

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CReticle::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CReticle::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CReticle::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CReticle::Update(void)
{
	if (m_bRockOn == true)
	{
		m_Col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		m_Rot.z += ROT_SPEED;
	}
	else
	{
		m_Col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		m_Rot.z = 0.0f;
	}
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CReticle::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CReticle::Draw(void)
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
* �֐����FCReticle *CReticle::Create( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CReticle *CReticle::Create(Vector3 pos)
{
	CReticle *reticle;
	reticle = new CReticle;
	reticle->Init(pos);
	return reticle;
}