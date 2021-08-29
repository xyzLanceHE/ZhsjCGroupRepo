#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"
#include "Exceptions.h"
#include "Messages.h"
#include <iostream>
namespace TinyROS
{

	class Publisher
	{
	// 通过包装好的NewPublisher函数获取对象指针
	private:
		Publisher(const char* topicName, TypeIDHash msgType);
		Publisher() = delete;
		Publisher(const Publisher&) = delete;
		Publisher(Publisher&&) = delete;
		template<typename TMessage> friend Publisher* NewPublisher(const char* name);
	public:
		void Publish(Message& msg) {
			
			std::cout << msg.GetTypeID().ToHexString() << " of 1\n";
			StringMessage m("a");
			std::cout << m.GetTypeID().ToHexString() << " of 2\n";
		}
	};

	template<typename TMessage>
	Publisher* NewPublisher(const char* name)
	{
		// 甚至不需要type_traits, 如果不是Message及其子类, dynamic_cast在编译时就会报错
		TMessage* pTemp = new TMessage();
		Message* pMsg = dynamic_cast<Message*>(pTemp);
		std::cout << pMsg->GetTypeID().ToHexString() << " of 1\n";
		StringMessage m("a");
		std::cout << m.GetTypeID().ToHexString() << " of 2\n";
		return nullptr;
	}


}