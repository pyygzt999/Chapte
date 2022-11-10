/**
* 蓝牙客户端通信基本流程：
* 1、创建客户端蓝牙Sokcet
* 2、创建连接
* 3、读写数据
* 4、关闭
*/


#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2bth.h>
#include <Initguid.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERADDR "00:09:31:00:13:F9"
#define BUFSIZE    512

DEFINE_GUID(BthServer_Service, 0x4672de25, 0x588d, 0x48af,
	0x80, 0x73, 0x5f, 0x2b, 0x7b, 0x0, 0x60, 0x1f);

//错误处理
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//显示错误信息
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char *argv[])
{
	int retval;

	/*与所有Windows套接字应用程序编程一样，
	必须调用 WSAStartup 函数来启动Windows套接字功能并启用蓝牙*/
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	/*	WSAQUERYSET 结构用于操作，包括设备查询、服务查询和设置服务。*/
	DWORD qslen = sizeof(WSAQUERYSET);//获取一个 WSAQUERYSET 的大小
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen);//用一个结构体指针指向 WSAQUERYSET
	ZeroMemory(qs, qslen);//初始化
	qs->dwSize = qslen;
	qs->dwNameSpace = NS_BTH;
	qs->lpServiceClassId = (GUID *)&BthServer_Service;
	qs->lpszContext = SERVERADDR;
	DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_ADDR;
	
	
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup);
	if(retval == SOCKET_ERROR){
		printf("错误！！\n");
		exit(1);
	}

	
	SOCKADDR_BTH *sa = NULL;
	int serverport = 0;
	bool done = false;

	while(!done){
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if(retval == NO_ERROR){
			
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			
			serverport = sa->port;
			break;
		}
		else{
			if(WSAGetLastError() == WSAEFAULT){
				free(qs);
				qs = (WSAQUERYSET *)malloc(qslen);
			}
			else{
				done = true;
			}
		}
	}
	if(sa == NULL){
		printf("错误！！\n", SERVERADDR);
		exit(1);
	}

	
	WSALookupServiceEnd(hLookup);
	free(qs);

	// socket()
	SOCKET sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_BTH serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress(SERVERADDR, AF_BTH, NULL,
		(SOCKADDR *)&serveraddr, &addrlen);
	serveraddr.port = serverport;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	
	char buf[BUFSIZE+1];
	int len;

	
	while(1){
		
		printf("\n[焊尘 单捞磐] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		
		retval = send(sock, buf, strlen(buf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		printf("[喉风捧胶 努扼捞攫飘] %d官捞飘甫 焊陈嚼聪促.\n", retval);
	}

	// closesocket()
	closesocket(sock);

	
	WSACleanup();
	return 0;
}