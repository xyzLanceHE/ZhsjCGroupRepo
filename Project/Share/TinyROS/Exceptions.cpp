#include "Exceptions.h"

namespace TinyROS
{
	TinyROSException::TinyROSException()
	{
		this->whatMessage = "TinyROS功能相关的模块引发了异常";
	}

	TinyROSException::TinyROSException(const char* whatMessage)
	{
		this->whatMessage = whatMessage;
	}

	TinyROSException::TinyROSException(std::string& whatMessage)
	{
		this->whatMessage = whatMessage;
	}

	const char* TinyROSException::what() noexcept
	{
		return this->whatMessage.c_str();
	}

	//const char* NodeInitializeFailedException::what() noexcept
	//{
	//	return this->whatMessage.c_str();
	//}
}
