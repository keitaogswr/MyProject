/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Ftime.cpp
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
#include "renderer.h"
#include "scene.h"
#include "scene2D.h"
#include "number.h"
#include "time.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define WIDTH			( 25 )
#define HEIGHT			( 50 )
#define TEXTURE_ROW		( 10 )
#define TEXTURE_COLUMN	( 1 )
#define DEF_TIME		( 999 )

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/

/*******************************************************************************
* �֐����FCTime::CTime()
* 
* ����	�F
* �߂�l�F
* ����	�F�R���X�g���N�^
*******************************************************************************/
CTime::CTime( DRAWORDER DrawOrder ):CScene( DrawOrder )
{
	m_nTime = 0;
	m_nTimeCnt = 0;
}

/*******************************************************************************
* �֐����FCTime::~CTime()
* 
* ����	�F
* �߂�l�F
* ����	�F�f�X�g���N�^
*******************************************************************************/
CTime::~CTime()
{
}

/*******************************************************************************
* �֐����Fvoid CTime::Init( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
void CTime::Init( Vector3 pos )
{
	m_Pos = pos;
	m_nTime = DEF_TIME;

	for( int i = 0; i < TIMER_MAX; i++ )
	{
		m_Number[ i ] = CNumber::Create( Vector3( m_Pos.x + WIDTH * TIMER_MAX / 2 - WIDTH * i, m_Pos.y, m_Pos.z ) );
	}
}

/*******************************************************************************
* �֐����Fvoid CTime::Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void CTime::Uninit( void )
{
	for( int i = 0; i < TIMER_MAX; i++ )
	{
		m_Number[ i ]->Uninit();
		SAFE_DELETE( m_Number[ i ] );
	}
}

/*******************************************************************************
* �֐����Fvoid CTime::Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void CTime::Update( void )
{
	// �J�E���^�[���Z
	m_nTimeCnt++;
	if( m_nTimeCnt == 60 )
	{// �^�C�}�[�̍X�V
		m_nTime--;
		m_nTimeCnt = 0;
	}

	if( m_nTime < 0 )
	{// ������
		m_nTime = DEF_TIME;
	}

	// �^�C�}�[�̊e���̐����̕ۑ�
	for( int i = 0, div = 1; i < TIMER_MAX; i++, div *= 10 )
	{
		m_Number[ i ]->SetNum( m_nTime / div % 10 );
	}
	// �i���o�[�̍X�V
	for( int i = 0; i < TIMER_MAX; i++ )
	{
		m_Number[ i ]->Update();
	}
}

/*******************************************************************************
* �֐����Fvoid CTime::Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void CTime::Draw( void )
{
	for( int i = 0; i < TIMER_MAX; i++ )
	{
		m_Number[ i ]->Draw();
	}
}

/*******************************************************************************
* �֐����FCTime *CTime::Create( Vector3 pos )
* 
* ����	�F
* �߂�l�F
* ����	�F��������
*******************************************************************************/
CTime *CTime::Create( Vector3 pos )
{
	CTime *time;
	time = new CTime;
	time->Init( pos );
	return time;
}