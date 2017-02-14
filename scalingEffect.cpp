/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FscalingEffect.cpp
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
#include "texture.h"
#include "scalingEffect.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 30 )		// �|���S����
#define HEIGHT			( 30 )		// �|���S������
#define TEXTURE_ROW		( 1 )		// �e�N�X�`���񕪊���
#define TEXTURE_COLUMN	( 1 )		// �e�N�X�`���s������
#define MOVE_SPEED		( 0.5f )	// �ړ����x
#define LIFE_MAX		( 1000 )	// ����
#define ATTEN_DEF		( 0.0001f )	// ���������W��
#define ATTEN_RISE		( 2.0f )	// �����W���̏㏸�{��

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCScalingEffect::CScalingEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEffect(DrawOrder, ObjType)
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CScalingEffect::CScalingEffect(DRAWORDER DrawOrder, OBJTYPE ObjType) :CEffect(DrawOrder, ObjType)
{
	m_fScalingValue = 0;
	m_Vec = Vector3(0.0f, 0.0f, 0.0f);
	m_PosOffset = Vector3(0.0f, 0.0f, 0.0f);
	m_pParentMtx = NULL;
}

/*******************************************************************************
* �֐����FCScalingEffect::~CScalingEffect()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CScalingEffect::~CScalingEffect()
{
}

/*******************************************************************************
* �֐����Fvoid CScalingEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CScalingEffect::Init(Vector3 pos, D3DXCOLOR col, float width, float height)
{
	m_PosOffset = pos;
	CEffect::Init(pos, col, width, height);
}

/*******************************************************************************
* �֐����Fvoid CScalingEffect::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CScalingEffect::Update(void)
{
	// ���������炷
	m_nLife--;

	// �|���S���̑傫���X�V
	m_fHeight += m_fScalingValue;
	m_fWidth += m_fScalingValue;

	// ���W�v�Z
	m_PosOffset += m_Vec * MOVE_SPEED;

	// ���W�ϊ�
	if (m_pParentMtx)
	{
		D3DXVec3TransformCoord(&m_Pos, &m_PosOffset, m_pParentMtx);
	}

	// ���_�ݒ�
	SetVertex();

	// �����`�F�b�N
	if (m_nLife <= 0 || m_fHeight < 0.1f || m_fWidth < 0.1f)
	{// �������s������j��
		SetDeleteFlg(true);
	}
}

/*******************************************************************************
* �֐����FCScalingEffect *CScalingEffect::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col, float width, float height, float scale, int life)
*
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CScalingEffect *CScalingEffect::Create(Vector3 pos, Vector3 vec, D3DXCOLOR col, float width, float height, float scale, int life, D3DXMATRIX *matrix)
{
	CScalingEffect *effect;
	effect = new CScalingEffect;

	effect->Init(pos, col, width, height);
	effect->SetLife(life);
	effect->SetScalingValue(scale);
	effect->SetVec(vec);
	effect->SetParentMtx(matrix);

	return effect;
}

/*******************************************************************************
* �֐����Fvoid CScalingEffect::SetWorldMatrix( void )
*
* ����	�F
* �߂�l�F
* ����	�F���[���h�}�g���b�N�X�ݒ菈��
*******************************************************************************/
void CScalingEffect::SetWorldMatrix(void)
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����
	D3DXMATRIX mtxView;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_MtxWorld);

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �r���[�}�g���b�N�X�̋t�s������߂�
	D3DXMatrixInverse(&m_MtxWorld,
		NULL,
		&mtxView);

	// �}�g���N�X�̈ړ��Ɋւ���ϐ��N���A
	m_MtxWorld._41 = 0.0f;
	m_MtxWorld._42 = 0.0f;
	m_MtxWorld._43 = 0.0f;

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, m_Scl.x, m_Scl.y, m_Scl.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxScl);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	D3DXMatrixMultiply(&m_MtxWorld,
		&m_MtxWorld,
		&mtxTrans);
}