#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/core/Core.hpp>
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace cv;

typedef struct
{
	cv::Rect rect;
	double distance;
	int countInRange;
} rectPointType;

bool sortFun(const rectPointType& r1, const rectPointType& r2)
{
	return (r1.countInRange > r2.countInRange);
}

int main()
{
	Mat imgSrc = cv::imread("1.jpg");

	cv::Size sz = imgSrc.size();
	if (sz.width > 0)
	{
		imshow("pic", imgSrc);
	}
	waitKey(0);
	Mat imgBlur;
	blur(imgSrc, imgBlur, cv::Size(7, 7));

	Mat imgGray;
	cvtColor(imgBlur, imgGray, CV_BGR2GRAY);

	Mat canny_output;
	int thresh = 10;
	Canny(imgGray, canny_output, thresh, thresh * 2, 3);

	int dilation_type = MORPH_CROSS;
	int dilation_size = 2;
	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));

	dilate(canny_output, canny_output, element);

	imshow("pic_canny", canny_output);
	waitKey(0);

	vector<vector<Point> > countours;
	vector<Vec4i> hierarchy;

	/// Find contours
	findContours(canny_output, countours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > vecApproxOut;
	vector<rectPointType> vecRect;
	for (int i = 0; i < countours.size(); i++)
	{
		vector<Point> approxOut;
		approxPolyDP(Mat(countours[i]), approxOut, arcLength(Mat(countours[i]), true) * 0.01, true);

		if ((approxOut.size() < 4) || (approxOut.size() > 10))
		{
			continue;
		}

		double area = contourArea(approxOut);
		if ((area < 1000) || (area >10000))
		{
			continue;
		}

		Rect rect;
		rect = boundingRect(approxOut);

		double t = ((float)rect.height / ((float)rect.width));
		std::cout << "t " << t << std::endl;
		if ((t < 0.8) || (t > 1.3))
		{
			continue;
		}

		rectPointType rectPoint;
		rectPoint.rect = rect;
		rectPoint.distance = 0.0f;

		vecRect.push_back(rectPoint);
	}

	for (int index = 0; index < vecRect.size(); index++)
	{
		rectPointType now_rect = vecRect.at(index);
		int countInRange = 0;
		for (int i = 0; i < vecRect.size(); i++)
		{
			double x = now_rect.rect.x - vecRect.at(i).rect.x;
			double y = now_rect.rect.y - vecRect.at(i).rect.y;
			double dis = x*x + y*y;
			std::cout << "index " << index << " dis " << dis << std::endl;
			if (dis < 60000)
			{
				countInRange++;
			}
		}
		vecRect.at(index).countInRange = countInRange;
	}

	std::sort(vecRect.begin(), vecRect.end(), sortFun);

	vector<Point> pointBox;

	for (int i = 0; i < vecRect.size(); i++)
	{
		if (vecRect.at(i).countInRange > 8)
		{
			Point p1;
			Point p2;
			Point p3;
			Point p4;

			p1.x = vecRect.at(i).rect.x;
			p1.y = vecRect.at(i).rect.y;

			p2.x = vecRect.at(i).rect.x + vecRect.at(i).rect.height;
			p2.y = vecRect.at(i).rect.y;

			p3.x = vecRect.at(i).rect.x + vecRect.at(i).rect.height;
			p3.y = vecRect.at(i).rect.y + vecRect.at(i).rect.width;

			p4.x = vecRect.at(i).rect.x;
			p4.y = vecRect.at(i).rect.y + vecRect.at(i).rect.width;

			pointBox.push_back(p1);
			pointBox.push_back(p2);
			pointBox.push_back(p3);
			pointBox.push_back(p4);
		}
	}

	Rect rectBox;
	rectBox = boundingRect(pointBox);

	line(imgSrc, Point(rectBox.x, rectBox.y), Point(rectBox.x + rectBox.width, rectBox.y), cvScalar(0, 0, 255), 4);
	line(imgSrc, Point(rectBox.x + rectBox.width, rectBox.y), Point(rectBox.x + rectBox.width, rectBox.y + rectBox.height), cvScalar(0, 255, 0), 4);
	line(imgSrc, Point(rectBox.x + rectBox.width, rectBox.y + rectBox.height), Point(rectBox.x, rectBox.y + rectBox.height), cvScalar(255, 0, 0), 4);
	line(imgSrc, Point(rectBox.x, rectBox.y + rectBox.height), Point(rectBox.x, rectBox.y), cvScalar(0, 255, 255), 4);

	imshow("imgSrc", imgSrc);
	waitKey(0);
	return 0;
}