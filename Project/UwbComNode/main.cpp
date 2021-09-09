#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  /*数据类型*/
#include <sys/stat.h>   /*定义了一些返回值的结构*/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <string.h>
#include <math.h>
#include <iostream> 
#include <fstream> 

#define ComAddress  "/dev/ttyUSB0"
#define MemoryData 5
struct coordinate
{
    float x;
    float y;
};
coordinate Base[3] = { { 0,0 },{ 1,0 },{ 0,1 } };
/**
*@brief  设置串口属性
*/
void setCom(int fd) 
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial");
    }
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;/*设置字符大小*/
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB;//stop bits
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;/*设置等待时间和最小接收字符*/
    tcflush(fd, TCIFLUSH);/*处理未接收字符*/
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("com set error!\n");
    }
    std::cout << "Configure Completed" << std::endl;
}

/**
*@ 将数据从接收的串口信息中提取出来
* buff：串口信息
* an：目标信息
* s：信息位置
* p：信息存储位置
*/
void GetData(char* buff,float* an,char* s,int p)
{
    char data[10];
    if (strstr(buff, s) != NULL)
    {
        strncpy(data, strstr(buff, s) + 4, 5);
        data[5] = '\0';
        /*printf("%s", data);
        printf("atof%f\n", atof(data));*/
        //an[p] = atof(data);
        an[p] = (float)((((int)data[0]) - 48) + (((int)data[2]) - 48) * 0.1 + (((int)data[3]) - 48) * 0.01 + (((int)data[4]) - 48) * 0.001);
    }
    else
        an[p] = 0;

}

float average(float an[MemoryData])
{
    int t = 0;
    float dis = 0;
    for (int i = 0; i < MemoryData; i++)
    {
        if (an[i] != 0)
        {
            dis += an[i];
            t++;
        }
    }
    if (t != 0)
        dis = dis / t;
    return dis;
}

coordinate ThreeCheck(float dis[])
{
    coordinate point = { 0,0 };
    for (int i = 0; i < 3; i++)
        if (dis[i] <= 0)
            return point;

    for (int i = 0; i < 3; i++)
    {
        for (int j = i + 1; j < 3; j++)
        {
            //圆心距离PQ
            float p2p = (float)sqrt((Base[i].x - Base[j].x) * (Base[i].x - Base[j].x) + (Base[i].y - Base[j].y) * (Base[i].y - Base[j].y));
            //判断两圆是否相交
            if (dis[i] + dis[j] <= p2p)
            {
                //不相交，按比例求
                point.x += Base[i].x + (Base[j].x - Base[i].x) * dis[i] / (dis[i] + dis[j]);
                point.y += Base[i].y + (Base[j].y - Base[i].y) * dis[i] / (dis[i] + dis[j]);
            }
            else
            {
                //相交则套用公式
                //PC
                float dr = p2p / 2 + (dis[i] * dis[i] - dis[j] * dis[j]) / (2 * p2p);
                //x = xp + (xq-xp) * PC / PQ
                point.x += Base[i].x + (Base[j].x - Base[i].x) * dr / p2p;
                //y = yp + (yq-yp) * PC / PQ
                point.y += Base[i].y + (Base[j].y - Base[i].y) * dr / p2p;
            }
        }
    }
    point.x /= 3;
    point.y /= 3;
    return point;
}

int main()
{
    printf("Link Successed\n"); 
    int fd;
    int nread;
    /*以读写方式打开串口*/
    while (1)
    {
        fd = open(ComAddress, O_RDONLY | O_NOCTTY);
        if (fd < 0)//失败
        {
            perror("Can't Open Serial Port\n");
            sleep(1);
            continue;
        }
        std::cout << "Open Serial Port Successful\n";
        break;
    }
    /*设置串口属性*/
    setCom(fd);
    /*进行读取操作*/
    char buff[50];
    float an0[MemoryData], an1[MemoryData], an2[MemoryData];
    memset(an0, 0, sizeof(an0));
    memset(an1, 0, sizeof(an1));
    memset(an2, 0, sizeof(an2));
    float dis[3] = { 0,0,0 };
    int p=0;
    float ave; 
    coordinate point = { 0,0 };

    while (1)
    {
        if ((nread = read(fd, buff, 50)) > 0)
        {
            buff[nread] = '\0';
            //printf("%s", buff);
            GetData(buff, an0, "an0", p);
            GetData(buff, an1, "an1", p);
            GetData(buff, an2, "an2", p);
            if (++p == 5)
            {
                p = 0;
            }
        }
        ave = average(an0);
        if (ave > 0)
            dis[0] = ave;
        ave = average(an1);
        if (ave > 0)
            dis[1] = ave;
        ave = average(an2);
        if (ave > 0)
            dis[2] = ave;
        //printf("0:%3.3f 1:%3.3f 2:%3.3f\n", dis0, dis1, dis2);
        point = ThreeCheck(dis);
        printf("X:%3.2f Y:%3.2f\n", point.x, point.y);
    }
    close(fd);
    return 0;
}