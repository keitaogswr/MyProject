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
#include "texture.h"
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
#include "smoke.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH				( 30 )		// �|���S����
#define HEIGHT				( 30 )		// �|���S������
#define TEXTURE_ROW			( 8 )		// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN		( 1 )		// �e�N�X�`���s������
#define MOVE_SPEED			( 10.0f )	// �ړ����x
#define LIFE_MAX			( 1000 )	// ����
#define ATTEN_DEF			( 0.0001f )	// ���������W��
#define ATTEN_RISE			( 2.0f )	// �����W���̏㏸�{��
#define DRAW_SPEED			( 5 )		// �`�摬�x
#define UP_SPEED			( 0.5f )	// �㏸���x
#define LENG_SPEED			( 0.5f )	// �㏸���x
const float RAND_WIDTH = 15.0f;

/*******************************************************************************
* �֐����FCSmoke::CSmoke()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CSmoke::CSmoke(DRAWORDER DrawOrder, OBJTYPE ObjType) :CAnimationBoard(DrawOrder, ObjType)
{
}

/*******************************************************************************
* �֐����FCSmoke::~CSmoke()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CSmoke::~CSmoke()
{
}

/*******************************************************************************
* �֐����Fvoid CSmoke::Init( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CSmoke::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_Pos = pos;
	m_Col = col;
	float randLength = -RAND_WIDTH + (int)(rand() * (RAND_WIDTH * 2 + 1) / (1 + RAND_MAX));
	m_fWidth = width + randLength;
	m_fHeight = height + randLength;
	
	m_Rot = Vector3(0.0f, 0.0f, -D3DX_PI + (int)(rand() * (D3DX_PI * 2 + 1.0f) / (1.0f + RAND_MAX)));

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
* �֐����Fvoid CSmoke::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CSmoke::Update(void)
{
	// ���������炷
	m_nLife--;

	m_fWidth += LENG_SPEED;
	m_fHeight += LENG_SPEED;

	// �p�^�[���`��X�V
	m_nCntAnim++;

	m_Pos.y += UP_SPEED;
	if (m_nCntAnim == DRAW_SPEED)
	{
		m_nCntAnim = 0;
		m_nPatternAnim++;
		if (m_nPatternAnim == TEXTURE_ROW)
		{
			m_nPatternAnim = 0;

			SetDeleteFlg(true);
		}
	}
	// ���_�ݒ�
	SetVertex();
}

/*******************************************************************************
* �֐����FCSmoke *CSmoke::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CSmoke *CSmoke::Create(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	CSmoke *effect;
	effect = new CSmoke;

	effect->Init(pos, col, width, height);
	return effect;
}
