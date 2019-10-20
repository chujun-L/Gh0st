#include <Windows.h>
#include <iostream>
//#include <WS2tcpip.h>

//#pragma comment(lib,"ws2_32.lib")

using namespace std;

typedef void(_cdecl *pDLL)(const char *, UINT);

int main()
{
	//char szHostName[MAX_PATH] = { 0 }, szIP[MAX_PATH] = { 0 };
	//ADDRINFOA AddrInfo, *Result = nullptr;
	//SOCKADDR_IN *pAddr = nullptr;

	//WSADATA wsaData;
	//if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
	//	return -1;
	//}
	//
	//int ret = gethostname(szHostName, sizeof(szHostName));
	//ret = WSAGetLastError();

	//ZeroMemory(&AddrInfo, sizeof(AddrInfo));
	//AddrInfo.ai_family = AF_INET;
	//AddrInfo.ai_socktype = SOCK_STREAM;
	//AddrInfo.ai_protocol = IPPROTO_TCP;

	//if (!getaddrinfo(szHostName, NULL, &AddrInfo, &Result)) {
	//	pAddr = (SOCKADDR_IN *)Result->ai_addr;
	//	inet_ntop(AF_INET, &pAddr->sin_addr, szIP, sizeof(szIP));
	//}

	char FilePath[MAX_PATH] = {0};
	GetModuleFileName(NULL, FilePath, sizeof(FilePath));

	char *p = strrchr(FilePath, '\\');
	*p = 0;
	strncat_s(FilePath, "\\DllServer.dll", 15);

	HMODULE hServerDLL = LoadLibrary(FilePath);
	if (NULL == hServerDLL) {
		cout << "LoadLibrary() failed" << endl;
		return 1;
	}

	const char *host = "192.168.1.102";
	UINT port = 50010;

	pDLL pTestRun = (pDLL)GetProcAddress(hServerDLL, "TestRun");
	if (NULL != pTestRun) {
		pTestRun(host, port);
	}

	return 0;
}