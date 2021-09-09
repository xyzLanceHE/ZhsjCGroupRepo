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

//�������������С��������
bool ascendSort(vector<Point> a, vector<Point> b)
{
	return a.size() < b.size();
}
//�������������С��������
bool descendSort(vector<Point> a, vector<Point> b) {
	return a.size() > b.size();
}
static inline bool ContoursSortFun(vector<cv::Point> contour1, vector<cv::Point> contour2)
{
	return (cv::contourArea(contour1) > cv::contourArea(contour2));
}
void main()
{
	//������ͷ������Ƶ  
	VideoCapture capture(1);//������ͷ  
	if (!capture.isOpened())
		return;
	Mat edges; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��ѭ����ʾÿһ֡ 
	float KNOWN_WIDTH, TEST_WIDTH, Observed[20];
	int count = 0;
	cout << "\nPlease enter the obstacle width(mm):";
	cin >> KNOWN_WIDTH;
	while (1)
	{
		Mat frame; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��  
		capture >> frame;  //��ȡ��ǰ֡   
		if (frame.empty())
		{
			break;
		}
		else
		{
			//waitKey(2000);����ѡ����д���֡����ʱ��
			cvtColor(frame, edges, CV_BGR2GRAY);//��ɫת���ɻҶ�  
			GaussianBlur(edges, edges, Size(3, 3), 0, 0);//ģ����  
			//Canny(edges, edges, 35, 125, 3);//��Ե��  
			threshold(edges, edges, 220, 255, CV_THRESH_BINARY);
			//imshow("Video1", edges);
			Mat mask = Mat::zeros(edges.size(), CV_8UC1);
			vector<vector<Point>>contours;
			vector<Vec4i>hierarchy;//4άint����
			findContours(edges, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);//��������
			vector<RotatedRect> rectangle(contours.size()); //��С��Ӿ���    ***��С��Ӿ��κ���С����Ӿ��λ��ǲ�һ����***
			Point2f rect[4];
			float width = 0;//��Ӿ��εĿ�͸�
			float height = 0;

			for (int i = 0; i < contours.size(); i++)
			{
				rectangle[i] = minAreaRect(Mat(contours[i]));
				rectangle[i].points(rect); //��С��Ӿ��ε�4���˵�
				width = rectangle[i].size.width;
				height = rectangle[i].size.height;
				if (height >= width)
				{
					float x = 0;
					x = height;
					height = width;
					width = x;
				}
				//cout << "��" << width << " " << "��" << height << endl;
				for (int j = 0; j < 4; j++)
				{
					//cout << "0" << rect[j] << " " << "1" << rect[(j + 1) % 4] << endl;
					line(frame, rect[j], rect[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);//������С��Ӿ��ε�ÿ����
				}
			}
			float D = (KNOWN_WIDTH * 1302.8) / width;
			Observed[count] = D;
			char tam[100];
			sprintf_s(tam, "D=:%lf", D);
			putText(frame, tam, Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 0, 255), 1, 8);
			//imshow("Video2", mask); //��ʾ��ǰ֡  
			imshow("Video", frame);

		}
		waitKey(50); //��ʱ50ms  
		count++; 
		if (count >= 20)
		{
			sort(Observed, Observed+20);
			TEST_WIDTH = Observed[10];
			cout << "TEST_WIDTH=" << TEST_WIDTH << endl;
			count = 0;
		}
	}
	capture.release();//�ͷ���Դ
	destroyAllWindows();//�ر����д���
}
