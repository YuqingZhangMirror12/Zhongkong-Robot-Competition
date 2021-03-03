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
		mydcb.BaudRate = baud;//波特率
		mydcb.ByteSize = 8;//每个字节有8位
		mydcb.Parity = NOPARITY;//无奇偶校验位
		mydcb.StopBits = ONESTOPBIT;//1个停止位
		SetCommState(hCom, &mydcb);

		SetupComm(hCom, 1024, 1024);//设置缓冲区

		//总超时＝时间系数×要求读/写的字符数＋时间常量
		COMMTIMEOUTS TimeOuts;
		TimeOuts.ReadIntervalTimeout = 1000;
		TimeOuts.ReadTotalTimeoutMultiplier = 500;
		TimeOuts.ReadTotalTimeoutConstant = 5000;
		TimeOuts.WriteTotalTimeoutMultiplier = 500;
		TimeOuts.WriteTotalTimeoutConstant = 2000;
		SetCommTimeouts(hCom, &TimeOuts);

		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空缓冲区
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
	if (!bReadStatus)//如果ReadFile函数返回FALSE
	{
		if (GetLastError() == ERROR_IO_PENDING)//GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作
		{
			WaitForSingleObject(m_osRead.hEvent, 2000);
			//使用WaitForSingleObject函数等待，直到读操作完成或延时已达到2秒钟
			//当串口读操作进行完毕后，m_osRead的hEvent事件会变为有信号
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
	
		
		
	