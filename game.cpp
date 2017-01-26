/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fgame.cpp
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
#include "fade.h"
#include "title.h"
#include "result.h"
#include "pause.h"
#include "sound.h"
#include "input.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "scene2D.h"
#include "scene3D.h"
#include "sceneX.h"
#include "motionManager.h"
#include "dynamicModel.h"
#include "player.h"
#include "playerT.h"
#include "enemy.h"
#include "bossEnemy.h"
#include "meshField.h"
#include "skyDome.h"
#include "skySphere.h"
#include "field.h"
#include "billboard.h"
#include "treeObject.h"
#include "time.h"
#include "game.h"
#include "bullet.h"
#include "shadow.h"
#include "effect.h"
#include "reticle.h"
#include "barrier.h"
#include "playerLifeGauge.h"
#include "texture.h"
#include "sound.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
const int START_CNT = 180;
const int END_CNT = 180;

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCGame::CGame()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CGame::CGame()
{
	m_bMultithread = true;
	m_Camera = NULL;
	m_Light = NULL;
	m_MeshField = NULL;
	m_Player = NULL;
	m_pReticle = NULL;
	m_pPlayerLifeGauge = NULL;
	m_pLogo = NULL;
	m_bPause = false;
	m_State = STATE_START;
	m_nStateCnt = 0;
}

/*******************************************************************************
* �֐����FCGame::~CGame()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CGame::~CGame()
{
}

/*******************************************************************************
* �֐����Fvoid CGame::Init( void )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CGame::Init(void)
{
	CScene *scene;
	// ��������
	srand((unsigned)time(NULL));

	// ���[�V�����f�[�^�ǂݍ���
	CMotionManager::Load();

	// �e�I�u�W�F�N�g��������
	//scene = CField::Create( Vector3( 0.0f, 0.0f, 0.0f ) );
	m_MeshField = CMeshField::Create(Vector3(0.0f, 0.0f, 0.0f), 100, 100, 64, 64);
	scene = CSkySphere::Create(Vector3(0.0f, 0.0f, 0.0f), 7000.0f, 16, 16);
	m_Player = CPlayer::Create(Vector3(10.0f, 1000.0f, 100.0f));

	CEnemy::Load();

	m_pReticle = CReticle::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f));
	scene = CTime::Create(Vector3(SCREEN_WIDTH / 2, 50.0f, 0.0f));
	m_pPlayerLifeGauge = CPlayerLifeGauge::Create(Vector3(300.0f, 50.0f, 0.0f));
	CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), SCREEN_WIDTH, SCREEN_HEIGHT, TEXTURE_TYPE_GAMEUI_000);
	m_pLogo = CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), 600.0f, 200.0f, TEXTURE_TYPE_GAME_START_000);
	// �J��������
	m_Camera = CCamera::Create();
	// ���C�g����
	m_Light = CLight::Create();

	// ���\�[�X�ǂݍ���
	CTreeObject::LoadFile();

	CPause::Init();
	// �T�E���h�Đ�
	CSound::Play(CSound::SOUND_LABEL_BGM_001);
}

/*******************************************************************************
* �֐����Fvoid CGame::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CGame::Uninit(void)
{
	// ���\�[�X���
	CPause::Uninit();
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	CScene::UninitAll();
	m_Player = NULL;
	m_MeshField = NULL;
	m_pReticle = NULL;
	m_pPlayerLifeGauge = NULL;
	m_pLogo = NULL;
	CMotionManager::Unload();

	CSound::Stop();
}

/*******************************************************************************
* �֐����Fvoid CGame::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CGame::Update(void)
{
	StateUpdate();
	if (CInput::GetKeyboardTrigger(DIK_P))
	{
		m_bPause = m_bPause == true ? false : true;
	}
	if (!m_bPause)
	{
		m_Camera->Update();

		CScene::UpdateAll();
#ifdef _DEBUG
		if (CInput::GetKeyboardTrigger(DIK_RETURN))
		{
			CFade::Start(new CResult);
		}
		// �G�̏���
		CScene *scene = CScene::GetList(DRAWORDER_3D);
		CScene *next = NULL;
		if (CInput::GetKeyboardTrigger(DIK_DELETE))
		{
			while (scene != NULL)
			{
				next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
				if (scene->GetObjType() == OBJTYPE_ENEMY)
				{
					scene->SetDeleteFlg(true);
				}
				scene = next;
			}
		}
#endif
	}
	else
	{
		CPause::Update();
	}
}

/*******************************************************************************
* �֐����Fvoid CGame::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CGame::Draw(void)
{
	m_Camera->Set();
	CScene::DrawAll();
	if (m_bPause)
	{
		CPause::Draw();
	}
}

/*******************************************************************************
* �֐����FCCamera *CGame::GetCamera( void )
*
* ����	�F
* �߂�l�F
* ����	�F�J�����擾����
*******************************************************************************/
CCamera *CGame::GetCamera(void)
{
	return m_Camera;
}

/*******************************************************************************
* �֐����FCMeshField *CGame::GetMeshField( void )
*
* ����	�F
* �߂�l�F
* ����	�F���b�V���t�B�[���h�擾����
*******************************************************************************/
CMeshField *CGame::GetMeshField(void)
{
	return m_MeshField;
}

/*******************************************************************************
* �֐����FCPlayer *CGame::GetPlayer( void )
*
* ����	�F
* �߂�l�F
* ����	�F�v���C���[�擾����
*******************************************************************************/
CPlayer *CGame::GetPlayer(void)
{
	return m_Player;
}

/*******************************************************************************
* �֐����Fvoid CGame::StateUpdate(void)
*
* ����	�F
* �߂�l�F
* ����	�F�Q�[����ԑJ�ڏ���
*******************************************************************************/
void CGame::StateUpdate(void)
{
	int cnt = 0;
	CScene *scene = CScene::GetList(DRAWORDER_3D);
	CScene *next = NULL;
	switch (m_State)
	{
	case STATE_START:
		m_nStateCnt++;
		if (m_nStateCnt > START_CNT)
		{
			SetState(STATE_NORMAL);
			m_pLogo->SetDeleteFlg(true);
		}
		break;
	case STATE_NORMAL:
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
			if (scene->GetObjType() == OBJTYPE_ENEMY)
			{
				cnt++;
			}
			scene = next;
		}
		if (cnt == 0)
		{
			SetState(STATE_BOSS);
			CBossEnemy::Create(Vector3(0.0f, 1000.0f, 0.0f));
		}
		break;
	case STATE_BOSS:

		break;
	case STATE_END:
		if (m_nStateCnt == 0)
		{
			m_pLogo = CScene2D::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), 600.0f, 200.0f, TEXTURE_TYPE_FINISH_000);
		}
		m_nStateCnt++;
		if (m_nStateCnt > END_CNT)
		{
			CFade::Start(new CResult);
		}
		break;
	default:
		break;
	}
}