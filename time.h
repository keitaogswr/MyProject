/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Ftime.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _TIME_H_
#define _TIME_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define TIMER_MAX	( 3 )

/*******************************************************************************
* �\����
*******************************************************************************/

/*******************************************************************************
* �O���錾
*******************************************************************************/
class CNumber;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CTime : public CScene
{
	private:

	public:
		CTime( DRAWORDER DrawOrder = DRAWORDER_2D );
		~CTime();

		void Init( Vector3 pos );
		void Uninit( void );
		void Update( void );
		void Draw( void );

		CNumber *m_Number[ TIMER_MAX ];
		int m_nTime;
		int m_nTimeCnt;

		static CTime *Create( Vector3 pos );
		int GetTime(void) { return m_nTime; }
};

#endif