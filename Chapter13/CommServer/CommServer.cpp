#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char *argv[])
{
	int retval;

	HANDLE hComm;
	hComm = CreateFile("COM3", GENERIC_READ|GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	if(hComm == INVALID_HANDLE_VALUE) err_quit("CreateFile()");

	
	DCB dcb;
	if(!GetCommState(hComm, &dcb)) err_quit("GetCommState()");

	
	dcb.BaudRate = CBR_19200;
	dcb.ByteSize = 8;
	dcb.fParity = FALSE;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	if(!SetCommState(hComm, &dcb)) err_quit("SetCommState()");

	
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if(!SetCommTimeouts(hComm, &timeouts)) err_quit("SetCommTimeouts()");

	
	char buf[BUFSIZE+1];
	DWORD BytesRead, BytesWritten;

	//设置一个循环，不断监听客户端的信息
	while(1){
		
		retval = ReadFile(hComm, buf, BUFSIZE, &BytesRead, NULL);
		if(retval == 0) err_quit("ReadFile()");

		
		buf[BytesRead] = '\0';
		printf("[罐篮 单捞磐] %s\n", buf);

		
		retval = WriteFile(hComm, buf, BUFSIZE, &BytesWritten, NULL);
		if(retval == 0) err_quit("WriteFile()");
	}

	CloseHandle(hComm);
	return 0;
}