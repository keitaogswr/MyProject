/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FmeshField.cpp
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
#include "game.h"
#include "scene.h"
#include "scene3D.h"
#include "billboard.h"
#include "treeObject.h"
#include "meshField.h"
#include "camera.h"
#include "input.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 150 )
#define HEIGHT			( 150 )
#define DEPTH			( 150 )
#define TEXTURE_ROW		( 1 )
#define TEXTURE_COLUMN	( 1 )
#define OPARATE_SPEED	( 3.0f )
#define TEXTFILENAME	( "data\\EDITDATA\\mapdata.txt" )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCMeshField::CMeshField()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CMeshField::CMeshField( DRAWORDER DrawOrder, OBJTYPE ObjType ):CScene3D( DrawOrder, ObjType )
{
	m_IdxBuff = NULL;

	m_fWidth = 0.0f;
	m_fDepth = 0.0f;
	m_nRowBlock = 0;
	m_nColumnBlock = 0;	
	m_nRowNumVtx = 0;
	m_nColumnNumVtx = 0;
	m_nNumVtxMax = 0;
	m_nNumIdx = 0;
	m_nNumPrim = 0;

	m_fWidthAll = 0.0f;
	m_fDepthAll = 0.0f;

	m_nCollNumVtx = 0;

	mesh = NULL;
}

/*******************************************************************************
* �֐����FCMeshField::~CMeshField()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CMeshField::~CMeshField()
{
}

/*******************************************************************************
* �֐����Fvoid CMeshField::Init( D3DXVECTOR3 pos, float width, float height, int row, int column )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CMeshField::Init( Vector3 pos, float width, float height, int row, int column )
{
	int nCntRow, nCntColumn, nIdxCnt;

	/* �ϐ���� */
	m_Pos = pos;
	m_nRowBlock = row;
	m_nColumnBlock = column;
	m_nRowNumVtx = m_nRowBlock + 1;
	m_nColumnNumVtx = m_nColumnBlock + 1;
	m_fWidth = width;
	m_fDepth = height;
	m_nNumVtxMax = m_nRowNumVtx * m_nColumnNumVtx;
	m_nNumIdx = ( ( m_nColumnBlock + 1 ) * m_nRowBlock * 2 + ( m_nRowBlock - 1 ) * 2 );
	m_nNumPrim = ( m_nColumnBlock * m_nRowBlock * 2 + ( m_nRowBlock - 1 ) * 4  );

	m_fWidthAll = width * column;
	m_fDepthAll = height * row;

	mesh = new Vector3[ m_nNumVtxMax ];

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].x = m_Pos.x -( m_fWidth * m_nColumnBlock * 0.5f ) + m_fWidth * nCntColumn;
			//mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].y = g_HeightMap[ nCntRow ][ nCntColumn ] * 10.0f;
			mesh[ nCntColumn + nCntRow * m_nRowNumVtx ].y = 0.0f;
			mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].z = m_Pos.z + ( m_fDepth * m_nRowBlock * 0.5f ) - m_fDepth * nCntRow;
		}
	}

	LoadFile();

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// �e�N�X�`���̓ǂݍ���
	m_pTexture = CTexture::GetTexture(TEXTURE_TYPE_FIELD_000);

	// ���_�o�b�t�@�̊m��
	if( FAILED( pDevice->CreateVertexBuffer(
							sizeof( VERTEX_3D ) * m_nNumVtxMax,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_VtxBuff,
							NULL ) ) )
	{
		return;
	}

	// �C���f�b�N�X�o�b�t�@�̊m��
	pDevice->CreateIndexBuffer( sizeof( WORD ) * m_nNumIdx,		// �m�ۂ���o�b�t�@�T�C�Y
								D3DUSAGE_WRITEONLY,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&m_IdxBuff,
								NULL );

	// ���_���ݒ�
	SetVertex();


	// �C���f�b�N�X���ݒ�
	WORD *pIdx;

	m_IdxBuff->Lock( 0, 0, ( void** )&pIdx, 0 );

	// �ϐ���`
	int x, y, cnt;

	for( y = 0, cnt = 0; y < m_nRowBlock; y++ )
	{
		for( x = 0; x < m_nColumnNumVtx; x++, cnt++ )
		{
			pIdx[ cnt * 2 + 0 ] = x + ( y + 1 ) * m_nColumnNumVtx;
			pIdx[ cnt * 2 + 1 ] = x + ( y + 0 ) * m_nColumnNumVtx;
			if( x == m_nColumnNumVtx - 1 && cnt * 2 < m_nNumIdx - 2 )
			{
				cnt++;
				pIdx[ cnt * 2 + 0 ] = x + ( y + 0 ) * m_nColumnNumVtx;
				pIdx[ cnt * 2 + 1 ] = ( y + 1 + 1 ) * m_nColumnNumVtx;
			}
		}
	}

	m_IdxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CMeshField::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CMeshField::Uninit( void )
{
	// �������
	SAFE_RELEASE( m_VtxBuff );
	m_pTexture = NULL;
	SAFE_RELEASE( m_IdxBuff );
	SAFE_DELETE( mesh );
}

/*******************************************************************************
* �֐����Fvoid CMeshField::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CMeshField::Update( void )
{
	if (CManager::GetOperateMode() == CManager::OPMODE_MESH)
	{// �t�B�[���h�G�f�B�^���[�h��
		// ���b�V������
		Operate();

		// �x�N�g��
		Vector3 vector;

		// �}�E�X�擾
		//MOUSESTATE *mouse = CInput::GetMouseState();
		//// �J�����擾
		//CGame *game = (CGame*)CManager::GetMode();
		//CCamera *camera = game->GetCamera();
		////  XZ���ʂƃX�N���[�����W�̌�_�Z�o
		//CManager::CalcScreenToXZ(&vector, mouse->localPos.x, mouse->localPos.y,
		//	SCREEN_WIDTH, SCREEN_HEIGHT, camera->GetMtxView(), camera->GetMtxProj());
		//// ���W�ݒ�
		//Collision(&vector);
		//CTreeObject::Set(vector, CTreeObject::m_nTexNum);

		//SetVertex();
		//SetColorVertex(m_nCollNumVtx, D3DCOLOR_RGBA(255, 0, 0, 255));
		//SetColorVertex(m_nCollNumVtx + 1, D3DCOLOR_RGBA(255, 0, 0, 255));
		//SetColorVertex(m_nCollNumVtx + m_nColumnNumVtx, D3DCOLOR_RGBA(255, 0, 0, 255));
		//SetColorVertex(m_nCollNumVtx + m_nColumnNumVtx + 1, D3DCOLOR_RGBA(255, 0, 0, 255));
	}

}
/*******************************************************************************
* �֐����Fvoid CMeshField::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CMeshField::Draw( void )
{
	/* �ϐ���` */
	D3DXMATRIX mtxScl, mtxRot, mtxTrans;	// �X�P�[���A�����A�|�W�V����

	// �f�o�C�X�̎擾
	CRenderer *renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity( &m_MtxWorld );

	// �X�P�[���𔽉f
	D3DXMatrixScaling( &mtxScl, m_Scl.x, m_Scl.y, m_Scl.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxScl );
	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll( &mtxRot, m_Rot.y, m_Rot.x, m_Rot.z );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxRot );
	// �ʒu�𔽉f
	D3DXMatrixTranslation( &mtxTrans, 0.0f, 0.0f, 0.0f );
	D3DXMatrixMultiply( &m_MtxWorld,
						&m_MtxWorld,
						&mtxTrans );
	
	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform( D3DTS_WORLD, &m_MtxWorld );

	// �X�g���[���Ƀo�C���h
	pDevice->SetStreamSource( 0, m_VtxBuff, 0, sizeof( VERTEX_3D ) );

	// �C���f�b�N�X�o�b�t�@���o�C���h
	pDevice->SetIndices( m_IdxBuff );

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF( FVF_VERTEX_3D );

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture( 0, *m_pTexture );

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLESTRIP,		// �v���~�e�B�u�̎��
					0,
					0,							
					m_nNumVtxMax,	// ���_��
					0,
					m_nNumPrim );	// �`�悷��v���~�e�B�u��
}

/*******************************************************************************
* �֐����FCMeshField *CMeshField::Create( Vector3 pos, float width, float height, int row, int column )
* 
* ����	�FD3DXVECTOR3 pos	// �|���S���̌��_�ʒu
*		  float width		// 1�u���b�N�̕�
*		  float height		// 1�u���b�N�̍���
*		  int row			// ��u���b�N��
*		  int column		// �s�u���b�N��
* �߂�l�F
* ����	�F�t�B�[���h�ݒ菈��
*******************************************************************************/
CMeshField *CMeshField::Create( Vector3 pos, float width, float height, int row, int column )
{
	CMeshField *meshField;
	meshField = new CMeshField;
	meshField->Init( pos, width, height, row, column );
	return meshField;
}

/*******************************************************************************
* �֐����Fint CMeshField::Collision( Vector3 vector )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h�����蔻�菈��
*******************************************************************************/
int CMeshField::Collision( Vector3 *vector )
{
	// �ϐ���`
	Vector3 posA, posB, posC;
	Vector3 VecAB, VecBC, VecCA, VecAP, VecBP, VecCP;
	Vector3 c1, c2, c3;
	Vector3 n1, n2;
	int columnIdx, rowIdx;
	int vtxIdx = 0;
	columnIdx = (int)(vector->x / m_fWidth + m_nColumnBlock * 0.5f);
	rowIdx = (int)(m_nRowBlock * 0.5f - vector->z / m_fDepth);
	vtxIdx = columnIdx + m_nRowNumVtx * rowIdx;
	if (vtxIdx < 0 || vtxIdx > m_nNumVtxMax)
	{
		vtxIdx = 0;
	}

	float f = 0;
	float height = 0;

	// �����蔻��
	Vector3 vec1 = mesh[vtxIdx] - mesh[vtxIdx + m_nColumnNumVtx + 1];
	Vector3 vec2 = *vector - mesh[vtxIdx + m_nColumnNumVtx + 1];
	float fNor1 = vec2.x * vec1.z - vec2.z * vec1.x;

	D3DXVec3Cross(&n1, &vec1, &vec2);
	D3DXVec3Normalize(&n1, &n1);

	if (fNor1 > 0)
	{
		// �����蔻��
		// 1�ʖ�
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + 1];
		posC = mesh[vtxIdx + m_nColumnNumVtx];

		VecAB = posB - posA;
		VecAP = *vector - posA;

		VecBC = posC - posB;
		VecBP = *vector - posB;

		VecCA = posA - posC;
		VecCP = *vector - posC;

		// �O��
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// �O�p�`�̓����ɓ_������
			m_nCollNumVtx = vtxIdx;

			return 0;
		}
	}
	else
	{
		// 2�ʖ�
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + m_nColumnNumVtx];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = *vector - posA;
		D3DXVec3Normalize(&VecAB, &VecAB);
		D3DXVec3Normalize(&VecAP, &VecAP);

		VecBC = posC - posB;
		VecBP = *vector - posB;
		D3DXVec3Normalize(&VecBC, &VecBC);
		D3DXVec3Normalize(&VecBP, &VecBP);

		VecCA = posA - posC;
		VecCP = *vector - posC;
		D3DXVec3Normalize(&VecCA, &VecCA);
		D3DXVec3Normalize(&VecCP, &VecCP);

		// �O��
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// �O�p�`�̓����ɓ_������
			m_nCollNumVtx = vtxIdx + m_nColumnNumVtx;

			return 0;
		}
	}

	
	return 1;
}

/*******************************************************************************
* �֐����Ffloat CMeshField::GetHeight( D3DXVECTOR3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h�����蔻�菈��
*******************************************************************************/
float CMeshField::GetHeight( Vector3 pos )
{
	// �ϐ���`
	Vector3 posA, posB, posC;
	Vector3 VecAB, VecBC, VecCA, VecAP, VecBP, VecCP;
	Vector3 c1, c2, c3;
	Vector3 n1, n2;
	int columnIdx, rowIdx;
	int vtxIdx;
	columnIdx = (int)(pos.x / m_fWidth + m_nColumnBlock * 0.5f);
	rowIdx = (int)(m_nRowBlock * 0.5f - pos.z / m_fDepth);
	vtxIdx = columnIdx + m_nRowNumVtx * rowIdx;

	float height = 0;

	// �����蔻��
	Vector3 vec1 = mesh[vtxIdx] - mesh[vtxIdx + m_nColumnNumVtx + 1];
	Vector3 vec2 = pos - mesh[vtxIdx + m_nColumnNumVtx + 1];
	float fNor1 = vec2.x * vec1.z - vec2.z * vec1.x;

	D3DXVec3Cross(&n1, &vec1, &vec2);
	D3DXVec3Normalize(&n1, &n1);

	if (fNor1 > 0)
	{
		// 1�ʖ�
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + 1];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = pos - posA;

		VecBC = posC - posB;
		VecBP = pos - posB;

		VecCA = posA - posC;
		VecCP = pos - posC;

		// �O��
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// �O�p�`�̓����ɓ_������
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			height = mesh[num].y - ((n1.x * (pos.x - mesh[num].x) + n1.z * (pos.z - mesh[num].z)) / n1.y);
		}
	}
	else
	{
		// 2�ʖ�
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + m_nColumnNumVtx];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = pos - posA;
		D3DXVec3Normalize(&VecAB, &VecAB);
		D3DXVec3Normalize(&VecAP, &VecAP);

		VecBC = posC - posB;
		VecBP = pos - posB;
		D3DXVec3Normalize(&VecBC, &VecBC);
		D3DXVec3Normalize(&VecBP, &VecBP);

		VecCA = posA - posC;
		VecCP = pos - posC;
		D3DXVec3Normalize(&VecCA, &VecCA);
		D3DXVec3Normalize(&VecCP, &VecCP);

		// �O��
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// �O�p�`�̓����ɓ_������
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			height = mesh[num].y - ((n1.x * (pos.x - mesh[num].x) + n1.z * (pos.z - mesh[num].z)) / n1.y);
		}
	}
	
	return height;
}

/*******************************************************************************
* �֐����Fvoid CMeshField::SetVertex( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h���_�ݒ菈��
*******************************************************************************/
void CMeshField::SetVertex( void )
{
	// ���_���ݒ�
	VERTEX_3D *pVtx;
	int nCntRow, nCntColumn, nIdxCnt;
	D3DXVECTOR3 n1, n2, n3, n4, n5, n6;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			pVtx[ nIdxCnt ].pos.x = mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].x;
			pVtx[ nIdxCnt ].pos.y = mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].y;
			pVtx[ nIdxCnt ].pos.z = mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].z;

			pVtx[ nIdxCnt ].nor = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			pVtx[ nIdxCnt ].col = D3DCOLOR_RGBA( 255, 255, 255, 255 );

			//�e�N�X�`���u���b�N�\��
			pVtx[ nIdxCnt ].tex.x = 1.0f * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = 1.0f * nCntRow;

			//�e�N�X�`���S�ʕ\��
			/*pVtx[ nIdxCnt ].tex.x = ( 1.0f / m_nColumnBlock ) * nCntColumn;
			pVtx[ nIdxCnt ].tex.y = ( 1.0f / m_nRowBlock ) * nCntRow;*/
		}
	}

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			// �@���ݒ� ///
			if( nCntRow == 0 )
			{
				if( nCntColumn == 0 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 ) );
				}
				else if( nCntColumn == m_nColumnNumVtx - 1 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt +  m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 ) );
				}
				else
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Cross( &n3,
									&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n3, &n3 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
				}
			}
			else if( nCntRow == m_nRowNumVtx - 1 )
			{
				if( nCntColumn == 0 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 ) );
				}
				else if( nCntColumn == m_nColumnNumVtx - 1 )
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 ) );
				}
				else
				{
					D3DXVec3Cross( &n1,
									&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n1, &n1 );
					D3DXVec3Cross( &n2,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n2, &n2 );
					D3DXVec3Cross( &n3,
									&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
									&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
					D3DXVec3Normalize( &n3, &n3 );
					D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
				}
			}
			else if( nCntColumn == 0 && nCntRow != 0 && nCntRow != m_nRowNumVtx - 1 )
			{
				D3DXVec3Cross( &n1,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n1, &n1 );
				D3DXVec3Cross( &n2,
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n2, &n2 );
				D3DXVec3Cross( &n3,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n3, &n3 );
				D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
			}
			else if( nCntColumn == m_nColumnNumVtx - 1 && nCntRow != 0 && nCntRow != m_nRowNumVtx - 1 )
			{
				D3DXVec3Cross( &n1,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n1, &n1 );
				D3DXVec3Cross( &n2,
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n2, &n2 );
				D3DXVec3Cross( &n3,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n3, &n3 );
				D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 ) );
			}
			else
			{
				D3DXVec3Cross( &n1,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n1, &n1 );
				D3DXVec3Cross( &n2,
								&( pVtx[ nIdxCnt - m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n2, &n2 );
				D3DXVec3Cross( &n3,
								&( pVtx[ nIdxCnt + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n3, &n3 );
				D3DXVec3Cross( &n4,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx + 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n4, &n4 );
				D3DXVec3Cross( &n5,
								&( pVtx[ nIdxCnt + m_nColumnNumVtx ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n5, &n5 );
				D3DXVec3Cross( &n6,
								&( pVtx[ nIdxCnt - 1 ].pos - pVtx[ nIdxCnt ].pos ),
								&( pVtx[ nIdxCnt - m_nColumnNumVtx - 1 ].pos - pVtx[ nIdxCnt ].pos ) );
				D3DXVec3Normalize( &n6, &n6 );

				D3DXVec3Normalize( &pVtx[ nIdxCnt ].nor, &( n1 + n2 + n3 + n4 + n5 + n6 ) );
			}
		}
	}

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CMeshField::SetColorVertex( int idx, D3DCOLOR col )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h���_�ݒ菈��
*******************************************************************************/
void CMeshField::SetColorVertex( int idx, D3DCOLOR col )
{
	// ���_���ݒ�
	VERTEX_3D *pVtx;

	m_VtxBuff->Lock( 0, 0, ( void** )&pVtx, 0 );

	pVtx[ idx ].col = col;

	m_VtxBuff->Unlock();
}

/*******************************************************************************
* �֐����Fvoid CMeshField::ResetPosition( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h���Z�b�g����
*******************************************************************************/
void CMeshField::ResetPosition( void )
{
	int nCntRow, nCntColumn, nIdxCnt;

	for( nCntRow = 0, nIdxCnt = 0; nCntRow < m_nRowNumVtx; nCntRow++ )
	{
		for( nCntColumn = 0; nCntColumn < m_nColumnNumVtx; nCntColumn++, nIdxCnt++ )
		{
			mesh[ nCntColumn + nCntRow * m_nColumnNumVtx ].y = 0.0f;
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CMeshField::Operate( void )
* 
* ����	�F
* �߂�l�F
* ����	�F���_���쏈��
*******************************************************************************/
void CMeshField::Operate( void )
{
	CInput *input = CManager::GetInput();
	if(input->GetKeyboardPress( DIK_F5 ) )
	{
		// ����������
		ResetPosition();
	}
	if(input->GetKeyboardPress( DIK_F7 ) )
	{
		// �����ۑ�
		SaveFile();
	}
	if(input->GetKeyboardPress( DIK_F10 ) )
	{
		// �������[�h
		LoadFile();
	}

	// �}�E�X�N���b�N
	/*if(input->GetMousePress( DIM_LEFT ) )
	{
		mesh[ m_nCollNumVtx ].y += OPARATE_SPEED;
		mesh[ m_nCollNumVtx + 1 ].y += OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx ].y += OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx + 1 ].y += OPARATE_SPEED;
	}
	if(input->GetMousePress( DIM_RIGHT ) )
	{
		mesh[ m_nCollNumVtx ].y -= OPARATE_SPEED;
		mesh[ m_nCollNumVtx + 1 ].y -= OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx ].y -= OPARATE_SPEED;
		mesh[ m_nCollNumVtx + m_nColumnNumVtx + 1 ].y -= OPARATE_SPEED;
	}*/
}

/*******************************************************************************
* �֐����Fvoid CMeshField::SaveFile( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�@�C���Z�[�u����
*******************************************************************************/
int CMeshField::SaveFile( void )
{
	FILE *fp;

	// �t�@�C���ǂݍ��݊J�n
	if ( ( fp = fopen( TEXTFILENAME, "w" ) ) == NULL)
	{
		return EXIT_FAILURE;
	}

	for( int i = 0; i < m_nNumVtxMax; i++ )
	{
		fprintf( fp, "POSY %f\n", mesh[ i ].y );
	}

	// �t�@�C���ǂݍ��ݏI��
	fclose( fp );

	return 0;
}

/*******************************************************************************
* �֐����Fvoid CMeshField::LoadFile( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�t�@�C�����[�h����
*******************************************************************************/
int CMeshField::LoadFile( void )
{
	FILE *fp;

	// �t�@�C���ǂݍ��݊J�n
	if ( ( fp = fopen( TEXTFILENAME, "r" ) ) == NULL)
	{
		return EXIT_FAILURE;
	}

	for( int i = 0; i < m_nNumVtxMax; i++ )
	{
		fscanf( fp, "POSY %f\n", &mesh[ i ].y );
	}

	// �t�@�C���ǂݍ��ݏI��
	fclose( fp );

	return 0;
}
/*******************************************************************************
* �֐����FVector3 CMeshField::GetNormal( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�@���擾����
*******************************************************************************/
Vector3 CMeshField::GetNormal( Vector3 *vector )
{
	// �ϐ���`
	Vector3 posA, posB, posC;
	Vector3 VecAB, VecBC, VecCA, VecAP, VecBP, VecCP;
	Vector3 c1, c2, c3;
	Vector3 n1, n2;
	int columnIdx, rowIdx;
	int vtxIdx;
	columnIdx = (int)(vector->x / m_fWidth + m_nColumnBlock * 0.5f);
	rowIdx = (int)(m_nRowBlock * 0.5f - vector->z / m_fDepth);
	vtxIdx = columnIdx + m_nRowNumVtx * rowIdx;

	float height = 0;

	// �����蔻��
	Vector3 vec1 = mesh[vtxIdx] - mesh[vtxIdx + m_nColumnNumVtx + 1];
	Vector3 vec2 = *vector - mesh[vtxIdx + m_nColumnNumVtx + 1];
	float fNor1 = vec2.x * vec1.z - vec2.z * vec1.x;

	D3DXVec3Cross(&n1, &vec1, &vec2);
	D3DXVec3Normalize(&n1, &n1);

	if (fNor1 > 0)
	{
		// 1�ʖ�
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + 1];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = *vector - posA;

		VecBC = posC - posB;
		VecBP = *vector - posB;

		VecCA = posA - posC;
		VecCP = *vector - posC;

		// �O��
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// �O�p�`�̓����ɓ_������
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			return n1;
		}
	}
	else
	{
		// 2�ʖ�
		posA = mesh[vtxIdx];
		posB = mesh[vtxIdx + m_nColumnNumVtx];
		posC = mesh[vtxIdx + m_nColumnNumVtx + 1];

		VecAB = posB - posA;
		VecAP = *vector - posA;
		D3DXVec3Normalize(&VecAB, &VecAB);
		D3DXVec3Normalize(&VecAP, &VecAP);

		VecBC = posC - posB;
		VecBP = *vector - posB;
		D3DXVec3Normalize(&VecBC, &VecBC);
		D3DXVec3Normalize(&VecBP, &VecBP);

		VecCA = posA - posC;
		VecCP = *vector - posC;
		D3DXVec3Normalize(&VecCA, &VecCA);
		D3DXVec3Normalize(&VecCP, &VecCP);

		// �O��
		D3DXVec3Cross(&c1, &VecAB, &VecBP);
		D3DXVec3Cross(&c2, &VecBC, &VecCP);
		D3DXVec3Cross(&c3, &VecCA, &VecAP);

		D3DXVec3Normalize(&c1, &c1);
		D3DXVec3Normalize(&c2, &c2);
		D3DXVec3Normalize(&c3, &c3);

		if ((c1.y > 0 && c2.y > 0 && c3.y > 0) || (c1.y < 0 && c2.y < 0 && c3.y < 0))
		{// �O�p�`�̓����ɓ_������
			int num = vtxIdx;

			D3DXVec3Cross(&n1,
				&(mesh[vtxIdx + m_nColumnNumVtx + 1] - mesh[vtxIdx]),
				&(mesh[vtxIdx + m_nColumnNumVtx] - mesh[vtxIdx]));
			D3DXVec3Normalize(&n1, &n1);
			return n1;
		}
	}
	
	return n1;
}

/*******************************************************************************
* �֐����Ffloat CMeshField::GetWidthAll( void )
*
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h�S�̂̕��擾����
*******************************************************************************/
float CMeshField::GetWidthAll(void)
{
	return m_fWidthAll;
}

/*******************************************************************************
* �֐����Ffloat CMeshField::GetDepthAll( void )
*
* ����	�F
* �߂�l�F
* ����	�F�t�B�[���h�S�̂̉��s�擾����
*******************************************************************************/
float CMeshField::GetDepthAll(void)
{
	return m_fDepthAll;
}