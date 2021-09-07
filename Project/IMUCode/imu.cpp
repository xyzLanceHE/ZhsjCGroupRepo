#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h> 
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/


#define DEV_NAME  "/dev/ttyUSB0"//数字随机更改
 
 
int main (int argc, char *argv[])
{
    //打开串口
    int fd; 
    while(1)
    {
        fd = open(DEV_NAME, O_RDWR | O_NOCTTY);
        if(fd < 0)//失败
        {
            perror("open uart device error\n");
            sleep(1);
            continue;
        }
        else//成功
        {
            //串口设置
            struct termios options;
            tcgetattr(fd,&options);
            cfsetispeed(&options,B115200);
            cfsetospeed(&options,B115200);
            if (tcsetattr(fd,TCSANOW,&options) != 0)    
            {  
                perror("com set error!\n");    
            }  
            break;
        }
    }

    //数据读取
     int len;
    char buf[82];
    len = read(fd, buf, sizeof(buf));
    if (len < 0) {
            printf("read error \n");
            return -1;
    }
    printf("%s\n", buf);

    close(fd);
    return(0);
}
 