/*******************************************************************************
* �^�C�g�����F
* �t�@�C�����FsoundAL.h
* �쐬��	�FAT13B284 10 ���}���[��
* �쐬��	�F
********************************************************************************
* �X�V����	�F
*
*******************************************************************************/
/*******************************************************************************
* �C���N���[�h�K�[�h
*******************************************************************************/
#ifndef _SOUND_H_
#define _SOUND_H_

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "xaudio2.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define SOUND_MAX	( 3 )

/*******************************************************************************
* �O���錾
*******************************************************************************/

/*******************************************************************************
* �N���X�錾
*******************************************************************************/
class CSound
{
	public:
		typedef enum
		{
			SOUND_LABEL_000 = 0,		// BGM0
			SOUND_LABEL_EXPLOSION_000,	// BGM1
			SOUND_LABEL_002,			// BGM2
			SOUND_LABEL_MAX
		}SOUND_LABEL;

		typedef struct
		{
			char *pFileName;
			int nCntLoop;
		}SOUND_PARAM;

		CSound();
		~CSound();

		static HRESULT Init( HWND hWnd );
		static void Uninit( void );
		
		static HRESULT Play( SOUND_LABEL label );
		static void Stop( SOUND_LABEL label );
		static void Stop( void );
	private:
		static HRESULT CheckChunk( HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition );
		static HRESULT ReadChunkData( HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset );

		static SOUND_PARAM m_SoundParam[ SOUND_MAX ];

		static IXAudio2 *m_pXAudio2;									// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
		static IXAudio2MasteringVoice *m_pMasteringVoice;				// �}�X�^�[�{�C�X
		static IXAudio2SourceVoice *m_apSourceVoice[ SOUND_LABEL_MAX ];	// �\�[�X�{�C�X
		static BYTE *m_apDataAudio[ SOUND_LABEL_MAX ];					// �I�[�f�B�I�f�[�^
		static DWORD m_aSizeAudio[ SOUND_LABEL_MAX ];					// �I�[�f�B�I�f�[�^�T�C�Y
};

#endif
