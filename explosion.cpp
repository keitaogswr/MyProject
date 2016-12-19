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
#include "explosion.h"
#include "texture.h"
#include "sound.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH				( 30 )		// �|���S����
#define HEIGHT				( 30 )		// �|���S������
#define TEXTURE_ROW			( 2 )		// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN		( 8 )		// �e�N�X�`���s������
#define MOVE_SPEED			( 10.0f )	// �ړ����x
#define LIFE_MAX			( 1000 )	// ����
#define ATTEN_DEF			( 0.0001f )	// ���������W��
#define ATTEN_RISE			( 2.0f )	// �����W���̏㏸�{��
#define DRAW_SPEED			( 3 )		// �`�摬�x

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCExplosion::CExplosion()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CExplosion::CExplosion(DRAWORDER DrawOrder, OBJTYPE ObjType) : CAnimationBoard(DrawOrder, ObjType)
{
	m_nCntAnim = 0;
	m_nPatternAnim = 0;
	m_nRowAnim = 0;
	m_nTexColumn = TEXTURE_COLUMN;
	m_nTexRow = TEXTURE_ROW;
	CSound::Play(CSound::SOUND_LABEL_EXPLOSION_000);
}

/*******************************************************************************
* �֐����FCExplosion::~CExplosion()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CExplosion::~CExplosion()
{
}

/*******************************************************************************
* �֐����Fvoid CAnimationBoard::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CExplosion::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_Pos = pos;
	m_Col = col;
	m_fWidth = width;
	m_fHeight = height;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_EXPLOSION_000);

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
* �֐����Fvoid CExplosion::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CExplosion::Update(void)
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
* �֐����FCExplosion *CExplosion::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CExplosion *CExplosion::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CExplosion *effect;
	effect = new CExplosion;

	effect->Init(pos, col, width, height);
	return effect;
}