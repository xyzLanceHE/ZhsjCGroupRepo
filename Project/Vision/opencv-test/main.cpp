#include<opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>    
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2\imgproc\types_c.h>
#include<stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<opencv2/opencv_modules.hpp>
#include "opencv2/imgproc/imgproc_c.h"
using namespace cv;
using namespace std;

//轮廓按照面积大小升序排序
bool ascendSort(vector<Point> a, vector<Point> b)
{
	return a.size() < b.size();
}
//轮廓按照面积大小降序排序
bool descendSort(vector<Point> a, vector<Point> b) {
	return a.size() > b.size();
}
static inline bool ContoursSortFun(vector<cv::Point> contour1, vector<cv::Point> contour2)
{
	return (cv::contourArea(contour1) > cv::contourArea(contour2));
}
void main()
{
	//从摄像头读入视频  
	VideoCapture capture(1);//打开摄像头  
	if (!capture.isOpened())
		return;
	Mat edges; //定义一个Mat变量，用于存储每一帧的图像，循环显示每一帧 
	float KNOWN_WIDTH, TEST_WIDTH, Observed[20];
	int count = 0;
	cout << "\nPlease enter the obstacle width(mm):";
	cin >> KNOWN_WIDTH;
	while (1)
	{
		Mat frame; //定义一个Mat变量，用于存储每一帧的图像  
		capture >> frame;  //读取当前帧   
		if (frame.empty())
		{
			break;
		}
		else
		{
			//waitKey(2000);可以选择进行处理帧数的时间
			cvtColor(frame, edges, CV_BGR2GRAY);//彩色转换成灰度  
			GaussianBlur(edges, edges, Size(3, 3), 0, 0);//模糊化  
			//Canny(edges, edges, 35, 125, 3);//边缘化  
			threshold(edges, edges, 220, 255, CV_THRESH_BINARY);
			//imshow("Video1", edges);
			Mat mask = Mat::zeros(edges.size(), CV_8UC1);
			vector<vector<Point>>contours;
			vector<Vec4i>hierarchy;//4维int向量
			findContours(edges, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);//画出轮廓
			vector<RotatedRect> rectangle(contours.size()); //最小外接矩形    ***最小外接矩形和最小正外接矩形还是不一样的***
			Point2f rect[4];
			float width = 0;//外接矩形的宽和高
			float height = 0;

			for (int i = 0; i < contours.size(); i++)
			{
				rectangle[i] = minAreaRect(Mat(contours[i]));
				rectangle[i].points(rect); //最小外接矩形的4个端点
				width = rectangle[i].size.width;
				height = rectangle[i].size.height;
				if (height >= width)
				{
					float x = 0;
					x = height;
					height = width;
					width = x;
				}
				//cout << "宽" << width << " " << "高" << height << endl;
				for (int j = 0; j < 4; j++)
				{
					//cout << "0" << rect[j] << " " << "1" << rect[(j + 1) % 4] << endl;
					line(frame, rect[j], rect[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);//绘制最小外接矩形的每条边
				}
			}
			float D = (KNOWN_WIDTH * 1302.8) / width;
			Observed[count] = D;
			char tam[100];
			sprintf_s(tam, "D=:%lf", D);
			putText(frame, tam, Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 0, 255), 1, 8);
			//imshow("Video2", mask); //显示当前帧  
			imshow("Video", frame);

		}
		waitKey(50); //延时50ms  
		count++; 
		if (count >= 20)
		{
			sort(Observed, Observed+20);
			TEST_WIDTH = Observed[10];
			cout << "TEST_WIDTH=" << TEST_WIDTH << endl;
			count = 0;
		}
	}
	capture.release();//释放资源
	destroyAllWindows();//关闭所有窗口
}
