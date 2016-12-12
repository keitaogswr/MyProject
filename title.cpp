/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Ftitle.cpp
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
#include "input.h"
#include "manager.h"
#include "mode.h"
#include "fade.h"
#include "game.h"
#include "result.h"
#include "title.h"
#include "load.h"
#include "scene.h"
#include "scene2D.h"
#include "bg.h"
#include "logo.h"
#include "scene3D.h"
#include "sceneX.h"
#include "motionManager.h"
#include "dynamicModel.h"
#include "light.h"
#include "camera.h"
#include "titleCamera.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCTitle::CTitle()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CTitle::CTitle()
{
	m_bMultithread = false;
}

/*******************************************************************************
* �֐����FCTitle::~CTitle()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CTitle::~CTitle()
{
}

/*******************************************************************************
* �֐����Fvoid CTitle::Init( void )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CTitle::Init(void)
{
	CMotionManager::Load();
	CScene *scene;
	scene = CBg::Create(Vector3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), TEXTURE_TYPE_TITLE);
	scene = CLogo::Create(Vector3(SCREEN_WIDTH / 2, 300.0f, 0.0f), 0.0f, TEXTURE_TYPE_TITLE_LOGO, 600.0f, 90.0f, false);
	scene = CLogo::Create(Vector3(SCREEN_WIDTH / 2, 500.0f, 0.0f), 0.0f, TEXTURE_TYPE_PRESS_START, 500.0f, 90.0f, true);
	scene = CDynamicModel::Create(Vector3(0.0f, 0.0f, 0.0f));
	// �J��������
	m_Camera = CTitleCamera::Create();
	m_Light = CLight::Create();
}

/*******************************************************************************
* �֐����Fvoid CTitle::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CTitle::Uninit(void)
{
	CScene::UninitAll();
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	// ���\�[�X���
	CMotionManager::Unload();
}

/*******************************************************************************
* �֐����Fvoid CTitle::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CTitle::Update(void)
{
	m_Camera->Update();
	CScene::UpdateAll();
	if (CInput::GetKeyboardTrigger(DIK_RETURN))
	{
		CFade::Start(new CLoad);
	}

}

/*******************************************************************************
* �֐����Fvoid CTitle::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CTitle::Draw(void)
{
	m_Camera->Set();
	CScene::DrawAll();
}