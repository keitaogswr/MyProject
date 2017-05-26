/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Ftexture.cpp
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

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
LPDIRECT3DTEXTURE9 CTexture::m_Texture[TEXTURE_TYPE_MAX];
TEXTURE_PARAM CTexture::m_TexParam[TEXTURE_TYPE_MAX] =
{
	{ "data\\TEXTURE\\title.jpg" },					// TEXTURE_TYPE_TITLE
	{ "data\\TEXTURE\\title_logo.png" },			// TEXTURE_TYPE_TITLE_LOGO
	{ "data\\TEXTURE\\press_start.png" },			// TEXTURE_TYPE_PRESS_START
	{ "data\\TEXTURE\\sky003.jpg" },				// TEXTURE_TYPE_RESULT
	{ "data\\TEXTURE\\gear1_white.png" },			// TEXTURE_TYPE_GEAR_000
	{ "data\\TEXTURE\\number000.png" },				// TEXTURE_TYPE_NUMBER_000
	{ "data\\TEXTURE\\rand.jpg" },					// TEXTURE_TYPE_FIELD_000
	{ "data\\TEXTURE\\skydome000.jpg" },			// TEXTURE_TYPE_SKY_000
	{ "data\\TEXTURE\\tree001.png" },				// TEXTURE_TYPE_TREE_000
	{ "data\\TEXTURE\\tree002.png" },				// TEXTURE_TYPE_TREE_001
	{ "data\\TEXTURE\\tree003.png" },				// TEXTURE_TYPE_TREE_002
	{ "data\\TEXTURE\\tree004.png" },				// TEXTURE_TYPE_TREE_003
	{ "data\\TEXTURE\\tree005.png" },				// TEXTURE_TYPE_TREE_004
	{ "data\\TEXTURE\\weed001.png" },				// TEXTURE_TYPE_WEED_000
	{ "data\\TEXTURE\\weed002.png" },				// TEXTURE_TYPE_WEED_001
	{ "data\\TEXTURE\\weed003.png" },				// TEXTURE_TYPE_WEED_002
	{ "data\\TEXTURE\\effect000.jpg" },				// TEXTURE_TYPE_EFFECT_000
	{ "data\\TEXTURE\\explosion000.png" },			// TEXTURE_TYPE_SMOKE_000
	{ "data\\TEXTURE\\shadow000.jpg" },				// TEXTURE_TYPE_SHADOW_000
	{ "data\\TEXTURE\\bullet000.png" },				// TEXTURE_TYPE_BULLET_000
	{ "data\\TEXTURE\\reticle001.png" },			// TEXTURE_TYPE_RETICLE_000
	{ "data\\TEXTURE\\explosion.png" },				// TEXTURE_TYPE_EXPLOSION_000
	{ "data\\TEXTURE\\barrierEffect001.png" },		// TEXTURE_TYPE_BARRIER_000
	{ "data\\TEXTURE\\lifebar.png" },				// TEXTURE_TYPE_LIFEBAR_000
	{ "data\\TEXTURE\\lifebarGauge.png" },			// TEXTURE_TYPE_LIFEBAR_GAUGE_000
	{ "data\\TEXTURE\\pause.png" },					// TEXTURE_TYPE_PAUSE_000
	{ "data\\TEXTURE\\restart.png" },				// TEXTURE_TYPE_RESTART_000
	{ "data\\TEXTURE\\exit.png" },					// TEXTURE_TYPE_EXIT_000
	{ "data\\TEXTURE\\game_start.png" },			// TEXTURE_TYPE_GAME_START_000
	{ "data\\TEXTURE\\finish.png" },				// TEXTURE_TYPE_FINISH_000
	{ "data\\TEXTURE\\gameUI2.png" },				// TEXTURE_TYPE_GAMEUI_000
	{ "data\\TEXTURE\\thank_you.png" },				// TEXTURE_TYPE_THANK_YOU_000
	{ "data\\TEXTURE\\chargeEffect000.png" },		// TEXTURE_TYPE_CHARGE_000
	{ "data\\TEXTURE\\explosion_dome.png" }			// TEXTURE_TYPE_LAZER_000
};

/*******************************************************************************
* �֐����FCTexture::CTexture()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CTexture::CTexture()
{
}

/*******************************************************************************
* �֐����FCTexture::~CTexture()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CTexture::~CTexture()
{
}

/*******************************************************************************
* �֐����Fvoid CTexture::Load(void)
*
* ����	�F
* �߂�l�F
* ����	�F�ǂݍ��ݏ���
*******************************************************************************/
void CTexture::Load(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	for (int i = 0; i < TEXTURE_TYPE_MAX; i++)
	{// �e�N�X�`���̓ǂݍ���
		D3DXCreateTextureFromFile(
			pDevice,
			m_TexParam[i].texName,
			&m_Texture[i]);
	}
}

/*******************************************************************************
* �֐����Fvoid CTexture::Unload(void)
*
* ����	�F
* �߂�l�F
* ����	�F�������
*******************************************************************************/
void CTexture::Unload(void)
{
	for (int i = 0; i < TEXTURE_TYPE_MAX; i++)
	{// �e�N�X�`���̉��
		SAFE_RELEASE(m_Texture[i]);
	}
}

/*******************************************************************************
* �֐����FLPDIRECT3DTEXTURE9 GetTexture(int nIdx)
*
* ����	�F
* �߂�l�F
* ����	�F�ǂݍ��ݏ���
*******************************************************************************/
LPDIRECT3DTEXTURE9 *CTexture::GetTexture(int nIdx)
{
	return &m_Texture[nIdx];
}