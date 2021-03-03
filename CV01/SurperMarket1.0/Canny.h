#pragma once
#include "opencv2/opencv.hpp"  
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

#define WINDOW_NAME1 "��ԭʼͼ���ڡ�"			//Ϊ���ڱ��ⶨ��ĺ� 
#define WINDOW_NAME2 "������ͼ��"					//Ϊ���ڱ��ⶨ��ĺ� 

Mat g_srcImage;
Mat g_grayImage;
int g_nThresh = 150;
Mat g_cannyMat_output;
vector<vector<Point>> g_vContours;
vector<Vec4i> g_vHierarchy;
int gangsiqiu(Mat g_srcImage)
{
	// ת�ɻҶȲ�ģ��������
	cvtColor(g_srcImage, g_grayImage, CV_BGR2GRAY);
	blur(g_grayImage, g_grayImage, Size(3, 3));

	//imshow(WINDOW_NAME1, g_srcImage);

	double g_dConArea = 0;
	// ��Canny���Ӽ���Ե
	Canny(g_grayImage, g_cannyMat_output, g_nThresh, g_nThresh * 2, 3);

	// Ѱ������
	findContours(g_cannyMat_output, g_vContours, g_vHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// �������
	Mat drawing = Mat::zeros(g_cannyMat_output.size(), CV_8UC3);
	for (int i = 0; i< g_vContours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing, g_vContours, i, color, CV_AA, 8, g_vHierarchy, 0, Point());
		g_dConArea += contourArea(g_vContours[i]);
		//cout << "now:" << contourArea(g_vContours[i]) << endl;
	}
	cout << "Area=" << g_dConArea << endl;
	// ��ʾЧ��ͼ
	//imshow(WINDOW_NAME2, drawing);
	return g_dConArea;
}