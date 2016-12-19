/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fpause.cpp
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
#include "fade.h"
#include "input.h"
#include "game.h"
#include "title.h"
#include "result.h"
#include "load.h"
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "texture.h"
#include "pause.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;
const D3DXCOLOR DEFAULT_COLOR = D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.6f);

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
LPDIRECT3DVERTEXBUFFER9 CPause::m_VtxBuff;	// ���_�o�b�t�@
float CPause::m_fWidth;						// �|���S����
float CPause::m_fHeight;					// �|���S������
D3DXCOLOR CPause::m_Col;					// �F
CScene2D *CPause::m_pLogo[PAUSE_LOGO_MAX];
int CPause::m_nCurrent;

/*******************************************************************************
* �֐����FCPause::CPause()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CPause::CPause()
{
}

/*******************************************************************************
* �֐����FCPause::~CPause()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CPause::~CPause()
{
}

/*******************************************************************************
* �֐����Fvoid CPause::Init( void )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CPause::Init(void)
{
	// �l�̊i�[
	m_fWidth = WIDTH;
	m_fHeight = HEIGHT;
	m_Col = DEFAULT_COLOR;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f - m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f - m_fHeight * 0.5f,
		0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f + m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f - m_fHeight * 0.5f,
		0.0f);
	pVtx[2].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f - m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f + m_fHeight * 0.5f,
		0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f + m_fWidth * 0.5f,
		SCREEN_HEIGHT * 0.5f + m_fHeight * 0.5f,
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
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	m_VtxBuff->Unlock();

	m_pLogo[0] = CScene2D::Create(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f - 200.0f, 0.0f), 400.0f, 200.0f, TEXTURE_TYPE_PAUSE_000);
	m_pLogo[0]->SetPauseFlg(true);
	m_pLogo[0]->SetDrawFlg(false);
	m_pLogo[1] = CScene2D::Create(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), 400.0f, 200.0f, TEXTURE_TYPE_RESTART_000);
	m_pLogo[1]->SetPauseFlg(true);
	m_pLogo[1]->SetDrawFlg(false);
	m_pLogo[2] = CScene2D::Create(Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f + 200.0f, 0.0f), 400.0f, 200.0f, TEXTURE_TYPE_EXIT_000);
	m_pLogo[2]->SetPauseFlg(true);
	m_pLogo[2]->SetDrawFlg(false);
}

/*******************************************************************************
* �֐����Fvoid CPause::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CPause::Uninit(void)
{
	// ���
	SAFE_RELEASE(m_VtxBuff);
}

/*******************************************************************************
* �֐����Fvoid CPause::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CPause::Update(void)
{
	// �|�[�Y��ʑ���
	if (CInput::GetKeyboardTrigger(DIK_W))
	{
		m_nCurrent--;
	}
	if (CInput::GetKeyboardTrigger(DIK_S))
	{
		m_nCurrent++;
	}
	if (m_nCurrent < 0)
	{
		m_nCurrent = PAUSE_LOGO_MAX - 1;
	}
	if (m_nCurrent >= PAUSE_LOGO_MAX)
	{
		m_nCurrent = 0;
	}
	// �I�����Ƃ̓���
	CGame *game = (CGame*)CManager::GetMode();
	if(CInput::GetKeyboardTrigger(DIK_RETURN))
	{
		switch (m_nCurrent)
		{
		case 0:
			game->SetPause(false);
			break;
		case 1:
			CFade::Start(new CLoad);
			break;
		case 2:
			CFade::Start(new CTitle);
			break;
		default:
			break;
		}
	}

	// ���S�̍X�V
	for (int i = 0; i < PAUSE_LOGO_MAX; i++)
	{
		m_pLogo[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
	}
	m_pLogo[m_nCurrent]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	for (int i = 0; i < PAUSE_LOGO_MAX; i++)
	{
		m_pLogo[i]->Update();
	}
}

/*******************************************************************************
* �֐����Fvoid CPause::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CPause::Draw(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource(0, m_VtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, NULL);

	// �|���S���̕`��
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
		0,							//�ŏ��̒��_�̃C���f�b�N�X
		2);							//�`�悷��v���~�e�B�u��
	for (int i = 0; i < PAUSE_LOGO_MAX; i++)
	{
		m_pLogo[i]->Draw();
	}
}