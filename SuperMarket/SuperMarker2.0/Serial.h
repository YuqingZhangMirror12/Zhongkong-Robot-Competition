#pragma once
#include<cstdio>
#include<conio.h>
#include<Windows.h>
#include<algorithm>

HANDLE hCom;
DWORD dwErrorFlags;
COMSTAT ComStat;
char readdata[1024] = { 0 };
char writedata[1024] = "EORR\r\n";

bool SerialPortInit(int baud, char* numofcom);
DWORD ReadSerial(char lpInBuffer[]);
DWORD WriteSerial(char lpOutBuffer[]);
void CloseSerial();
bool IsComIn();

bool SerialPortInit(int baud, char* numofcom)
{
	DWORD dwError;
	hCom = CreateFile(numofcom, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		printf("Fail>_<\n");
		dwError = GetLastError();
		printf("%x", dwError);
		return FALSE;
	}
	else
	{
		printf("Create Success!\n");

		DCB mydcb;
		GetCommState(hCom, &mydcb);
		mydcb.BaudRate = baud;//������
		mydcb.ByteSize = 8;//ÿ���ֽ���8λ
		mydcb.Parity = NOPARITY;//����żУ��λ
		mydcb.StopBits = ONESTOPBIT;//1��ֹͣλ
		SetCommState(hCom, &mydcb);

		SetupComm(hCom, 1024, 1024);//���û�����

		//�ܳ�ʱ��ʱ��ϵ����Ҫ���/д���ַ�����ʱ�䳣��
		COMMTIMEOUTS TimeOuts;
		TimeOuts.ReadIntervalTimeout = 1000;
		TimeOuts.ReadTotalTimeoutMultiplier = 500;
		TimeOuts.ReadTotalTimeoutConstant = 5000;
		TimeOuts.WriteTotalTimeoutMultiplier = 500;
		TimeOuts.WriteTotalTimeoutConstant = 2000;
		SetCommTimeouts(hCom, &TimeOuts);

		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ�����
	}
	return TRUE;
}

DWORD ReadSerial(char lpInBuffer[])
{
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	int que= ComStat.cbInQue;
	DWORD dwBytesRead = min(1024, que);
	OVERLAPPED m_osRead;
	memset(&m_osRead, 0, sizeof(OVERLAPPED));
	m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	BOOL bReadStatus;
	bReadStatus = ReadFile(hCom, lpInBuffer, dwBytesRead, &dwBytesRead, &m_osRead);
	if (!bReadStatus)//���ReadFile��������FALSE
	{
		if (GetLastError() == ERROR_IO_PENDING)//GetLastError()��������ERROR_IO_PENDING,�����������ڽ��ж�����
		{
			WaitForSingleObject(m_osRead.hEvent, 2000);
			//ʹ��WaitForSingleObject�����ȴ���ֱ����������ɻ���ʱ�Ѵﵽ2����
			//�����ڶ�����������Ϻ�m_osRead��hEvent�¼����Ϊ���ź�
			PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			return dwBytesRead;
		}
		return 0;
	}
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return dwBytesRead;
}
DWORD WriteSerial(char lpOutBuffer[])
{
	OVERLAPPED m_osWrite;
	DWORD dwBytesWritten = 1024;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	memset(&m_osWrite, 0, sizeof(OVERLAPPED));
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	bWriteStat = WriteFile(hCom, lpOutBuffer, dwBytesWritten, &dwBytesWritten, &m_osWrite);
	if (!bWriteStat)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_osWrite.hEvent, 2000);
			PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			return dwBytesWritten;
		}
		return 0;
	}
	return dwBytesWritten;
}

void CloseSerial()
{
	CloseHandle(hCom);
}

bool IsComIn()
{
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	if (ComStat.cbInQue)
		return TRUE;
	return FALSE;
}
	
		
		
	