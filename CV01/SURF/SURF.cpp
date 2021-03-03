#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
using namespace cv;
using namespace std;

int main()
{
	//【1】载入图像、显示并转化为灰度图
	Mat trainImageOld;
	Mat trainImage = imread("10.jpg"), trainImage_gray;
	//resize(trainImageOld, trainImage, Size(trainImageOld.cols / 5, trainImageOld.rows / 5), 0, 0, INTER_LINEAR);

	imshow("原始图", trainImage);
	cvtColor(trainImage, trainImage_gray, CV_BGR2GRAY);

	//【2】检测Surf关键点、提取训练图像描述符
	vector<KeyPoint> train_keyPoint;
	Mat trainDescriptor;
	SurfFeatureDetector featureDetector(80);
	featureDetector.detect(trainImage_gray, train_keyPoint);
	SurfDescriptorExtractor featureExtractor;
	featureExtractor.compute(trainImage_gray, train_keyPoint, trainDescriptor);

	//【3】创建基于FLANN的描述符匹配对象
	FlannBasedMatcher matcher;
	vector<Mat> train_desc_collection(1, trainDescriptor);
	matcher.add(train_desc_collection);
	matcher.train();

	int MatchCnt=0;
	Mat testImage, testImage_gray,testImage0;
	testImage = imread("12.jpg");
	imshow("Camera", testImage);

	//<2>转化图像到灰度
	cvtColor(testImage, testImage_gray, CV_BGR2GRAY);

	//<3>检测S关键点、提取测试图像描述符
	vector<KeyPoint> test_keyPoint;
	Mat testDescriptor;
	featureDetector.detect(testImage_gray, test_keyPoint);
	featureExtractor.compute(testImage_gray, test_keyPoint, testDescriptor);

	//<4>匹配训练和测试描述符
	vector<vector<DMatch> > matches;
	matcher.knnMatch(testDescriptor, matches, 2);

	// <5>根据劳氏算法（Lowe's algorithm），得到优秀的匹配点
	vector<DMatch> goodMatches;
	for (unsigned int i = 0; i < matches.size(); i++)
	{
		if (matches[i][0].distance < 0.6 * matches[i][1].distance)
		{
			goodMatches.push_back(matches[i][0]);
			MatchCnt++;
		}

	}

	//<6>绘制匹配点并显示窗口
	Mat dstImage;
	drawMatches(testImage, test_keyPoint, trainImage, train_keyPoint, goodMatches, dstImage);
	imshow("匹配窗口", dstImage);

	cout << "匹配数：" << MatchCnt << endl;
	waitKey(0);

	return 0;
}