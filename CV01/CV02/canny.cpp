
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

		imshow("��ԭʼͼ��Canny��Ե���", srcImage); 	//��ʾԭʼͼ 
		Mat dstImage, edge, grayImage;	//��������

		dstImage.create(srcImage.size(), srcImage.type());

		cvtColor(srcImage, grayImage, CV_BGR2GRAY);

		blur(grayImage, edge, Size(5, 5));

		Canny(edge, edge, 3, 25, 3);

		imshow("��Ч��ͼ��Canny��Ե���", edge);
		
		waitKey(30);
	}
	
	return 0;
}