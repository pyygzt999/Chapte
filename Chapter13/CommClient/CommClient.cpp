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
	//CreatFile（）为读访问、写访问或读写访问“打开”串口
	hComm = CreateFile(
		"COM3",  // 文件名
		GENERIC_READ|GENERIC_WRITE,//允许读和写
		0, //独占方式
		NULL, 
		OPEN_EXISTING,//打开而不是创建 
		0, 
		NULL);
	if(hComm == INVALID_HANDLE_VALUE) err_quit("CreateFile()");//错误处理

	//串口DCB结构体
	DCB dcb;

	//GetCommState 读取串口设置(波特率,校验,停止位,数据位等)。
	//如果应用程序只需要修改一部分配置的时候，可以通过GetCommState（）函数获得当前的DCB结构，
	//然后更改DCB结构中的参数，调用SetCommState（）函数配置修改过的DCB来配置端口
	if(!GetCommState(hComm, &dcb)) err_quit("GetCommState()");

	//设置波动率9600
	dcb.BaudRate = CBR_9600;

	//数据长度8位
	dcb.ByteSize = 8;

	//指定是否允许奇偶校验，如果这个参数设置为TRUE，则执行奇偶校验并报告错误信息。
	//此处为不执行奇偶校验
	dcb.fParity = FALSE;

	//无校验位
	dcb.Parity = NOPARITY;

	//指定串口当前使用的停止位数
	dcb.StopBits = ONESTOPBIT;

	//SetCommState 重新设置COM口的设备控制块
	if(!SetCommState(hComm, &dcb)) err_quit("SetCommState()");

	
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	//调用 SetCommTimeouts 可以用某一个COMMTIMEOUTS结构的内容来设置超时。
	if(!SetCommTimeouts(hComm, &timeouts)) err_quit("SetCommTimeouts()");

	
	char buf[BUFSIZE+1];
	int len;
	DWORD BytesRead, BytesWritten;

	
	while(1){
		
		ZeroMemory(buf, sizeof(buf));
		printf("\n[焊尘 单捞磐] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		
		retval = WriteFile(hComm, buf, BUFSIZE, &BytesWritten, NULL);
		if(retval == 0) err_quit("WriteFile()");
		printf("[努扼捞攫飘] %d官捞飘甫 焊陈嚼聪促.\n", BytesWritten);

		
		retval = ReadFile(hComm, buf, BUFSIZE, &BytesRead, NULL);
		if(retval == 0) err_quit("ReadFile()");

		
		buf[BytesRead] = '\0';
		printf("[努扼捞攫飘] %d官捞飘甫 罐疽嚼聪促.\n", BytesRead);
		printf("[罐篮 单捞磐] %s\n", buf);
	}

	//关闭串口
	CloseHandle(hComm);
	return 0;
}