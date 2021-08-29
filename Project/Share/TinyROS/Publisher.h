#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"
#include "Exceptions.h"

namespace TinyROS
{
	
	class PublisherImplement
	{
	public:
		~PublisherImplement();
		class PublisherInnerNetwork;
	private:
		void Init(); // 单独初始化，避免构造函数抛异常导致无法析构
		void InnerPublish(const char* buf, int len);
	private:
		// 虽然这个类不是模板，但是放在Publisher内部的话，定义还是会带模板
		// 为了把定义放到源文件里，暂且搞成这样，把类拉到Publisher外面，把构造函数声明为private，再把Publiser声明为friend，
		// 最后还得再套一层内部类的壳，主要是不想引入socket、thread相关的头文件，不知道有没有更好的办法
		PublisherImplement() = delete;
		PublisherImplement(const char* topicName, TypeIDHash typdIdHash);
		template<typename TMessage> friend class Publisher;
		PublisherInnerNetwork* innerImpl;
	};
	

	template<typename TMessage>
	class Publisher
	{
	public:
		Publisher(const char* topicName);
		Publisher(const Publisher&) = delete;
		Publisher& operator=(const Publisher&) = delete;
		~Publisher();
	public:
		// 发布消息
		// useRef指定是否使用msg的引用，区别不大
		void Publish(TMessage& msg, bool useRef = true);
	private:
		void PublishRef(TMessage& msg);
		void PublishNoRef(TMessage msg);
		PublisherImplement* impl;
	};

	template<typename TMessage>
	inline Publisher<TMessage>::Publisher(const char* topicName)
	{
		
		TypeID typedId(typeid(TMessage));
		TypeIDHash typeIdHash = typedId.hash_code();
		this->impl = new PublisherImplement(topicName, typeIdHash);
		try
		{
			this->impl->Init();
		}
		catch (TinyROSException& e)
		{
			delete this->impl;
			throw e;
		}
	}

	template<typename TMessage>
	inline Publisher<TMessage>::~Publisher()
	{
		delete this->impl;
	}

	template<typename TMessage>
	inline void Publisher<TMessage>::Publish(TMessage& msg, bool useRef)
	{
		if (useRef)
		{
			this->PublishRef(msg);
		}
		else
		{
			this->PublishNoRef(msg);
		}
	}

	template<typename TMessage>
	inline void Publisher<TMessage>::PublishRef(TMessage& msg)
	{
		std::string s = msg.Serialize();
		this->impl->InnerPublish(s.c_str(), s.size());
	}

	template<typename TMessage>
	inline void Publisher<TMessage>::PublishNoRef(TMessage msg)
	{
		std::string s = msg.Serialize();
		this->impl->InnerPublish(s.c_str(), s.size());
	}


}