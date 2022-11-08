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
	dcb.BaudRate = CBR_19200;

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

	//��ʱ����
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;//��msΪ��λָ��ͨ����·�������ַ�����֮������ʱ����,�������Ϊ0�����ʾ��ʹ�ü����ʱ��

	//���ÿ�����������ܳ�ʱʱ�����ReadTotalTimeoutMultiplier����ֵ���Զ�����Ҫ��ȡ���ֽ����ټ���ReadTotalTimeoutConstant����ֵ�ĺ͡�
	//�����ReadTotalTimeoutMultiplier��ReadTotalTimeoutConstant������Ϊ0�����ʾ��������ʹ���ܳ�ʱʱ�䡣
	timeouts.ReadTotalTimeoutMultiplier = 0;//��msΪ��λָ��һ��ϵ������ϵ������������������ܳ�ʱʱ��
	timeouts.ReadTotalTimeoutConstant = 0;//��ʱ���������ϵ���ͳ�����Ϊ0����û���ܳ�ʱʱ�䡣

	//ÿ��д������WriteTotalTimeoutConstant����WriteTotalTimeoutMultiplier��Աֵ����д����Ҫд���ֽ�����
	//�ټ���WriteTotalTimeoutConstant����ֵ�ĺ͡�
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	//���� SetCommTimeouts ���û�ı�ԭ���ĳ�ʱ����������һ��boolֵ
	if(!SetCommTimeouts(hComm, &timeouts)) err_quit("SetCommTimeouts()");

	
	char buf[BUFSIZE+1];
	int len;
	//DWORD ��ʵ���� unsigned long
	DWORD BytesRead, BytesWritten;

	//����һ��ѭ�����Ϸ�����Ϣ
	while(1){
		//��ջ�����
		ZeroMemory(buf, sizeof(buf));
		printf("\n[���� ������] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		//WriteFile() �򴮿���д����
		retval = WriteFile(hComm, buf, BUFSIZE, &BytesWritten, NULL);
		if(retval == 0) err_quit("WriteFile()");
		printf("[Hello World] %d.\n", BytesWritten);

		//ReadFile() �Ӵ����ж�ȡ����
		retval = ReadFile(
			hComm,//ָ���ʶ�ľ��
			buf, //ָ��һ��������
			BUFSIZE, //��ȡ���ֽڴ�СΪ 512 ���ֽ�
			&BytesRead, //ָ����øú�����ȡ��ʵ���ֽ�����С
			NULL);// һ��OVERLAPPED�Ľṹ
		if(retval == 0) err_quit("ReadFile()");

		
		buf[BytesRead] = '\0';
		printf("[Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", BytesRead);
		printf("[���� ������] %s\n", buf);
	}

	//�رմ���
	CloseHandle(hComm);
	return 0;
}