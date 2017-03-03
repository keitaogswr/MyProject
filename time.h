/*******************************************************************************
* タイトル名：
* ファイル名：time.h
* 作成者	：AT13B284 10 小笠原啓太
* 作成日	：
********************************************************************************
* 更新履歴	：
*
*******************************************************************************/
/*******************************************************************************
* インクルードガード
*******************************************************************************/
#ifndef _TIME_H_
#define _TIME_H_

/*******************************************************************************
* インクルードファイル
*******************************************************************************/

/*******************************************************************************
* マクロ定義
*******************************************************************************/
#define TIMER_MAX	( 3 )

/*******************************************************************************
* 構造体
*******************************************************************************/

/*******************************************************************************
* 前方宣言
*******************************************************************************/
class CNumber;

/*******************************************************************************
* クラス宣言
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
		static int m_nTime;
		int m_nTimeCnt;

		static CTime *Create( Vector3 pos );
		static int GetTime(void) { return m_nTime; }
};

#endif