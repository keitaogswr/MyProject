/*******************************************************************************
* タイトル名：
* ファイル名：soundAL.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _SOUND_H_
#define _SOUND_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "xaudio2.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/

/*******************************************************************************
* クラス宣言
*******************************************************************************/
class CSound
{
	public:
		typedef enum
		{
			SOUND_LABEL_000 = 0,			// SE0
			SOUND_LABEL_EXPLOSION_000,		// SE1
			SOUND_LABEL_SE_SHOT_000,		// SE1
			SOUND_LABEL_SE_BUTTON_000,		// SE1
			SOUND_LABEL_BGM_000,			// BGM0
			SOUND_LABEL_BGM_001,			// BGM1
			SOUND_LABEL_BGM_002,			// BGM2
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

		static SOUND_PARAM m_SoundParam[SOUND_LABEL_MAX];

		static IXAudio2 *m_pXAudio2;									// XAudio2オブジェクトへのインターフェイス
		static IXAudio2MasteringVoice *m_pMasteringVoice;				// マスターボイス
		static IXAudio2SourceVoice *m_apSourceVoice[ SOUND_LABEL_MAX ];	// ソースボイス
		static BYTE *m_apDataAudio[ SOUND_LABEL_MAX ];					// オーディオデータ
		static DWORD m_aSizeAudio[ SOUND_LABEL_MAX ];					// オーディオデータサイズ
};

#endif
