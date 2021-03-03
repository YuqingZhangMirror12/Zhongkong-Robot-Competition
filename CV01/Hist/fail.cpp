//---------------------------------【头文件、命名空间包含部分】----------------------------
//		描述：包含程序所使用的头文件和命名空间
//------------------------------------------------------------------------------------------------
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

//--------------------------------------【main( )函数】-----------------------------------------
//          描述：控制台应用程序的入口函数，我们的程序从这里开始执行
//-----------------------------------------------------------------------------------------------
int main()
{
	//【1】声明储存基准图像和另外两张对比图像的矩阵( RGB 和 HSV )
	Mat srcImage_base;
	Mat srcImage_test;


	//【2】载入基准图像(srcImage_base) 和测试图像srcImage_test1，并显示
	srcImage_base = imread("1.bmp", 1);
	srcImage_test = imread("2.bmp", 1);
	//显示载入的3张图像
	imshow("基准图像", srcImage_base);
	imshow("测试图像", srcImage_test);

	int bins = 64;
	const int hist_size[3] = { bins, bins, bins };
	float range[2] = { 0, 255 };
	const float* ranges[] = { range, range, range };
	MatND baseHist,testHist;
	int channels[] = { 0,1,2 };

	//【3】进行直方图的计算
	calcHist(&srcImage_base, 1, channels, Mat(), //不使用掩膜
		baseHist, 3, hist_size, ranges,
		true, false);
	//normalize(baseHist, baseHist, 0, 1);

	calcHist(&srcImage_test, 1, channels, Mat(), //不使用掩膜
		testHist, 3, hist_size, ranges,
		true, false);
	//normalize(testHist, testHist, 0, 1);


	//【8】按顺序使用4种对比标准将基准图像的直方图与其余各直方图进行对比:
	for (int i = 0; i < 4; i++)
	{
		//进行图像直方图的对比
		int compare_method = i;
		double base_base = compareHist(baseHist, baseHist, compare_method);
		double base_test = compareHist(baseHist, testHist, compare_method);

		//输出结果
		printf(" 方法 [%d] 的匹配结果如下：\n\n 【基准图 - 基准图】：%f,【基准图 - 测试图】： %f\n-----------------------------------------------------------------\n", i, base_base, base_test);
	}

	printf("检测结束。");
	waitKey(0);
	return 0;
}
