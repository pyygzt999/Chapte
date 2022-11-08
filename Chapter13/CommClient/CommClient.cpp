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
	dcb.BaudRate = CBR_19200;

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

	//超时设置
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;//以ms为单位指定通信线路上两个字符到达之间的最大时间间隔,如果被置为0，则表示不使用间隔超时。

	//因此每个读操作的总超时时间等于ReadTotalTimeoutMultiplier参数值乘以读操作要读取的字节数再加上ReadTotalTimeoutConstant参数值的和。
	//如果将ReadTotalTimeoutMultiplier和ReadTotalTimeoutConstant都设置为0，则表示读操作不使用总超时时间。
	timeouts.ReadTotalTimeoutMultiplier = 0;//以ms为单位指定一个系数，该系数用来计算读操作的总超时时间
	timeouts.ReadTotalTimeoutConstant = 0;//超时常数，如果系数和常数都为0，则没有总超时时间。

	//每个写操作的WriteTotalTimeoutConstant等于WriteTotalTimeoutMultiplier成员值乘以写操作要写的字节数，
	//再加上WriteTotalTimeoutConstant参数值的和。
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	//调用 SetCommTimeouts 设置或改变原来的超时参数。返回一个bool值
	if(!SetCommTimeouts(hComm, &timeouts)) err_quit("SetCommTimeouts()");

	
	char buf[BUFSIZE+1];
	int len;
	//DWORD 其实就是 unsigned long
	DWORD BytesRead, BytesWritten;

	//设置一个循环不断发送消息
	while(1){
		//清空缓存区
		ZeroMemory(buf, sizeof(buf));
		printf("\n[焊尘 单捞磐] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		//WriteFile() 向串口中写数据
		retval = WriteFile(hComm, buf, BUFSIZE, &BytesWritten, NULL);
		if(retval == 0) err_quit("WriteFile()");
		printf("[Hello World] %d.\n", BytesWritten);

		//ReadFile() 从串口中读取数据
		retval = ReadFile(
			hComm,//指向标识的句柄
			buf, //指向一个缓存区
			BUFSIZE, //读取的字节大小为 512 个字节
			&BytesRead, //指向调用该函数读取的实际字节数大小
			NULL);// 一个OVERLAPPED的结构
		if(retval == 0) err_quit("ReadFile()");

		
		buf[BytesRead] = '\0';
		printf("[努扼捞攫飘] %d官捞飘甫 罐疽嚼聪促.\n", BytesRead);
		printf("[罐篮 单捞磐] %s\n", buf);
	}

	//关闭串口
	CloseHandle(hComm);
	return 0;
}