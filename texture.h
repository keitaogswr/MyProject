/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Ftexture.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#pragma once

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/
// �e�N�X�`���^�C�v�̖��O
typedef enum
{
	TEXTURE_TYPE_TITLE,
	TEXTURE_TYPE_TITLE_LOGO,
	TEXTURE_TYPE_PRESS_START,
	TEXTURE_TYPE_RESULT,
	TEXTURE_TYPE_GEAR_000,
	TEXTURE_TYPE_NUMBER_000,
	TEXTURE_TYPE_FIELD_000,
	TEXTURE_TYPE_SKY_000,
	TEXTURE_TYPE_TREE_000,
	TEXTURE_TYPE_TREE_001,
	TEXTURE_TYPE_TREE_002,
	TEXTURE_TYPE_TREE_003,
	TEXTURE_TYPE_TREE_004,
	TEXTURE_TYPE_WEED_000,
	TEXTURE_TYPE_WEED_001,
	TEXTURE_TYPE_WEED_002,
	TEXTURE_TYPE_EFFECT_000,
	TEXTURE_TYPE_SMOKE_000,
	TEXTURE_TYPE_SHADOW_000,
	TEXTURE_TYPE_BULLET_000,
	TEXTURE_TYPE_RETICLE_000,
	TEXTURE_TYPE_EXPLOSION_000,
	TEXTURE_TYPE_MAX
}TEXTURE_TYPE;

// �e�N�X�`���̃p�����[�^
typedef struct
{
	char *texName;
}TEXTURE_PARAM;

/*******************************************************************************
* �O���錾
*******************************************************************************/

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CTexture
{
private:
	static LPDIRECT3DTEXTURE9 m_Texture[TEXTURE_TYPE_MAX];					// �e�N�X�`���ւ̃|�C���^
	static TEXTURE_PARAM m_TexParam[TEXTURE_TYPE_MAX];						// �e�N�X�`���̃p�����[�^
public:
	CTexture();
	~CTexture();

	static void Load(void);
	static void Unload(void);

	static LPDIRECT3DTEXTURE9 *GetTexture(int nIdx);
};

