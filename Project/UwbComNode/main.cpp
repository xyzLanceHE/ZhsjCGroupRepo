#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  /*数据类型*/
#include <sys/stat.h>   /*定义了一些返回值的结构*/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <string.h>
#include <iostream> 
#include <fstream> 

#define ComAddress  "/dev/ttyUSB0"
#define MemoryData 5

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
    memset(an0, 0, MemoryData);
    memset(an1, 0, MemoryData);
    memset(an2, 0, MemoryData);
    float dis0=0, dis1=0, dis2=0;
    int p=0,t;

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
        dis0 = average(an0);
        dis1 = average(an1);
        dis2 = average(an2);
        printf("0:%3.3f 1:%3.3f 2:%3.3f\n", dis0, dis1, dis2);
    }
    close(fd);
    return 0;
}