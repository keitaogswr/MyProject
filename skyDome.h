/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FskyDome.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _SKYDOME_H_
#define _SKYDOME_H_

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

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CSkyDome : public CScene3D
{
protected:
	typedef struct
	{
		LPDIRECT3DVERTEXBUFFER9	pVtxBuff;			// ���_�o�b�t�@�ւ̃|�C���^
		LPDIRECT3DINDEXBUFFER9	pIdxBuff;			// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
		int nNumVtxMax;								// ���_�ő吔
		int nNumIdx;								// �C���f�b�N�X�ő吔
		int nNumPrim;								// �v���~�e�B�u�ő吔
	}MESHFAN;
	typedef struct
	{
		LPDIRECT3DVERTEXBUFFER9	pVtxBuff;			// ���_�o�b�t�@�ւ̃|�C���^
		LPDIRECT3DINDEXBUFFER9	pIdxBuff;			// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
		float fRadius;								// �h�[�����a
		int nRowBlock;								// �s�u���b�N��
		int nColumnBlock;							// ��u���b�N��
		int nRowNumVtx;								// �s���_��
		int nColumnNumVtx;							// �񒸓_��
		int nNumVtxMax;								// ���_�ő吔
		int nNumIdx;								// �C���f�b�N�X�ő吔
		int nNumPrim;								// �v���~�e�B�u�ő吔
	}MESHDOME;

	MESHFAN m_MeshFan;
	MESHDOME m_MeshDome;
public:
	CSkyDome(DRAWORDER DrawOrder = DRAWORDER_BACK, OBJTYPE ObjType = OBJTYPE_NONE);
	~CSkyDome();
	void Init(Vector3 pos, float rad, int row, int column);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CSkyDome *Create(Vector3 pos, float rad, int row, int column);
};

#endif