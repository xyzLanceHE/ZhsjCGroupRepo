#include <stdio.h>
//#include <fcntl.h>
//#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <string>
//#include <sys/time.h>
//#include <sys/types.h>
#include <errno.h>

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
	}

	CarControllerInterface::~CarControllerInterface()
	{
		delete this->impl;
	}

	void CarControllerInterface::SetSpeed(float linear, float radius)
	{

	}

}