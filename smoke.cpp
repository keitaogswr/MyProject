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
#define DRAW_SPEED			( 3 )		// �`�摬�x
#define UP_SPEED			( 0.5f )	// �㏸���x
#define LENG_SPEED			( 0.5f )	// �㏸���x

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
