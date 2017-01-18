/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����Fmain.cpp
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "renderer.h"
#include "main.h"
#include "scene.h"
#include "input.h"
#include "manager.h"
#ifdef _DEBUG
#include "debugProc.h"
#endif

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
HRESULT Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow );
void Uninit( void );
void Update( void );
void Draw( void );
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
int g_nCountFPS;							// FPS�|�C���^

/*******************************************************************************
* �֐����FWINAPI WinMain( HINSTANCE hInstance,
*					HINSTANCE hPrevInstance,
*					LPSTR lpCmdLine,
*					int nCmdShow )
* 
* ����	�F
* �߂�l�F
* ����	�F���C���֐�
*******************************************************************************/
int WINAPI WinMain( HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow )
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//�ϐ���`
	DWORD dwFrameCount;
	DWORD dwCurrentTime;
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;

	//�E�B���h�E�N���X�̓o�^
	WNDCLASSEX wcex = 
	{
		sizeof( WNDCLASSEX ),					//�������T�C�Y��ݒ�
		CS_CLASSDC,								//�E�B���h�E�̃X�^�C����ݒ�
		WndProc,								//�E�B���h�E�v���V�[�W���̃A�h���X�i�֐����j���w��
		0,
		0,
		hInstance,								//�C���X�^���g�n���h�����w��
		NULL,									//�g�p����A�C�R�����w��
		LoadCursor( NULL, IDC_ARROW ),			//�}�E�X�J�[�\�����w��
		( HBRUSH )( COLOR_WINDOW + 1 ),			//�N���C�A���g�̈�̔w�i�F��ݒ�
		NULL,									//���j���[��ݒ�
		WindowClassName,						//�E�C���h�E�N���X�̖��O
		NULL									//�������A�C�R�����ݒ肳�ꂽ�ꍇ�̏��
	};

	HWND hWnd;									//�E�B���h�E�n���h��
	MSG msg;									//���b�Z�[�W
	RegisterClassEx( &wcex );

	
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, false );

	//�E�B���h�E�N���X�̐ݒ�
	hWnd = CreateWindowEx( 0,
							WindowClassName,			//�E�B���h�E�N���X�̖��O
							"SWIPE OUT Mk.II",			//�E�B���h�E�̖��O
							WS_OVERLAPPEDWINDOW,		
							CW_USEDEFAULT,				//�E�B���h�E�̍���X���W
							CW_USEDEFAULT,				//�E�B���h�E�̍���Y���W
							rect.right - rect.left,		//��
							rect.bottom - rect.top,		//����
							NULL,						//�e�E�B���h�E�̃n���h��
							NULL,						//���j���[�n���h���܂��͎q�E�B���h�EID
							hInstance,
							NULL );						//�E�B���h�E�쐬�f�[�^
	

	/* �E�B���h�E�̕\�� */
	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	timeBeginPeriod( 1 );
	dwFrameCount =
	dwCurrentTime = 0;
	dwExecLastTime =
	dwFPSLastTime = timeGetTime();

	// ������
	Init( hInstance, hWnd, TRUE );

	//���b�Z�[�W���[�v
	while( 1 )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) != 0 )
		{
			if( msg.message == WM_QUIT )
			{
				break;
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if( ( dwCurrentTime - dwFPSLastTime ) >= 500 )
			{
				g_nCountFPS = ( dwFrameCount * 1000 ) / ( dwCurrentTime - dwFPSLastTime );
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			if( ( dwCurrentTime - dwExecLastTime ) >= ( 1000 / 60 ) )
			{
				dwExecLastTime = dwCurrentTime;
				Update();		//�X�V����
				Draw();			//�`�揈��
				dwFrameCount++;
			}
		}
	}
	UnregisterClass( WindowClassName,		//�E�C���h�E�N���X�̖��O
					wcex.hInstance
					);
	Uninit();			// �I������
	timeEndPeriod( 1 );
	return ( int )msg.wParam;
}

/*******************************************************************************
* �֐����FHRESULT Init( HINSTANCE hInstance, HWND, BOOL bWindow )
* 
* ����	�F
* �߂�l�F
* ����	�F����������
*******************************************************************************/
HRESULT Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow )
{
	CManager::Init(hInstance, hWnd, TRUE);
	return S_OK;
}

/*******************************************************************************
* �֐����Fvoid Uninit( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�I������
*******************************************************************************/
void Uninit( void )
{
	CManager::Uninit();
}

/*******************************************************************************
* �֐����Fvoid Update( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�X�V����
*******************************************************************************/
void Update( void )
{
	CManager::Update();
	CDebugProc::Print("FPS:%d\n\n", g_nCountFPS);
}

/*******************************************************************************
* �֐����Fvoid Draw( void )
* 
* ����	�F
* �߂�l�F
* ����	�F�`�揈��
*******************************************************************************/
void Draw( void )
{
	CManager::Draw();			//�`�揈��
}

/*******************************************************************************
* �֐����FLRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
* 
* ����	�F
* �߂�l�F
* ����	�F�E�C���h�E�v���V�[�W��
*******************************************************************************/
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	/* �ϐ���` */
	int nID;

	switch( uMsg )
	{
		case WM_DESTROY :						//�E�B���h�E���j�����ꂽ
			PostQuitMessage( 0 );
			break;
		case WM_KEYDOWN :						//�L�[�������ꂽ
			switch( wParam )
			{
				case VK_ESCAPE :				//esc�L�[�������ꂽ
					DestroyWindow( hWnd );
			}
			break;
		case WM_CLOSE :							//�E�C���h�E�����
			nID = MessageBox( hWnd, "�I�����܂����H", "�x��", MB_YESNO );
			if( nID == IDYES )
			{
				DestroyWindow( hWnd );
			}
			else if( nID == IDNO )
			{
				return 0;
			}
			break;
		default :
			break;
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}