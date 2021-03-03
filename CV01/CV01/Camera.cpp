#include <opencv2\opencv.hpp>
//#include<Windows.h>

using namespace cv;


int main()
{
	//Sleep(3000);
	VideoCapture cap0,cap1;
	cap0.open(0);
	cap1.open(1);
	if (!cap1.isOpened())
	{
		return -1;
	}
	while (1)
	{
		Mat frame0,frame1;
		cap0 >> frame0;
		cap1 >> frame1;

		imshow("video0", frame0);
		imshow("video1", frame1);
		waitKey(30);
	}
	return 0;
}