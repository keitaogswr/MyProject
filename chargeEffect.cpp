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
#include "chargeEffect.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const float	WIDTH = 30.0f;			// �|���S����
const float	HEIGHT = 30.0f;			// �|���S������
const int	TEXTURE_ROW = 1;		// �e�N�X�`���񕪊���
const int	TEXTURE_COLUMN = 10;	// �e�N�X�`���s������
const float	MOVE_SPEED = 10.0f;		// �ړ����x
const int	LIFE_MAX = 1000;		// ����
const float	ATTEN_DEF = 0.0001f;	// ���������W��
const float	ATTEN_RISE = 2.0f;		// �����W���̏㏸�{��
const int	DRAW_SPEED = 5;			// �`�摬�x
const int	LOOP_MAX = 3;			// ���[�v��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCChargeEffect::CChargeEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) : CAnimationBoard(DrawOrder, ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CChargeEffect::CChargeEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) : CAnimationBoard(DrawOrder, ObjType)
{
	m_nTexColumn = TEXTURE_COLUMN;
	m_nTexRow = TEXTURE_ROW;
}

/*******************************************************************************
* �֐����FCChargeEffect::~CChargeEffect()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CChargeEffect::~CChargeEffect()
{
}

/*******************************************************************************
* �֐����Fvoid CChargeEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CChargeEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_Pos = pos;
	m_Col = col;
	m_fWidth = width;
	m_fHeight = height;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_CHARGE_000);

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
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CChargeEffect::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CChargeEffect::Update(void)
{
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
				m_nLoopCnt++;
			}
		}
		// ���_�ݒ�
		SetVertex();
	}
	if (m_nLoopCnt >= LOOP_MAX)
	{
		SetDeleteFlg(true);
	}
}

/*******************************************************************************
* �֐����FCChargeEffect *CChargeEffect::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CChargeEffect *CChargeEffect::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CChargeEffect *effect;
	effect = new CChargeEffect;

	effect->Init(pos, col, width, height);
	return effect;
}

/*******************************************************************************
* �֐����Fvoid CChargeEffect::SetRenderStateBegin( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�J�n����
*******************************************************************************/
void CChargeEffect::SetRenderStateBegin(void)
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���C�g��OFF
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �t�H�O��OFF
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	// Z�e�X�g��OFF
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// �A���t�@�e�X�g��ON
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
	// ���Z�����ɂ��A���t�@�u�����h�̃����_�[�X�e�[�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

/*******************************************************************************
* �֐����Fvoid CChargeEffect::SetRenderStateEnd( void )
*
* ����	�F
* �߂�l�F
* ����	�F�����_���[�X�e�[�g�ݒ�I������
*******************************************************************************/
void CChargeEffect::SetRenderStateEnd(void)
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
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// ���̃A���t�@�u�����h�̐ݒ�ɖ߂�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}