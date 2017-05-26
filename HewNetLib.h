#pragma once

#if 1
#ifdef HEWNETLIB_DLL
#define HEWNETLIB_API __declspec(dllexport)
#else
#define HEWNETLIB_API __declspec(dllimport)
#pragma comment(lib, "HewNetlib.lib")
#endif
#else
#define HEWNETLIB_API
#endif //  0


struct NetUtil;
struct NetInfo;
struct NetUser;

class HEWNETLIB_API HewNetLib
{
private:
	bool mIsInit;
	NetUtil *mNetUtil;
	NetInfo *mNetInfo;
	NetUser *mJoinUser;
public:
	~HewNetLib();

	static HewNetLib *Singleton( int id = 0 );

	void Init( const char*file, bool showLog = false );

	bool Ready();

	const char *UserName();
	bool SendPoint( int point );

	void Reset();

private:
	void UpdateThread();
};

