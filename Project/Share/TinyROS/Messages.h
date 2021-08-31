#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"
#include "LibWrapper.h"
#include "Exceptions.h"

//#define ENABLE_PARALLEL_CALLBACK
#ifdef ENABLE_PARALLEL_CALLBACK
#include <thread>
#endif // ENABLE_PARALLEL_CALLBACK



namespace TinyROS
{
	
	class SHA256ValueComparator
	{
	public:
		bool operator()(const SHA256Value& left, const SHA256Value& right) const;
	};

	struct MasterBroadcastDatagram
	{
		int ListenPort;
		int Signal;	
	};

	class Message
	{
	public:
		virtual TypeIDHash GetTypeID() = 0;
		virtual std::string Serialize() = 0;
		// 原位反序列化
		virtual void Deserialize(std::string&) = 0;
		// 在新的对象中序列化并返回，该对象由调用方回收
		virtual Message* NewDeserialize(std::string&) = 0;
		virtual ~Message() {}
	};


	// 直接使用对象的内存进行序列化/反序列化，
	// 因此，只能是一些简单的类型，不能包含指针、容器、静态成员等
	// 否则，行为是不可预料的
	template<typename TValue>
	class SimpleObjectMessage : public Message
	{
	public:
		virtual TypeIDHash GetTypeID() override;
		virtual std::string Serialize() override;
		virtual void Deserialize(std::string& str) override;
		virtual Message* NewDeserialize(std::string& str) override;
		virtual ~SimpleObjectMessage() {}
	public:
		SimpleObjectMessage();
		SimpleObjectMessage(TValue value);
		TValue Value;
	private:
		static TypeIDHash TypeHash;
	};

	template<typename TValue>
	TypeIDHash SimpleObjectMessage<TValue>::TypeHash = GetSHA("SimpleObjectMessage", sizeof("SimpleObjectMessage"));
	
	template<typename TValue>
	inline TypeIDHash SimpleObjectMessage<TValue>::GetTypeID()
	{
		// 暂时不能区分不同的模板实例
		return SimpleObjectMessage::TypeHash;
	}

	template<typename TValue>
	inline std::string SimpleObjectMessage<TValue>::Serialize()
	{
		constexpr int len = sizeof(TValue);
		char buf[len];
		char* pValue = reinterpret_cast<char*>(&this->Value);
		std::copy(pValue, pValue + len, buf);
		return std::string(buf, len);
	}

	template<typename TValue>
	inline void SimpleObjectMessage<TValue>::Deserialize(std::string& str)
	{
		constexpr int len = sizeof(TValue);
		char* pValue = reinterpret_cast<char*>(&this->Value);
		const char* buf = str.c_str();
		std::copy(buf, buf + len, pValue);
	}

	template<typename TValue>
	inline Message* SimpleObjectMessage<TValue>::NewDeserialize(std::string& str)
	{
		SimpleObjectMessage* pMsg = new SimpleObjectMessage();
		pMsg->Deserialize(str);
		return pMsg;
	}

	template<typename TValue>
	inline SimpleObjectMessage<TValue>::SimpleObjectMessage()
	{
		this->Value = TValue();
	}

	template<typename TValue>
	inline SimpleObjectMessage<TValue>::SimpleObjectMessage(TValue value)
	{
		this->Value = value;
	}



	// 提供对简单类型的对象数组的支持
	// 数组不论多长，都属于同一种类型的消息，
	// 但长度为1的数组与简单对象消息属于不同类型的消息
	// 发布和订阅时，请注意消息类型的匹配
	template<typename TValue>
	class SimpleObjectArrayMessage : public Message
	{
	public:
		virtual TypeIDHash GetTypeID() override;
		virtual std::string Serialize() override;
		virtual void Deserialize(std::string& str) override;
		virtual Message* NewDeserialize(std::string& str) override;
		virtual ~SimpleObjectArrayMessage();
	public:
		SimpleObjectArrayMessage();
		SimpleObjectArrayMessage(TValue* pValue, int count);
		SimpleObjectArrayMessage(const SimpleObjectArrayMessage& other);
		SimpleObjectArrayMessage(SimpleObjectArrayMessage&& other);
		SimpleObjectArrayMessage& operator= (const SimpleObjectArrayMessage& other);
		// 该方法直接访问内部数据，不建议使用
		// 不能删除通该方法返回的指针
		TValue* GetValuePointer();
		int GetCount();
		// 不进行越界检查，越界会造成未定义的结果
		TValue operator[](int index);
	private:
		TValue* pValue;
		int count;
	private:
		static TypeIDHash TypeHash;
	};

	template<typename TValue>
	TypeIDHash SimpleObjectArrayMessage<TValue>::TypeHash = GetSHA("SimpleObjectArrayMessage", sizeof("SimpleObjectArrayMessage"));

	template<typename TValue>
	inline TypeIDHash SimpleObjectArrayMessage<TValue>::GetTypeID()
	{

		// 暂时不能区分不同的模板实例
		return SimpleObjectArrayMessage::TypeHash;
	}

	template<typename TValue>
	inline std::string SimpleObjectArrayMessage<TValue>::Serialize()
	{
		if (this->count == 0)
		{
			return std::string();
		}
		constexpr int unitLen = sizeof(TValue);
		int totalLen = unitLen * this->count;
		char* p = reinterpret_cast<char*>(this->pValue);
		return std::string(p, totalLen);
	}

	template<typename TValue>
	inline void SimpleObjectArrayMessage<TValue>::Deserialize(std::string& str)
	{
		delete[] this->pValue;
		int count = str.size() / sizeof(TValue);
		if (count == 0)
		{
			this->pValue = nullptr;
			this->count = 0;
			return;
		}
		this->pValue = new TValue[count];
		this->count = count;
		char* p = reinterpret_cast<char*>(this->pValue);
		std::copy(str.c_str(), str.c_str() + sizeof(TValue) * count, p);
	}

	template<typename TValue>
	inline Message* SimpleObjectArrayMessage<TValue>::NewDeserialize(std::string& str)
	{
		SimpleObjectArrayMessage* pMsg = new SimpleObjectArrayMessage();
		pMsg->Deserialize(str);
		return pMsg;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::~SimpleObjectArrayMessage()
	{
		delete[] this->pValue;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage()
	{
		this->pValue = nullptr;
		this->count = 0;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage(TValue* pValue, int count)
	{
		this->pValue = new TValue[count];
		this->count = count;
		std::copy(pValue, pValue + count, count);
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage(const SimpleObjectArrayMessage& other)
	{
		this->count = other.count;
		if (this->count == 0)
		{
			this->pValue = nullptr;
		}
		else
		{
			this->pValue = new TValue[this->count];
			std::copy(other.pValue, other.pValue + other.count, this->pValue);
		}
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage(SimpleObjectArrayMessage&& other)
	{
		this->count = other.count;
		this->pValue = other.pValue;
		other.pValue = nullptr;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>& SimpleObjectArrayMessage<TValue>::operator=(const SimpleObjectArrayMessage& other)
	{
		delete[] this->pValue;
		// 长得和拷贝构造很像
		this->count = other.count;
		if (this->count == 0)
		{
			this->pValue = nullptr;
		}
		else
		{
			this->pValue = new TValue[count];
			std::copy(other.pValue, other.pValue + other.count, this->pValue);
		}
		return *this;
	}

	template<typename TValue>
	inline TValue* SimpleObjectArrayMessage<TValue>::GetValuePointer()
	{
		return this->pValue;
	}

	template<typename TValue>
	inline int SimpleObjectArrayMessage<TValue>::GetCount()
	{
		return this->count;
	}

	template<typename TValue>
	inline TValue SimpleObjectArrayMessage<TValue>::operator[](int index)
	{
		return this->pValue[index];
	}


	class StringMessage : public Message
	{
	public:
		virtual TypeIDHash GetTypeID() override;
		virtual std::string Serialize() override;
		virtual void Deserialize(std::string& str) override;
		virtual Message* NewDeserialize(std::string& str) override;
		virtual ~StringMessage() {}
	public:
		StringMessage();
		StringMessage(std::string& str);
		StringMessage(const char* str);
		StringMessage(const StringMessage& other);
		std::string GetValue();
	private:
		std::string Value;
	private:
		static TypeIDHash TypeHash;
	};

	enum class InvokeType { Sequential, Parallel };

	class IMessageCallable
	{
	public:
	    virtual	void Invoke(Message& msg) = 0;
		virtual void SetInvokeType(InvokeType ivType) = 0;
	};

	// 模仿了一下C#的多播委托，可以回调多个函数
	// 改进版，模板拉到了类上面，能够做到保证与Subscriber匹配，Register/Unregister也不再需要填模板参数
	template<typename TMessage>
	class MulticastMessageDelegate : public IMessageCallable
	{
		private:
		class ITMessageCallable
		{
		public:
			virtual void Invoke(TMessage msg) = 0;
			virtual bool Equals(ITMessageCallable* other) = 0;
			virtual ~ITMessageCallable() {}
		};

		using NormalCallback = void (*)(TMessage);
		class NormalMessageDelegate : public ITMessageCallable
		{
		public:
			NormalMessageDelegate(NormalCallback callback)
			{
				this->CallbackFunction = callback;
			}
			~NormalMessageDelegate() {} // 类的所有资源都不属于自己，不要delete，后面两个类同理
		public:
			virtual void Invoke(TMessage msg) override
			{
				(*this->CallbackFunction)(msg);
			}
			virtual bool Equals(ITMessageCallable* other) override
			{
				try
				{
					NormalMessageDelegate* pOther = dynamic_cast<NormalMessageDelegate*>(other);
					return this->CallbackFunction == pOther->CallbackFunction;
				}
				catch (std::exception&)
				{
					return false;
				}
			}
		private:
			NormalCallback CallbackFunction;
		};

		// 对象的成员函数
		template<typename TObject>
		using ObjectMemberCallback = void (TObject::*)(TMessage);
		template<typename TObject>
		class ObjectMemberMessageDelegate : public ITMessageCallable
		{
		public:
			ObjectMemberMessageDelegate(ObjectMemberCallback<TObject> callback, TObject* obj)
			{
				this->Obj = obj;
				this->MemberCallbackFunction = callback;
			}
			~ObjectMemberMessageDelegate() {}
		public:
			virtual void Invoke(TMessage msg) override
			{
				(this->Obj->*MemberCallbackFunction)(msg);
			}
			virtual bool Equals(ITMessageCallable* other) override
			{
				try
				{
					ObjectMemberMessageDelegate* pOther = dynamic_cast<ObjectMemberMessageDelegate*>(other);
					return this->Obj == pOther->Obj;
				}
				catch (std::exception&)	// 实际上捕获的是std::bad_cast，但是需要引入<typeinfo>，所以直接捕获基类了
				{
					return false;
				}
			}
		private:
			TObject* Obj;
			ObjectMemberCallback<TObject> MemberCallbackFunction;
		};

		// 函数对象
		template<typename TObject>
		class FunctionObjectMessageDelegate : public ITMessageCallable
		{
		public:
			FunctionObjectMessageDelegate(TObject& obj)
			{
				this->pObj = &obj;
			}
			~FunctionObjectMessageDelegate() {}
		public:
			virtual void Invoke(TMessage msg) override
			{
				// this->pObj->operator()(typedMessage);
				(*this->pObj)(msg);
			}
			virtual bool Equals(ITMessageCallable* other) override
			{
				try
				{
					FunctionObjectMessageDelegate* pOther = dynamic_cast<FunctionObjectMessageDelegate*>(other);
					return this->pObj == pOther->pObj;
				}
				catch (std::exception&)
				{
					return false;
				}
			}
		private:
			TObject* pObj;
		};

	public:
		MulticastMessageDelegate(int maxCall = 3)
		{
			this->RegisteredCount = 0;
			this->MaxCount = maxCall;
			this->Delegates = new PointerITMessageCallable[maxCall]{ nullptr };
		}
		MulticastMessageDelegate(const MulticastMessageDelegate&) = delete;
		MulticastMessageDelegate(MulticastMessageDelegate&&) = delete;
		MulticastMessageDelegate& operator=(MulticastMessageDelegate&) = delete;
		~MulticastMessageDelegate()
		{
			for (int i = 0; i < this->RegisteredCount; i++)
			{
				delete this->Delegates[i];
			}
		}

		// 注册一个普通函数或者静态成员函数
		void Register(NormalCallback funtion)
		{
			this->ThrowIfMax();
			ITMessageCallable* callback = new NormalMessageDelegate(funtion);
			this->Delegates[this->RegisteredCount] = callback;
			this->RegisteredCount++;
		}

		// 注册一个对象的成员函数,Tobject可以自动推导
		template<typename TObject>
		void Register(ObjectMemberCallback<TObject> memeberFunction, TObject& obj)
		{
			// 这个函数有两个参数，没法重载+=号，所以其他的也不重载了
			this->ThrowIfMax();
			ITMessageCallable* callback = new ObjectMemberMessageDelegate<TObject>(memeberFunction, &obj);
			this->Delegates[this->RegisteredCount] = callback;
			this->RegisteredCount++;
		}

		// 注册一个函数对象,Tobject可以自动推导
		template<typename TObject>
		void Register(TObject& obj)
		{
			this->ThrowIfMax();
			ITMessageCallable* callback = new FunctionObjectMessageDelegate<TObject>(obj);
			this->Delegates[this->RegisteredCount] = callback;
			this->RegisteredCount++;
		}

		// 取消注册一个普通函数或者静态成员函数
		void Unregister(NormalCallback funtion)
		{
			ITMessageCallable* callback = new NormalMessageDelegate(funtion);
			this->FindAndRemove(callback);
		}

		// 取消注册一个对象的成员函数
		template<typename TObject>
		void Unregister(ObjectMemberCallback<TObject> memeberFunction, TObject& obj)
		{
			ITMessageCallable* callback = new ObjectMemberMessageDelegate<TObject>(memeberFunction, &obj);
			this->FindAndRemove(callback);
		}

		// 取消注册一个函数对象
		template<typename TObject>
		void Unregister(TObject& obj)
		{
			ITMessageCallable* callback = new FunctionObjectMessageDelegate<TObject>(obj);
			this->FindAndRemove(callback);
		}

		virtual void Invoke(Message& msg) override
		{
			TMessage& typdMessage = *dynamic_cast<TMessage*>(&msg);
			TMessage msgCopy(typdMessage);
			this->InvokeAll(msgCopy);
		}

		void SetInvokeType(InvokeType type) override
		{
			this->invokeType = type;
		}

	private:
		using PointerITMessageCallable = ITMessageCallable*;
		PointerITMessageCallable* Delegates;
		int RegisteredCount;
		int MaxCount;
		InvokeType invokeType = InvokeType::Sequential;

	private:
		inline void ThrowIfMax()
		{
			if (this->RegisteredCount == this->MaxCount)
			{
				throw CommunicateException("注册的回调函数达到上限");
			}
		}
		inline void FindAndRemove(ITMessageCallable* pMessageCallable)
		{
			int index = -1;
			for (int i = 0; i < this->RegisteredCount; i++)
			{
				if (this->Delegates[i]->Equals(pMessageCallable))
				{
					delete this->Delegates[i];
					this->Delegates[i] = nullptr;
					delete pMessageCallable;
					pMessageCallable = nullptr;
					index = i;
					break;
				}
			}
			if (index != -1)
			{
				for (int i = index; i < this->RegisteredCount - 1; i++)
				{
					this->Delegates[i] = this->Delegates[i + 1];
				}
				this->RegisteredCount--;
			}
		}

		void InvokeAll(TMessage& msg)
		{
			if (this->invokeType == InvokeType::Sequential)
			{
				for (int i = 0; i < this->RegisteredCount; i++)
				{
					this->InvokeAt(i, msg);
				}
			}
			else if (this->invokeType == InvokeType::Parallel)
			{
#ifdef ENABLE_PARALLEL_CALLBACK
				// 稳定性未知
				std::thread** callbackThreads = new std::thread*[this->RegisteredCount];
				for (int i = 0; i < this->RegisteredCount; i++)
				{
					std::thread callbackThread(&MulticastMessageDelegate::InvokeAt, this, i, msg);
					callbackThreads[i] = &callbackThread;
				}
				for (int i = 0; i < this->RegisteredCount; i++)
				{
					if (callbackThreads[i]->joinable())
					{
						callbackThreads[i]->join();
					}
				}
#else
				// throw CommunicateException("未启用并行回调");
				for (int i = 0; i < this->RegisteredCount; i++)
				{
					this->Delegates[i]->Invoke(msg);
				}
#endif 
			}
		}

		void InvokeAt(int index, TMessage& msg)
		{
			this->Delegates[index]->Invoke(msg);
		}

	private:
		void TypeTrial()
		{
			// 这个函数不被使用，但是可以保证如下性质成立：
			// TMessage是Message的子类，
			// TMessage有无参构造函数
			// TMessage有拷贝构造函数
			// 否则会过不了编译
			TMessage* pTMsg = new TMessage();
			Message* pMsg = dynamic_cast<Message*>(pTMsg);
			TMessage tMsg(*pTMsg);
		}
	};

	template<typename TMessage>
	using MessageCallback = MulticastMessageDelegate<TMessage>;
}