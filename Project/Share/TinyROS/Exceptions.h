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

	class NodeException : public TinyROSException
	{
	public:
		using TinyROSException::TinyROSException;
		virtual ~NodeException() {}
	};

	class NodeInitializeFailedException : public NodeException
	{
	public:
		using NodeException::NodeException;
		//virtual const char* what() noexcept;
		virtual ~NodeInitializeFailedException() {}
	};

	class MasterLaunchFailedException : public TinyROSException
	{
	public:
		using TinyROSException::TinyROSException;
		//virtual const char* what() noexcept;
		virtual ~MasterLaunchFailedException() {}
	};

	class MasterAlreadyExistExcepiton : public MasterLaunchFailedException
	{
	public:
		MasterAlreadyExistExcepiton();
		//virtual const char* what() noexcept;
		virtual ~MasterAlreadyExistExcepiton() {}
	};

	class InvalidConfigException : public TinyROSException
	{
	public:
		using TinyROSException::TinyROSException;
		virtual ~InvalidConfigException() {}
	};

	class TopicException : public TinyROSException
	{
	public:
		using TinyROSException::TinyROSException;
		virtual ~TopicException() {}
	};
}