#pragma once
#include "TinyROSPlatformDef.h"
#include <stdexcept>
#include <string>

namespace TinyROS
{
	class TinyROSException : public std::exception
	{
	public:
		TinyROSException();
		TinyROSException(const char* whatMessage);
		TinyROSException(std::string& whatMessage);
		virtual const char* what() noexcept;
		virtual ~TinyROSException() {}
	protected:
		std::string whatMessage;
	};

	class NodeInitializeFailedException : public TinyROSException
	{
	public:
		using TinyROSException::TinyROSException;
		virtual const char* what() noexcept;
		virtual ~NodeInitializeFailedException() {}
	};
}