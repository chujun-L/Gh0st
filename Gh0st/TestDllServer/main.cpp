#include <Windows.h>
#include <iostream>

using namespace std;

typedef void(_cdecl *pDLL)(const char *, UINT);

int main()
{
	const char *host = "127.0.0.1";
	UINT port = 50010;

	HMODULE hServerDLL = LoadLibrary("C:\\Users\\thinkserver\\Desktop\\gh0st\\Gh0st\\Debug\\DllServer.dll");
	if (NULL == hServerDLL) {
		cout << "LoadLibrary() failed" << endl;
		return 1;
	}

	pDLL pTestRun = (pDLL)GetProcAddress(hServerDLL, "TestRun");
	if (NULL != pTestRun) {
		pTestRun(host, port);
	}

	return 0;
}