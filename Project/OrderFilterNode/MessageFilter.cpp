#include "TinyROS/TinyROS.h"
#include "TinyROS/SharedMessageTypes.h"
#include <thread>
#include <ctime>
#include <map> 
#include <vector>

namespace RoboTax
{
	template <typename TMessage>
	class MessageFilter
	{
	private:
		TinyROS::Publisher* pFilterMessagePublisher;
		using PriorityMessageDictionary = std::map<int, std::pair<time_t, TMessage>>;
		PriorityMessageDictionary PriorityMessagePairs;
	public:
		MessageFilter(TinyROS::Publisher* pPublisher);
	public:
		void AddToSequence(TMessage msg, int priority);
	private:
		void PollForPublish();
		inline bool IsValid(int priority);
	};


	template <typename TMessage>
	class MessageCallbackWithPriority
	{
	public:
		MessageFilter<TMessage>* pFilter;
		int Priority;
	public:
		MessageCallbackWithPriority(MessageFilter<TMessage>* pFilter, int priority);
		void operator()(TMessage msg);
	};



	template<typename TMessage>
	MessageFilter<TMessage>::MessageFilter(TinyROS::Publisher* pPublisher)
	{
		this->pFilterMessagePublisher = pPublisher;
	}

	template<typename TMessage>
	void MessageFilter<TMessage>::AddToSequence(TMessage msg, int priority)
	{
		time_t timeNow = time(nullptr);
		this->PriorityMessagePairs[priority] = std::make_pair(timeNow, msg);
		this->PollForPublish();
	}

	template<typename TMessage>
	void MessageFilter<TMessage>::PollForPublish()
	{
		int highestValid = -1;
		for (auto priMsgPair : this->PriorityMessagePairs)
		{
			int priority = priMsgPair.first;
			if (priority < highestValid)
			{
				continue;
			}
			if (this->IsValid(priority))
			{
				highestValid = priority;
			}
		}
		if (highestValid != -1)
		{
			this->pFilterMessagePublisher->Publish(this->PriorityMessagePairs[highestValid].second);
		}
	}

	template<typename TMessage>
	inline bool MessageFilter<TMessage>::IsValid(int priority)
	{
		time_t timeNow = time(nullptr);
		return (timeNow - this->PriorityMessagePairs[priority].first < (time_t)2);
	}

	template<typename TMessage>
	MessageCallbackWithPriority<TMessage>::MessageCallbackWithPriority(MessageFilter<TMessage>* pFilter, int priority)
	{
		this->pFilter = pFilter;
		this->Priority = priority;
	}

	template<typename TMessage>
	void MessageCallbackWithPriority<TMessage>::operator()(TMessage msg)
	{
		this->pFilter->AddToSequence(msg, this->Priority);
	}

	template class MessageFilter<CarVelocityMessage>;
	template class MessageCallbackWithPriority<CarVelocityMessage>;

	using VelocityMessageFilter = MessageFilter<CarVelocityMessage>;
	using PriorityVelocityCallback = MessageCallbackWithPriority<CarVelocityMessage>;
}


