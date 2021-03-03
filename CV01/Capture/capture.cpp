#include <opencv2\opencv.hpp>
#include <conio.h>
#include <string.h>
#include <Windows.h>

using namespace cv;

int main()
{
	Mat frame0, frame1;
	VideoCapture cap0, cap1;
	int cnt = 0;
	char name[16],pname[5]=".jpg",tmp;
	cap0.open(1);
	Sleep(100);
	cap1.open(0);
	Sleep(100);

	while (1)
	{
		cap0 >> frame0;
		//imshow("video0", frame0);
		cap1 >> frame1;
		imshow("video1", frame1);
		while (!frame1.rows)
		{
			cap1 >> frame1;
			Sleep(50);
		}
		if (_kbhit())
		{
			tmp = _getch();
			printf("GET\n");
			_itoa(cnt++, name, 10);
			strcat(name, pname);
			imwrite(name,frame1);
		}
		waitKey(30);
	}
}