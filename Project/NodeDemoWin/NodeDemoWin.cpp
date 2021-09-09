#include <iostream>
// 需要进行的准备：（本Demo已经设置好了，可以参考）
// 将Share项目添加为本项目的引用，即可直接访问到Share下的资源
// 预处理器中预定义宏 TINYROS_ON_WINDOWS，否则会报错
// 项目中添加资源文件 libcrypto_static_x64.lib （Share/OpenSSL/lib下），否则会链接错误
#include<opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc.hpp>
#include <opencv2\imgproc\types_c.h>
#include<stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<opencv2/opencv_modules.hpp>
#include "opencv2/imgproc/imgproc_c.h"
using namespace cv;
using namespace std;
#include "TinyROS/TinyROS.h"
#include <thread>

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

int main()
{

    // TinyROS的所有内容都位于TinyROS命名空间下，（不建议using命名空间，避免冲突）
    // 关于Node：此类用于将本进程初始化为TinyROS的节点，请在程序开始时使用
    try
    {
        // 一句话即可。参数是本节点的名字，在局域网中必须是唯一的
        TinyROS::Node::Init("Vision Publisher");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    // Node初始化完成之后，可以定义Publisher和Subscriber
    // 本Demo演示Publisher
    // Publisher的构造函数被禁止调用，需要声明指针
    TinyROS::Publisher* visionPub;
    try
    {
        // 使用NewPublisher函数申请一个新的Publisher并得到指向它的指针
        // 该函数有一个模板参数，是消息的类型
        // 函数的参数是话题名称。话题不存在时，会自动创建
        // 话题一旦创建，消息的类型是确定的，后续新的Subscriber/Publisher订阅或者发布到此话题，需要与之匹配，否则会抛异常
        visionPub = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<float>>("vision");

    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    VideoCapture capture(1);//������ͷ  
    if (!capture.isOpened())//û�д�����ͷ�Ļ����ͷ��ء�
        return -1;
    Mat edges;
    float KNOWN_WIDTH, TEST_WIDTH, Observed[20];
    int count = 0;
    cout << "\nPlease enter the obstacle width(mm):";
    cin >> KNOWN_WIDTH;
    // Publisher 成功创建之后，就可以调用Publish方法
    std::string visionStr("This is the distance");
    while (true)
    {
        Mat frame; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��  
        capture >> frame;  //��ȡ��ǰ֡   
        //imshow("Video0", frame);
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
            sort(Observed, Observed + 20);
            TEST_WIDTH = Observed[10];
            cout << "TEST_WIDTH=" << TEST_WIDTH << endl;
            count = 0;
            TinyROS::SimpleObjectMessage<float> msg;
            msg = TEST_WIDTH;
            visionPub->Publish(msg);
        }
           /* std::string msgStr = visionStr + std::to_string(D);
            TinyROS::SimpleObjectMessage<float> D;
           
            visionPub->Publish(msg);*/
        
        /*using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);*/
    }



    // 程序退出之前，关闭此Node
    TinyROS::Node::Close();
}


