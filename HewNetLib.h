#pragma once

#ifdef HEWNETLIB_DLL
#define HEWNETLIB_API __declspec(dllexport)
#else
#define HEWNETLIB_API __declspec(dllimport)
#pragma comment(lib, "HewNetlib.lib")
#endif

struct NetUtil;
struct NetInfo;
struct NetUser;

class HEWNETLIB_API HewNetLib
{
private:
	NetUtil *mNetUtil;
	NetInfo *mNetInfo;
	NetUser *mJoinUser;
public:
	HewNetLib( const char *file, bool showLog = false );
	~HewNetLib();

	bool HasJoinUser();

	const char *GetJoinUserName();
	bool SendResult( const char *text, int point = 500 );

	void Reset();

private:
	void UpdateThread();
};

