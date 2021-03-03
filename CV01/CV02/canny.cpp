
#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

int main()
{
	VideoCapture capture, cap1;
	capture.open(0);
	cap1.open(1);
	if (!capture.isOpened())
	{
		return -1;
	}
	Mat srcImage;
	while (1)
	{
		capture.read(srcImage);

		imshow("【原始图】Canny边缘检测", srcImage); 	//显示原始图 
		Mat dstImage, edge, grayImage;	//参数定义

		dstImage.create(srcImage.size(), srcImage.type());

		cvtColor(srcImage, grayImage, CV_BGR2GRAY);

		blur(grayImage, edge, Size(5, 5));

		Canny(edge, edge, 3, 25, 3);

		imshow("【效果图】Canny边缘检测", edge);
		
		waitKey(30);
	}
	
	return 0;
}