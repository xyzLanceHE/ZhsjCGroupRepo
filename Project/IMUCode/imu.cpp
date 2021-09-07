#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h> 
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include    <string.h>
#include    <iostream>

#define DEV_NAME  "/dev/ttyUSB0"//数字随机更改
//#define uint8_t unsigned char
//#define uint16_t unsigned int

 //---定义数据包结构体
typedef struct 
{
    uint8_t tag; /* 数据标签:0x91 */
    uint8_t id; /* 模块ID */
    uint8_t rev[2];
    float prs; /* 气压 */
    uint32_t ts; /* 时间戳 */
    float acc[3]; /* 加速度 */
    float gyr[3]; /* 角速度 */
    float mag[3]; /* 地磁 */
    float eul[3]; /* 欧拉角:
    Roll,Pitch,Yaw */
    float quat[4]; /* 四元数 */
}id0x91_t;
id0x91_t  dat;


int main (int argc, char *argv[])
{
    //打开串口
    int fd; 
    while(1)
    {
        fd = open(DEV_NAME, O_RDONLY | O_NOCTTY);
        if(fd < 0)//失败
        {
            perror("Can't Open Serial Port\n");
            sleep(1);
            continue;
        }
        std::cout<<"串口打开成功"<<std::endl;
        break; 
    }
    //串口设置
    struct termios options;
    if( tcgetattr( fd,&options)  !=  0)  
    {  
        perror("SetupSerial"); 
    }
    options.c_cflag  |=  CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;/*设置字符大小*/
    options.c_cflag |= CS8;
    options.c_cflag &=  ~CSTOPB;//stop bits
    cfsetispeed(&options,B115200);
    cfsetospeed(&options,B115200);
    options.c_cc[VTIME]  = 0;
    options.c_cc[VMIN] = 0;/*设置等待时间和最小接收字符*/
    tcflush(fd,TCIFLUSH);/*处理未接收字符*/
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
    {  
        perror("com set error!\n");    
    }
    std::cout<<"串口配置完成"<<std::endl;

    //数据读取
    unsigned char buf[82];
    while(1)
    {
        memset(buf,0,82);
        read(fd, buf, sizeof(buf));
        if(*(buf) == 0x5A && *(buf+1) ==  0xA5 && *(buf+2) == 0x4C && *(buf+3) == 0x00)
        {
            if(*(buf+6) != 0x91)
            {
                continue;
            }
            std::cout<<"帧头符合"<<std::endl;
            printf("headid%d\n",*(buf+6));
            memcpy(&dat,(buf+6),sizeof(id0x91_t));
            printf("quat:%f\n",dat.quat[1]);
            
            //publisher
            
            //运行频率
            sleep(0.1);
        }
    }
    //回收资源
    close(fd);
    return(0);
}
 
