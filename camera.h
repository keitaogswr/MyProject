/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fcamera.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\����
*******************************************************************************/
typedef enum
{
	CAMERAMODE_SNEAK,
	CAMERAMODE_ROCKON,
	CAMERAMODE_MAX
}CAMERAMODE;

/*******************************************************************************
* �O���錾
*******************************************************************************/

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
// �J�����N���X
class CCamera
{
	protected:
		Vector3 m_Pos;					// ���_�ʒu
		Vector3 m_PosR;					// �����_�̈ʒu
		Vector3 m_VecU;					// ������x�N�g��
		Vector3 m_MoveV;				// ���_�ړ�����
		Vector3 m_MoveR;				// �����_�ړ�����
		D3DXMATRIX	m_MtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
		D3DXMATRIX	m_MtxView;			// �r���[�}�g���b�N�X
		
		Vector3 m_Rad;					// �p�x����
		Vector3 m_RotN;					// ���̌���

		bool m_bShake;					// ��ʂ�h�炷���ǂ���
		int m_nCnt;						// �U���J�E���^
		float m_fAmplitude;				// �U����
		int m_nShakeTime;				// �U������
		int m_nMode;					// �J�����̃��[�h

		void Operate(void);
		void OperateJoyStick(void);
	public:
		CCamera();
		~CCamera();
		virtual void Init( void );
		virtual void Uninit( void );
		virtual void Update( void );
		void Draw( void );
		virtual void Set( void );

		static CCamera *Create( void );

		Vector3 GetPosition( void );
		D3DXMATRIX *GetMtxProj( void );
		D3DXMATRIX *GetMtxView( void );
		D3DXVECTOR3 GetVecRV( void );

		void SetShake(float amplitudeint, int time);
		void SetCameraMode(int id);
		void UpdateMode(void);

		Vector3 m_Rot;
};

#endif