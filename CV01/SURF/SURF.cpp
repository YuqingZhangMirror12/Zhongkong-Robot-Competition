#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
using namespace cv;
using namespace std;

int main()
{
	//��1������ͼ����ʾ��ת��Ϊ�Ҷ�ͼ
	Mat trainImageOld;
	Mat trainImage = imread("10.jpg"), trainImage_gray;
	//resize(trainImageOld, trainImage, Size(trainImageOld.cols / 5, trainImageOld.rows / 5), 0, 0, INTER_LINEAR);

	imshow("ԭʼͼ", trainImage);
	cvtColor(trainImage, trainImage_gray, CV_BGR2GRAY);

	//��2�����Surf�ؼ��㡢��ȡѵ��ͼ��������
	vector<KeyPoint> train_keyPoint;
	Mat trainDescriptor;
	SurfFeatureDetector featureDetector(80);
	featureDetector.detect(trainImage_gray, train_keyPoint);
	SurfDescriptorExtractor featureExtractor;
	featureExtractor.compute(trainImage_gray, train_keyPoint, trainDescriptor);

	//��3����������FLANN��������ƥ�����
	FlannBasedMatcher matcher;
	vector<Mat> train_desc_collection(1, trainDescriptor);
	matcher.add(train_desc_collection);
	matcher.train();

	int MatchCnt=0;
	Mat testImage, testImage_gray,testImage0;
	testImage = imread("12.jpg");
	imshow("Camera", testImage);

	//<2>ת��ͼ�񵽻Ҷ�
	cvtColor(testImage, testImage_gray, CV_BGR2GRAY);

	//<3>���S�ؼ��㡢��ȡ����ͼ��������
	vector<KeyPoint> test_keyPoint;
	Mat testDescriptor;
	featureDetector.detect(testImage_gray, test_keyPoint);
	featureExtractor.compute(testImage_gray, test_keyPoint, testDescriptor);

	//<4>ƥ��ѵ���Ͳ���������
	vector<vector<DMatch> > matches;
	matcher.knnMatch(testDescriptor, matches, 2);

	// <5>���������㷨��Lowe's algorithm�����õ������ƥ���
	vector<DMatch> goodMatches;
	for (unsigned int i = 0; i < matches.size(); i++)
	{
		if (matches[i][0].distance < 0.6 * matches[i][1].distance)
		{
			goodMatches.push_back(matches[i][0]);
			MatchCnt++;
		}

	}

	//<6>����ƥ��㲢��ʾ����
	Mat dstImage;
	drawMatches(testImage, test_keyPoint, trainImage, train_keyPoint, goodMatches, dstImage);
	imshow("ƥ�䴰��", dstImage);

	cout << "ƥ������" << MatchCnt << endl;
	waitKey(0);

	return 0;
}