/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene2D.cpp
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
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCScene2D::CScene2D()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CScene2D::CScene2D(DRAWORDER DrawOrder, OBJTYPE ObjType) :CScene(DrawOrder, ObjType)
{
	m_fHeight = 0.0f;
	m_fWidth = 0.0f;
	pFileName = NULL;
	m_fLength = 0.0f;
	m_fAngle = 0.0f;

	m_Col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����FCScene2D::~CScene2D()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CScene2D::~CScene2D()
{
}

/*******************************************************************************
* �֐����Fvoid CScene2D::Init(Vector3 pos, float width, float height, int texId)
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CScene2D::Init(Vector3 pos, float width, float height, int texId)
{
	m_Pos = pos;

	m_fWidth = width;
	m_fHeight = height;

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(texId);

	// ���_�o�b�t�@�̐���
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_2D ) * VERTEX_MAX,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_2D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	m_fLength = sqrtf( m_fWidth * m_fWidth + m_fHeight * m_fHeight) * 0.5f;
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// ���_�o�b�t�@�̐ݒ�
	VERTEX_2D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ 0 ].pos = D3DXVECTOR3( m_Pos.x + sinf( D3DX_PI + m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( D3DX_PI + m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 1 ].pos = D3DXVECTOR3( m_Pos.x + sinf( D3DX_PI - m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( D3DX_PI - m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 2 ].pos = D3DXVECTOR3( m_Pos.x + sinf( -m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( -m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );
	pVtx[ 3 ].pos = D3DXVECTOR3( m_Pos.x + sinf( m_fAngle + m_Rot.z ) * m_fLength,
								 m_Pos.y + cosf( m_fAngle + m_Rot.z ) * m_fLength,
								 0.0f );

	pVtx[ 0 ].rhw =
	pVtx[ 1 ].rhw =
	pVtx[ 2 ].rhw =
	pVtx[ 3 ].rhw = 1.0f;

	pVtx[0].col =
	pVtx[1].col =
	pVtx[2].col =
	pVtx[3].col = m_Col;

	pVtx[ 0 ].tex = D3DXVECTOR2( 0.0f, 0.0f );
	pVtx[ 1 ].tex = D3DXVECTOR2( 1.0f / TEXTURE_ROW, 0.0f );
	pVtx[ 2 ].tex = D3DXVECTOR2( 0.0f, 1.0f / TEXTURE_COLUMN );
	pVtx[ 3 ].tex = D3DXVECTOR2( 1.0f / TEXTURE_ROW, 1.0f / TEXTURE_COLUMN );

	m_VtxBuff->Unlock();

}

/*******************************************************************************
* �֐����Fvoid CScene2D::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CScene2D::Uninit( void )
{
	// ���
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
}

/*******************************************************************************
* �֐����Fvoid CScene2D::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CScene2D::Update( void )
{
	SetVertex();
}

/*******************************************************************************
* �֐����Fvoid CScene2D::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CScene2D::Draw( void )
{
	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_2D ) );

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_2D );

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, *m_pTexture );

	// �|���S���̕`��
	pDevice->DrawPrimitive(
					D3DPT_TRIANGLESTRIP,		//�v���~�e�B�u�̎��
					0,							//�ŏ��̒��_�̃C���f�b�N�X
					2 );						//�`�悷��v���~�e�B�u��
}

/*******************************************************************************
* �֐����FCScene2D *CScene2D::Create(Vector3 pos, float width, float height, int texId)
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CScene2D *CScene2D::Create(Vector3 pos, float width, float height, int texId)
{
	CScene2D *scene2D;
	scene2D = new CScene2D;
	scene2D->Init(pos, width, height, texId);
	return scene2D;
}

/*******************************************************************************
* �֐����Fvoid CScene2D::SetVertex(void)
*
* ����	�F
* �߂�l�F
* ����	�F���_�ݒ菈��
*******************************************************************************/
void CScene2D::SetVertex(void)
{
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