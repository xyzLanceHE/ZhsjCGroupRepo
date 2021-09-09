#pragma once

namespace RoboTax
{

	class CarControllerInterface
	{
	public:
		CarControllerInterface(const char* serialPortName);
		~CarControllerInterface();
		void SetSpeed(float linear, float radius);
	private:
		class CarControllerInterfaceImplement;
		CarControllerInterfaceImplement* impl;
	};

}