/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fnumber.cpp
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
#include "texture.h"
#include "scene.h"
#include "scene2D.h"
#include "number.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 25 )
#define HEIGHT			( 50 )
#define TEXTURE_ROW		( 10 )
#define TEXTURE_COLUMN	( 1 )
#define DEF_TIME		( 10 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCNumber::CNumber()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CNumber::CNumber()
{
	m_Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_Rot = Vector3(0.0f, 0.0f, 0.0f);
}

/*******************************************************************************
* �֐����FCNumber::~CNumber()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CNumber::~CNumber()
{
}

/*******************************************************************************
* �֐����Fvoid CNumber::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CNumber::Init(Vector3 pos)
{
	m_Pos = pos;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_NUMBER_000);

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

	m_fLength = sqrtf(WIDTH * WIDTH + HEIGHT * HEIGHT) * 0.5f;
	m_fAngle = atan2f(WIDTH, HEIGHT);

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
* �֐����Fvoid CNumber::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CNumber::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CNumber::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CNumber::Update(void)
{
	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].tex.x = m_nNum * 0.1f;
	pVtx[0].tex.y = 0.0f;
	pVtx[1].tex.x = m_nNum * 0.1f + 0.1f;
	pVtx[1].tex.y = 0.0f;
	pVtx[2].tex.x = m_nNum * 0.1f;
	pVtx[2].tex.y = 1.0f;
	pVtx[3].tex.x = m_nNum * 0.1f + 0.1f;
	pVtx[3].tex.y = 1.0f;

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CNumber::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CNumber::Draw(void)
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
		2);						//�`�悷��v���~�e�B�u��
}

/*******************************************************************************
* �֐����FCNumber *CNumber::Create( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CNumber *CNumber::Create(Vector3 pos)
{
	CNumber *number;
	number = new CNumber;
	number->Init(pos);
	return number;
}

/*******************************************************************************
* �֐����Fvoid CNumber::SetNum( int num )
*
* ����	�F
* �߂�l�F
* ����	�F���l�Z�b�g����
*******************************************************************************/
void CNumber::SetNum(int num)
{
	if (num >= 0 && num <= 9)
	{
		m_nNum = num;
	}
	else
	{
		MessageBox(*CManager::GetHWND(), "���l������ł͂���܂���", "�x��", MB_ICONWARNING);
	}
}