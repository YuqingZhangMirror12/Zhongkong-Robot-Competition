//---------------------------------��ͷ�ļ��������ռ�������֡�----------------------------
//		����������������ʹ�õ�ͷ�ļ��������ռ�
//------------------------------------------------------------------------------------------------
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

//--------------------------------------��main( )������-----------------------------------------
//          ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼִ��
//-----------------------------------------------------------------------------------------------
int main()
{
	//��1�����������׼ͼ����������ŶԱ�ͼ��ľ���( RGB �� HSV )
	Mat srcImage_base;
	Mat srcImage_test;


	//��2�������׼ͼ��(srcImage_base) �Ͳ���ͼ��srcImage_test1������ʾ
	srcImage_base = imread("1.bmp", 1);
	srcImage_test = imread("2.bmp", 1);
	//��ʾ�����3��ͼ��
	imshow("��׼ͼ��", srcImage_base);
	imshow("����ͼ��", srcImage_test);

	int bins = 64;
	const int hist_size[3] = { bins, bins, bins };
	float range[2] = { 0, 255 };
	const float* ranges[] = { range, range, range };
	MatND baseHist,testHist;
	int channels[] = { 0,1,2 };

	//��3������ֱ��ͼ�ļ���
	calcHist(&srcImage_base, 1, channels, Mat(), //��ʹ����Ĥ
		baseHist, 3, hist_size, ranges,
		true, false);
	//normalize(baseHist, baseHist, 0, 1);

	calcHist(&srcImage_test, 1, channels, Mat(), //��ʹ����Ĥ
		testHist, 3, hist_size, ranges,
		true, false);
	//normalize(testHist, testHist, 0, 1);


	//��8����˳��ʹ��4�ֶԱȱ�׼����׼ͼ���ֱ��ͼ�������ֱ��ͼ���жԱ�:
	for (int i = 0; i < 4; i++)
	{
		//����ͼ��ֱ��ͼ�ĶԱ�
		int compare_method = i;
		double base_base = compareHist(baseHist, baseHist, compare_method);
		double base_test = compareHist(baseHist, testHist, compare_method);

		//������
		printf(" ���� [%d] ��ƥ�������£�\n\n ����׼ͼ - ��׼ͼ����%f,����׼ͼ - ����ͼ���� %f\n-----------------------------------------------------------------\n", i, base_base, base_test);
	}

	printf("��������");
	waitKey(0);
	return 0;
}
