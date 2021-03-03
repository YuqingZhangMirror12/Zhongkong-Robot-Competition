#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<Windows.h>
using namespace cv;

int main()
{
	Mat srcImage_base;
	Mat srcImage_test;
	srcImage_base = imread("0.jpg", 1);
		srcImage_test = imread("3.jpg", 1);
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
		srcImage_test = frame1;
		imshow("��׼ͼ��", srcImage_base);
		imshow("����ͼ��", srcImage_test);

		int bins = 256;
		int hist_size[] = { bins };
		float range[] = { 0, 256 };
		const float* ranges[] = { range };
		MatND redHist_base, greenHist_base, blueHist_base;
		MatND redHist_test, greenHist_test, blueHist_test;

		int channels_r[] = { 0 };
		//��3������ֱ��ͼ�ļ��㣨��ɫ�������֣�
		calcHist(&srcImage_base, 1, channels_r, Mat(), redHist_base, 1, hist_size, ranges, true, false);
		calcHist(&srcImage_test, 1, channels_r, Mat(), redHist_test, 1, hist_size, ranges, true, false);
		normalize(redHist_base, redHist_base, 0, 1, NORM_MINMAX, -1, Mat());
		normalize(redHist_test, redHist_test, 0, 1, NORM_MINMAX, -1, Mat());
		//��4������ֱ��ͼ�ļ��㣨��ɫ�������֣�
		int channels_g[] = { 1 };
		calcHist(&srcImage_base, 1, channels_g, Mat(), greenHist_base, 1, hist_size, ranges, true, false);
		calcHist(&srcImage_test, 1, channels_g, Mat(), greenHist_test, 1, hist_size, ranges, true, false);
		normalize(greenHist_base, greenHist_base, 0, 1, NORM_MINMAX, -1, Mat());
		normalize(greenHist_test, greenHist_test, 0, 1, NORM_MINMAX, -1, Mat());
		//��5������ֱ��ͼ�ļ��㣨��ɫ�������֣�
		int channels_b[] = { 2 };
		calcHist(&srcImage_base, 1, channels_b, Mat(), blueHist_base, 1, hist_size, ranges, true, false);
		calcHist(&srcImage_test, 1, channels_b, Mat(), blueHist_test, 1, hist_size, ranges, true, false);
		normalize(blueHist_base, blueHist_base, 0, 1, NORM_MINMAX, -1, Mat());
		normalize(blueHist_test, blueHist_test, 0, 1, NORM_MINMAX, -1, Mat());


		double base_r = compareHist(redHist_base, redHist_test, 1);
		double base_g = compareHist(greenHist_base, greenHist_test, 1);
		double base_b = compareHist(blueHist_base, blueHist_test, 1);
		double MatchValue = 0;
		MatchValue = base_r*base_r + base_g*base_g + base_b*base_b;
		printf("%lf\n%lf\n%lf\n%lf", base_r, base_g, base_b,MatchValue);




		//-----------------------���Ƴ���ɫֱ��ͼ------------------------
		//����׼��
		double maxValue_red, maxValue_green, maxValue_blue;
		minMaxLoc(redHist_base, 0, &maxValue_red, 0, 0);
		minMaxLoc(greenHist_base, 0, &maxValue_green, 0, 0);
		minMaxLoc(blueHist_base, 0, &maxValue_blue, 0, 0);
		int scale = 1;
		int histHeight = 256;
		Mat histImage = Mat::zeros(histHeight, bins * 3, CV_8UC3);

		//��ʽ��ʼ����
		for (int i = 0; i<bins; i++)
		{
			//����׼��
			float binValue_red = redHist_base.at<float>(i);
			float binValue_green = greenHist_base.at<float>(i);
			float binValue_blue = blueHist_base.at<float>(i);
			int intensity_red = cvRound(binValue_red*histHeight / maxValue_red);  //Ҫ���Ƶĸ߶�
			int intensity_green = cvRound(binValue_green*histHeight / maxValue_green);  //Ҫ���Ƶĸ߶�
			int intensity_blue = cvRound(binValue_blue*histHeight / maxValue_blue);  //Ҫ���Ƶĸ߶�

																					 //���ƺ�ɫ������ֱ��ͼ
			rectangle(histImage, Point(i*scale, histHeight - 1),
				Point((i + 1)*scale - 1, histHeight - intensity_red),
				CV_RGB(255, 0, 0));

			//������ɫ������ֱ��ͼ
			rectangle(histImage, Point((i + bins)*scale, histHeight - 1),
				Point((i + bins + 1)*scale - 1, histHeight - intensity_green),
				CV_RGB(0, 255, 0));

			//������ɫ������ֱ��ͼ
			rectangle(histImage, Point((i + bins * 2)*scale, histHeight - 1),
				Point((i + bins * 2 + 1)*scale - 1, histHeight - intensity_blue),
				CV_RGB(0, 0, 255));

		}

		//�ڴ�������ʾ�����ƺõ�ֱ��ͼ
		imshow("ͼ���RGBֱ��ͼ[1]", histImage);

		//-----------------------���Ƴ���ɫֱ��ͼ------------------------
		//����׼��
		minMaxLoc(redHist_test, 0, &maxValue_red, 0, 0);
		minMaxLoc(greenHist_test, 0, &maxValue_green, 0, 0);
		minMaxLoc(blueHist_test, 0, &maxValue_blue, 0, 0);
		histImage = Mat::zeros(histHeight, bins * 3, CV_8UC3);

		//��ʽ��ʼ����
		for (int i = 0; i<bins; i++)
		{
			//����׼��
			float binValue_red = redHist_test.at<float>(i);
			float binValue_green = greenHist_test.at<float>(i);
			float binValue_blue = blueHist_test.at<float>(i);
			int intensity_red = cvRound(binValue_red*histHeight / maxValue_red);  //Ҫ���Ƶĸ߶�
			int intensity_green = cvRound(binValue_green*histHeight / maxValue_green);  //Ҫ���Ƶĸ߶�
			int intensity_blue = cvRound(binValue_blue*histHeight / maxValue_blue);  //Ҫ���Ƶĸ߶�

																					 //���ƺ�ɫ������ֱ��ͼ
			rectangle(histImage, Point(i*scale, histHeight - 1),
				Point((i + 1)*scale - 1, histHeight - intensity_red),
				CV_RGB(255, 0, 0));

			//������ɫ������ֱ��ͼ
			rectangle(histImage, Point((i + bins)*scale, histHeight - 1),
				Point((i + bins + 1)*scale - 1, histHeight - intensity_green),
				CV_RGB(0, 255, 0));

			//������ɫ������ֱ��ͼ
			rectangle(histImage, Point((i + bins * 2)*scale, histHeight - 1),
				Point((i + bins * 2 + 1)*scale - 1, histHeight - intensity_blue),
				CV_RGB(0, 0, 255));

		}

		//�ڴ�������ʾ�����ƺõ�ֱ��ͼ
		imshow("ͼ���RGBֱ��ͼ[2]", histImage);
		waitKey(0);
	return 0;
}