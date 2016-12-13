/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Feditor.cpp
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
#include "input.h"
#include "game.h"
#include "title.h"
#include "result.h"
#include "editor.h"
#include "camera.h"
#include "editorCamera.h"
#include "light.h"
#include "scene.h"
#include "scene2D.h"
#include "scene3D.h"
#include "bg.h"
#include "skyDome.h"
#include "skySphere.h"
#include "meshField.h"
#include "texture.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCEditor::CEditor()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CEditor::CEditor()
{
	m_bMultithread = false;
	m_Camera = NULL;
	m_Light = NULL;
	m_MeshField = NULL;
	CManager::SetOperateMode(CManager::OPMODE_MESH);
}

/*******************************************************************************
* �֐����FCEditor::~CEditor()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CEditor::~CEditor()
{
}

/*******************************************************************************
* �֐����Fvoid CEditor::Init( void )
*
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CEditor::Init(void)
{
	CScene *scene;
	m_MeshField = CMeshField::Create(Vector3(0.0f, 0.0f, 0.0f), 100, 100, 64, 64);
	scene = CSkySphere::Create(Vector3(0.0f, 0.0f, 0.0f), 7000.0f, 8, 16);

	// �J��������
	m_Camera = CEditorCamera::Create();
	// ���C�g����
	m_Light = CLight::Create();
}

/*******************************************************************************
* �֐����Fvoid CEditor::Uninit( void )
*
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CEditor::Uninit(void)
{
	// ���\�[�X���
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	CScene::UninitAll();

	m_MeshField = NULL;
}

/*******************************************************************************
* �֐����Fvoid CEditor::Update( void )
*
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CEditor::Update(void)
{
	m_Camera->Update();
	CScene::UpdateAll();
}

/*******************************************************************************
* �֐����Fvoid CEditor::Draw( void )
*
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CEditor::Draw(void)
{
	m_Camera->Set();
	CScene::DrawAll();
}