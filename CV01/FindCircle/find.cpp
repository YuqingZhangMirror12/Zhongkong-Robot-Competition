#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include<cstdio>

using namespace cv;
vector<vector<Point>> g_vContours;
vector<Vec4i> g_vHierarchy;

int main()
{
	//开起摄像头
	/*VideoCapture capture, cap0;
	capture.open(0);
	cap0.open(1);
	*/
	Mat edges;  //定义转化的灰度图
	namedWindow("【效果图】", CV_WINDOW_NORMAL);
	/*while (1)
	{*/
		Mat frame,f0;
		frame = imread("1.jpg");
		//capture >> frame;
		//cap0 >> f0;
		if (!frame.data)
			return -1;
		cvtColor(frame, edges, CV_BGR2GRAY);
		//高斯滤波
		GaussianBlur(edges, edges, Size(7, 7), 5, 5);
		imshow("BLUR", edges);
		vector<Vec3f> circles;
		//霍夫圆
		HoughCircles(edges, circles, CV_HOUGH_GRADIENT, 1.5, 50, 80, 100, 120, 0);
		printf("%d\n", circles.size());
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			//绘制圆心  
			circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			//绘制圆轮廓  
			circle(frame, center, radius, Scalar(155, 50, 255), 3, 8, 0);
		}


		imshow("【效果图】", frame);


		waitKey();

	//}

	return 0;
}
