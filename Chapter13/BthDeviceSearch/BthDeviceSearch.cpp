#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2bth.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int retval;

	
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa)!=0) return 1;

	
	DWORD qslen = sizeof(WSAQUERYSET);
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen);
	ZeroMemory(qs, qslen);
	qs->dwSize = qslen;
	qs->dwNameSpace = NS_BTH;
	DWORD flags = LUP_CONTAINERS; /* 필수! */
	flags |= LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR;

	
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup);
	if(retval == SOCKET_ERROR){
		printf("[오류] 발견된 블루투스 장치 없음!\n");
		exit(1);
	}

	
	SOCKADDR_BTH *sa = NULL;
	bool done = false;
	while(!done){
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if(retval == NO_ERROR){
			
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			
			char addr[40] = {0};
			DWORD addrlen = sizeof(addr);
			WSAAddressToString((SOCKADDR *)sa, sizeof(SOCKADDR_BTH),
				NULL, addr, &addrlen);
			printf("블루투스 장치 발견! %s - %s\n",
				addr, qs->lpszServiceInstanceName);
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

	
	WSALookupServiceEnd(hLookup);
	free(qs);

	
	WSACleanup();
	return 0;
}