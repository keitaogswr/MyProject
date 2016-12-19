/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fpause.h
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
const int PAUSE_LOGO_MAX = 3;
/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CScene2D;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
// �|�[�Y�N���X
class CPause
{
private:
	static LPDIRECT3DVERTEXBUFFER9 m_VtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	static float m_fWidth;							// �|���S����
	static float m_fHeight;							// �|���S������
	static D3DXCOLOR m_Col;							// �F
	static CScene2D *m_pLogo[PAUSE_LOGO_MAX];
	static int m_nCurrent;
public:
	CPause();
	~CPause();
	static void Init(void);
	static void Uninit(void);
	static void Update(void);
	static void Draw(void);
};
