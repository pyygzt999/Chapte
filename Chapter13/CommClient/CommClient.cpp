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
	//CreatFile����Ϊ�����ʡ�д���ʻ��д���ʡ��򿪡�����
	hComm = CreateFile(
		"COM3",  // �ļ���
		GENERIC_READ|GENERIC_WRITE,//�������д
		0, //��ռ��ʽ
		NULL, 
		OPEN_EXISTING,//�򿪶����Ǵ��� 
		0, 
		NULL);
	if(hComm == INVALID_HANDLE_VALUE) err_quit("CreateFile()");//������

	//����DCB�ṹ��
	DCB dcb;

	//GetCommState ��ȡ��������(������,У��,ֹͣλ,����λ��)��
	//���Ӧ�ó���ֻ��Ҫ�޸�һ�������õ�ʱ�򣬿���ͨ��GetCommState����������õ�ǰ��DCB�ṹ��
	//Ȼ�����DCB�ṹ�еĲ���������SetCommState�������������޸Ĺ���DCB�����ö˿�
	if(!GetCommState(hComm, &dcb)) err_quit("GetCommState()");

	//���ò�����9600
	dcb.BaudRate = CBR_9600;

	//���ݳ���8λ
	dcb.ByteSize = 8;

	//ָ���Ƿ�������żУ�飬��������������ΪTRUE����ִ����żУ�鲢���������Ϣ��
	//�˴�Ϊ��ִ����żУ��
	dcb.fParity = FALSE;

	//��У��λ
	dcb.Parity = NOPARITY;

	//ָ�����ڵ�ǰʹ�õ�ֹͣλ��
	dcb.StopBits = ONESTOPBIT;

	//SetCommState ��������COM�ڵ��豸���ƿ�
	if(!SetCommState(hComm, &dcb)) err_quit("SetCommState()");

	
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	//���� SetCommTimeouts ������ĳһ��COMMTIMEOUTS�ṹ�����������ó�ʱ��
	if(!SetCommTimeouts(hComm, &timeouts)) err_quit("SetCommTimeouts()");

	
	char buf[BUFSIZE+1];
	int len;
	DWORD BytesRead, BytesWritten;

	
	while(1){
		
		ZeroMemory(buf, sizeof(buf));
		printf("\n[���� ������] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		
		retval = WriteFile(hComm, buf, BUFSIZE, &BytesWritten, NULL);
		if(retval == 0) err_quit("WriteFile()");
		printf("[Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", BytesWritten);

		
		retval = ReadFile(hComm, buf, BUFSIZE, &BytesRead, NULL);
		if(retval == 0) err_quit("ReadFile()");

		
		buf[BytesRead] = '\0';
		printf("[Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", BytesRead);
		printf("[���� ������] %s\n", buf);
	}

	//�رմ���
	CloseHandle(hComm);
	return 0;
}