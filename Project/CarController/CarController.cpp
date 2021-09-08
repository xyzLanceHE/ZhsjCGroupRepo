#include <stdio.h>
#include <fcntl.h>
//#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <string>
//#include <sys/time.h>
//#include <sys/types.h>
#include <errno.h>
#include <math.h>
#include "CarController.h"

namespace RoboTax
{


	class CarControllerInterface::CarControllerInterfaceImplement
	{
	public:
		int SerialPortFD;
		~CarControllerInterfaceImplement() 
		{
			close(this->SerialPortFD);
		}
	};


	CarControllerInterface::CarControllerInterface(const char* serialPortName)
	{
		this->impl = new CarControllerInterfaceImplement();
		
		int ret;

		this->impl->SerialPortFD = open(serialPortName, O_RDWR | O_NOCTTY | O_NDELAY);
		if (this->impl->SerialPortFD == -1)
		{
			delete this->impl;
			this->impl = nullptr;
		}

		ret = fcntl(this->impl->SerialPortFD, F_SETFL, 0);
		if (ret == -1)
		{
			delete this->impl;
			this->impl = nullptr;
		}

		termios portConfig;
		
		ret = tcgetattr(this->impl->SerialPortFD, &portConfig);
		if (ret == -1)
		{
			delete this->impl;
			this->impl = nullptr;
		}

		cfsetispeed(&portConfig, B19200);
		cfsetospeed(&portConfig, B19200);

		portConfig.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

		ret = tcsetattr(this->impl->SerialPortFD, TCSANOW, &portConfig);
		if (ret == -1)
		{
			delete this->impl;
			this->impl = nullptr;
		}

	}

	CarControllerInterface::~CarControllerInterface()
	{
		delete this->impl;
	}

	void CarControllerInterface::SetSpeed(float linear, float radius)
	{
		float rmm = std::abs(radius * 1000);
		float ctr = 0;
		if (rmm < 9999.0f)
		{
			if (radius <= 0.0f)
			{
				ctr = 1;
			}
			else
			{
				ctr = 2;
			}
		}
		int l = ((int)(linear * 1000)) % 1000;
		int r = ((int)rmm) % 10000;
		char cmd[8];
		sprintf(cmd, "%04d%1d%03d", l, ctr, r);
		write(this->impl->SerialPortFD, cmd, 8);
	}

}