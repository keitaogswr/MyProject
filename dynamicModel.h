/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FdynamicModel.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _DYNAMICMODEL_H_
#define _DYNAMICMODEL_H_

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
class CSceneX;
class CMotionManager;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CDynamicModel : public CSceneX
{
	protected:
		Vector3	m_Move;					// �ړ���
		Vector3	m_Rad;					// �ړI�Ǝ����̌����̍���
		Vector3	m_RotN;					// �ړI�̌���
		D3DXQUATERNION m_Quaternion;	// ���݂̃N�H�[�^�j�I��
		D3DXQUATERNION m_QuaterN;		// �ړI�̃N�H�[�^�j�I��

		CMotionManager *m_MotionManager;

		void SetModelMatrix(void);
	public:
		CDynamicModel( DRAWORDER DrawOrder = DRAWORDER_3D, OBJTYPE ObjType = OBJTYPE_NONE );
		~CDynamicModel();
		virtual void Init( Vector3 pos );
		virtual void Uninit( void );
		virtual void Update( void );
		virtual void Draw( void );

		static void Load( void ) {}
		static void Unload( void ) {}
		static CDynamicModel *Create( Vector3 pos );
		Vector3 GetMove( void ){ return m_Move; }
		Vector3 GetRotN( void ){ return m_RotN; }
};

#endif