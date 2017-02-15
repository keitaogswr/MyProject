/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fscene.cpp
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
#include "list.h"
#include "renderer.h"
#include "scene.h"

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
CScene *CScene::m_Top[DRAWORDER_MAX] = { NULL };
CScene *CScene::m_Cur[DRAWORDER_MAX] = { NULL };

/*******************************************************************************
* �֐����FCScene::CScene()
*
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CScene::CScene(DRAWORDER DrawOrder, OBJTYPE ObjType)
{
	if (m_Top[DrawOrder] == NULL)
	{// �擪�ɉ��������Ă��Ȃ�������
		m_Top[DrawOrder] = this;			// �擪�Ɏ����̃|�C���^����
		m_Cur[DrawOrder] = this;			// ���݂Ɏ����̃|�C���^����
		m_Prev = NULL;						// �����̑O�̃|�C���^��NULL��ݒ�
		m_Next = NULL;						// �����̎��̃|�C���^��NULL��ݒ�
	}
	else
	{// �擪�����܂��Ă�����
		m_Cur[DrawOrder]->m_Next = this;	// ���݂̎��̃|�C���^�Ɏ����̃|�C���^����
		m_Prev = m_Cur[DrawOrder];			// �����̑O�̃|�C���^�Ɍ��݂̃|�C���^����
		m_Cur[DrawOrder] = this;			// ���݂Ɏ����̃|�C���^����
		m_Next = NULL;						// �����̎��̃|�C���^��NULL��ݒ�
	}

	m_Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_Rot = Vector3(0.0f, 0.0f, 0.0f);
	m_Delete = false;
	m_bPause = false;
	m_bDraw = true;
	SetObjType(ObjType);
	SetDrawOrder(DrawOrder);
}

/*******************************************************************************
* �֐����FCScene::~CScene()
*
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CScene::~CScene()
{
}

/*******************************************************************************
* �֐����Fvoid CScene::UninitAll( void )
*
* ����	�F
* �߂�l�F
* ����	�F�S�I������
*******************************************************************************/
void CScene::UninitAll(void)
{
	for (int i = 0; i < DRAWORDER_MAX; i++)
	{
		CScene *scene = m_Top[i];
		CScene *next = NULL;
		while (scene != NULL)
		{
			next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
			scene->Uninit();
			delete scene;
			scene = next;
		}
		m_Top[i] = NULL;
	}
}

/*******************************************************************************
* �֐����Fvoid CScene::UpdateAll( void )
*
* ����	�F
* �߂�l�F
* ����	�F�S�X�V����
*******************************************************************************/
void CScene::UpdateAll(void)
{
	for (int i = 0; i < DRAWORDER_MAX; i++)
	{
		CScene *scene = m_Top[i];
		CScene *next;

		while (scene != NULL)
		{
			next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
			if (!scene->m_Delete && !scene->m_bPause)
			{
				scene->Update();
			}
			scene = next;
		}

		scene = m_Top[i];

		while (scene != NULL)
		{
			next = scene->m_Next;	// delete���̃��������[�N����̂��߂Ƀ|�C���^���i�[
			if (scene->m_Delete)
			{
				scene->Release(scene->m_DrawOrder);
			}
			scene = next;
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CScene::DrawAll( void )
*
* ����	�F
* �߂�l�F
* ����	�F�S�`�揈��
*******************************************************************************/
void CScene::DrawAll(void)
{
	for (int i = 0; i < DRAWORDER_MAX; i++)
	{
		CScene *scene = m_Top[i];
		while (scene != NULL)
		{
			if (scene->m_bDraw)
			{
				scene->Draw();
			}
			scene = scene->m_Next;
		}
	}
}

/*******************************************************************************
* �֐����Fvoid CScene::SetPosition( Vector3 pos )
*
* ����	�F
* �߂�l�F
* ����	�F�ʒu�ݒ菈��
*******************************************************************************/
void CScene::SetPosition(Vector3 pos)
{
	m_Pos = pos;
}

/*******************************************************************************
* �֐����Fvoid CScene::Release( DRAWORDER DrawOrder )
*
* ����	�F
* �߂�l�F
* ����	�F�������
*******************************************************************************/
void CScene::Release(DRAWORDER DrawOrder)
{
	if (this == m_Top[DrawOrder])
	{// �������擪�̂Ƃ�
		m_Top[DrawOrder] = this->m_Next;		// �擪�������̎��̃|�C���^�ɐݒ�
	}
	else
	{// �������擪�ł͂Ȃ��Ƃ�
		this->m_Prev->m_Next = this->m_Next;	// �����̎��̃|�C���^��ݒ�
	}

	if (this == m_Cur[DrawOrder])
	{// �����������̂Ƃ�
		m_Cur[DrawOrder] = this->m_Prev;		// �擪�������̎��̃|�C���^�ɐݒ�
	}
	else
	{// �����������ł͂Ȃ��Ƃ�
		this->m_Next->m_Prev = this->m_Prev;	// �����̑O�̃|�C���^��ݒ�
	}

	this->Uninit();
	delete this;
}

/*******************************************************************************
* �֐����FCScene *CScene::GetList( void )
*
* ����	�F
* �߂�l�F
* ����	�F���X�g�擾����
*******************************************************************************/
CScene *CScene::GetList(DRAWORDER DrawOrder)
{
	return m_Top[DrawOrder];
}

/*******************************************************************************
* �֐����FCScene *CScene::GetCur( void )
*
* ����	�F
* �߂�l�F
* ����	�F���X�g���ݎ擾����
*******************************************************************************/
CScene *CScene::GetCur(DRAWORDER DrawOrder)
{
	return m_Cur[DrawOrder];
}

/*******************************************************************************
* �֐����FCScene *CScene::GetNext( void )
*
* ����	�F
* �߂�l�F
* ����	�F���X�g�����擾����
*******************************************************************************/
CScene *CScene::GetNext(void)
{
	return m_Next;
}