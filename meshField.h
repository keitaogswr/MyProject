/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FmeshField.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CScene3D;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CMeshField : public CScene3D
{
	private:
		LPDIRECT3DINDEXBUFFER9	m_IdxBuff;		// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^

		float m_fWidth;							// �u���b�N��
		float m_fDepth;							// �u���b�N���s
		int m_nRowBlock;						// �s�u���b�N��
		int m_nColumnBlock;						// ��u���b�N��
		int m_nRowNumVtx;						// �s���_��
		int m_nColumnNumVtx;					// �񒸓_��
		int m_nNumVtxMax;						// ���_�ő吔
		int m_nNumIdx;							// �C���f�b�N�X�ő吔
		int m_nNumPrim;							// �v���~�e�B�u�ő吔
		
		float m_fWidthAll;						// �t�B�[���h�S�̂̕�
		float m_fDepthAll;						// �t�B�[���h�S�̂̉��s

		int m_nCollNumVtx;						// �����蔻��̂��������b�V���̋N�_���_�ԍ�

		void SetVertex( void );
		void SetColorVertex( int idx, D3DCOLOR col );
		void ResetPosition( void );
		void Operate( void );

		int SaveFile( void );
		int LoadFile( void );

		Vector3 *mesh;							// ���b�V�����
	public:
		CMeshField( DRAWORDER DrawOrder = DRAWORDER_BACK, OBJTYPE ObjType = OBJTYPE_MESHFIELD );
		~CMeshField();
		void Init( Vector3 pos, float width, float height, int row, int column );
		void Uninit( void );
		void Update( void );
		void Draw( void );

		static CMeshField *Create( Vector3 pos, float width, float height, int row, int column );
		
		int Collision( Vector3 *vector );
		float GetHeight( Vector3 pos );
		Vector3 GetNormal( Vector3 *vector );
		float GetWidthAll(void);
		float GetDepthAll(void);
};

#endif