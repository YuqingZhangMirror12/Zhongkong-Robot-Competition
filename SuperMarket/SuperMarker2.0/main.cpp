#include<cstdio>
#include<opencv2/opencv.hpp>
#include <string.h>
#include <Windows.h>

#include"Judge.h"
#include"Serial.h"
using namespace cv;
using namespace std;

int main()
{
	bool IsSerialInit=FALSE;
	Init_py();
	do
	{
		IsSerialInit = SerialPortInit(115200, "COM3");
		printf("Creat Again");
		Sleep(500);
	}
	while (IsSerialInit==FALSE);
	//readdata[0]='R';
	while (TRUE)
	{
		memset(readdata, 0, sizeof(int));
		if (IsComIn())//判断串口缓冲区是否有数据
			ReadSerial(readdata);
		printf("Read:%s\n", readdata);
		if (readdata[0] == 'R')
		{
			Mat frame0, frame1;
			VideoCapture cap0, cap1;
			cap0.open(1);
			Sleep(50);
			cap1.open(0);
			Sleep(100);
			cap0 >> frame0;
			cap1 >> frame1;
			while (!frame1.rows)
			{
				cap1 >> frame1;
				Sleep(50);
			}
			//imshow("video1", frame1);
			//waitKey(500);
			//Sleep(1000);
			int cnt = 0;
			char name[16], pname[5] = ".jpg";
			_itoa(++cnt, name, 10);
			strcat(name, pname);
			imwrite(name, frame1);
			int what;
			what = Judge(cnt);//拍照并判断物品是什么
			switch (what)
			{
			case 6: strcpy(writedata, "A1A1"); break;
			case 11: strcpy(writedata, "A2A2"); break;
			case 1: strcpy(writedata, "A3A3"); break;
			case 10: strcpy(writedata, "B4B4"); break;
			case 8: strcpy(writedata, "B5B5"); break;
			case 5: strcpy(writedata, "B6B6"); break;
			case 3: strcpy(writedata, "C7C7"); break;
			case 2: strcpy(writedata, "C8C8"); break;
			case 0: strcpy(writedata, "C9C9"); break;
			case 4: strcpy(writedata, "DxDx"); break;
			case 9: strcpy(writedata, "DyDy"); break;
			case 7: strcpy(writedata, "DzDz"); break;
			default:strcpy(writedata, "0000"); break;
			}
			WriteSerial(writedata);
			Sleep(500);
			printf("Send:%s\n", writedata);
			/*if (IsComIn())//判断串口缓冲区是否有数据
				ReadSerial(readdata);
			printf("Retell:%s\n", readdata);*/
		}
		Sleep(100);
	}
	return 0;
}