/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fgame.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _GAME_H_
#define _GAME_H_

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
class CScene;
class CCamera;
class CMeshField;
class CPlayer;
class CMode;
class CReticle;
class CPlayerLifeGauge;
class CScene2D;

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
// �Q�[���N���X
class CGame : public CMode
{
	private:
		CCamera *m_Camera;
		CLight *m_Light;
		CMeshField *m_MeshField;
		CPlayer *m_Player;
		CReticle *m_pReticle;
		CPlayerLifeGauge *m_pPlayerLifeGauge;
		CScene2D *m_pLogo;
		bool m_bPause;
		int m_State;
		int m_nStateCnt;
	public:
		typedef enum
		{
			STATE_START,
			STATE_NORMAL,
			STATE_BOSS,
			STATE_END,
			STATE_MAX
		}STATE;
		CGame();
		~CGame();
		void Init( void );
		void Uninit( void );
		void Update( void );
		void Draw( void );

		CCamera *GetCamera( void );
		CMeshField *GetMeshField( void );
		CPlayer *GetPlayer( void );
		CReticle *GetReticle(void) { return m_pReticle; }
		CPlayerLifeGauge *GetPlayerLifeGauge(void) { return m_pPlayerLifeGauge; }
		void SetPause(bool flg) { m_bPause = flg; }
		void StateUpdate(void);
		int GetState(void) { return m_State; }
		void SetState(int state) { m_State = state; m_nStateCnt = 0; }
};

#endif