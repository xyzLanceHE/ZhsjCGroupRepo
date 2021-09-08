#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  /*数据类型*/
#include <sys/stat.h>   /*定义了一些返回值的结构*/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <string.h>

int OpenDev(char* Dev)
{
    //Dev 就是设备，设备就是文件，就是给出该设备文件的路径
    int fd = open(Dev, O_RDWR); //| O_NOCTTY | O_NDELAY
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return -1;
    }
    else
        return fd;
}

/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                  B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300,
                  38400, 19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed) {
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
        if (speed == name_arr[i]) {
            /**
             * tcflush函数刷清(抛弃)输入缓存(终端驱动程序已接收到，但用户程序尚未读)或输出缓存(用户程序已经写，但尚未发送)。queue参数应是下列三个常数之一：
            * TCIFLUSH刷清输入队列。
             * TCOFLUSH刷清输出队列。
            * TCIOFLUSH刷清输入、输出队列。
             */
            tcflush(fd, TCIOFLUSH);//设置前flush    
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            //通过tcsetattr函数把新的属性设置到串口上。
            //tcsetattr(串口描述符，立即使用或者其他标示，指向termios的指针)
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0)
            {
                perror("tcsetattr fd");
                return;
            }
            tcflush(fd, TCIOFLUSH);  //设置后flush
        }
    }
}

/**
 *@brief   设置串口数据位，停止位和效验位
 *@param  fd     类型  int  打开的串口文件句柄
 *@param  databits 类型  int 数据位   取值 为 7 或者8
 *@param  stopbits 类型  int 停止位   取值为 1 或者2
 *@param  parity  类型  int  效验类型 取值为N,E,O,S
 */
int set_Parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1");
        return (false);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
        /*设置数据位数*/
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return (false);
    }
    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB; /* Clear parity enable */
        options.c_iflag &= ~INPCK; /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB; /* Enable parity */
        options.c_cflag &= ~PARODD; /* 转换为偶效验*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'S':
    case 's': /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return (false);
    }
    /* 设置停止位*/
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return (false);

    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return (false);
    }
    return (true);
}

int main()
{
    int fd;
    int nread;
    char buff[100];
    /*以读写方式打开串口*/
    char *ComAddress = "/dev/ttyS2";
    fd = OpenDev(ComAddress);
    /*设置串口属性*/
    set_speed(fd, 115200);
    if (set_Parity(fd, 8, 1, 'N') == false)
    {
        printf("Set Parity Error\n");
        return 0;
    }
    /*进行读取操作*/
    while (1)
    {
        while ((nread = read(fd, buff, 512)) > 0)
        {
            printf("\nLen %d\n", nread);
            buff[nread + 1] = '\0';
            printf("\n%s", buff);
        }
    }
    close(fd);
    return 0;
}