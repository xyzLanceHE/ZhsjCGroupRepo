#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"
#include "LibWrapper.h"
#include "Exceptions.h"

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
		virtual void Deserialize(std::string&) = 0;
		virtual ~Message() {}
	};


	// 直接使用对象的内存进行序列化/反序列化，
	// 因此，只能是一些简单的类型，不能包含指针、容器、静态成员等
	// 否则，行为是不可预料的
	template<typename TValue>
	class SimpleObjectMessage : public Message
	{
	public:
		virtual TypeIDHash GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
		virtual ~SimpleObjectMessage() {}
	public:
		SimpleObjectMessage();
		SimpleObjectMessage(TValue value);
		TValue value;
	};

	// 模板类必须把定义和声明放在一起，不然gcc编译不过
	
	template<typename TValue>
	inline TypeIDHash SimpleObjectMessage<TValue>::GetTypeID()
	{
		SHA256Value sha;
		// 暂时不能区分不同的模板实例
		const std::string name("SimpleObjectMessage");
		SetSHA256InPlace(name.c_str(), name.size(), &sha);
		return sha;
	}

	template<typename TValue>
	inline std::string SimpleObjectMessage<TValue>::Serialize()
	{
		constexpr int len = sizeof(TValue);
		char buf[len];
		char* pValue = reinterpret_cast<char*>(&this->value);
		std::copy(pValue, pValue + len, buf);
		return std::string(buf, len);
	}

	template<typename TValue>
	inline void SimpleObjectMessage<TValue>::Deserialize(std::string& str)
	{
		constexpr int len = sizeof(TValue);
		char* pValue = reinterpret_cast<char*>(&this->value);
		const char* buf = str.c_str();
		std::copy(buf, buf + len, pValue);
	}

	template<typename TValue>
	inline SimpleObjectMessage<TValue>::SimpleObjectMessage()
	{
		this->value = TValue();
	}

	template<typename TValue>
	inline SimpleObjectMessage<TValue>::SimpleObjectMessage(TValue value)
	{
		this->value = value;
	}


	// 提供对简单类型的对象数组的支持
	// 数组不论多长，都属于同一种类型的消息，
	// 但长度为1的数组与简单对象消息属于不同类型的消息
	// 发布和订阅时，请注意消息类型的匹配
	template<typename TValue>
	class SimpleObjectArrayMessage : public Message
	{
	public:
		virtual TypeIDHash GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
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
	};

	template<typename TValue>
	inline TypeIDHash SimpleObjectArrayMessage<TValue>::GetTypeID()
	{
		SHA256Value sha;
		// 暂时不能区分不同的模板实例
		const std::string name("SimpleObjecArraytMessage");
		SetSHA256InPlace(name.c_str(), name.size(), &sha);
		return sha;
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
		virtual TypeIDHash GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
		virtual ~StringMessage() {}
	public:
		StringMessage();
		StringMessage(std::string& str);
		StringMessage(const char* str);
		std::string GetValue();
	private:
		std::string Value;
	};


	// 使用Json进行编解码的消息
	// 内部拟用JsonCpp实现
	// 不能解决嵌套，除非调用也引入JsonCpp库，并且指定模板参数为Json::Value
	// 感觉没啥用，懒得实现了
	// 也不能解决TypeID的问题，存在隐患
	class JsonMessage : public Message
	{
	public:
		virtual TypeIDHash GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
		virtual ~JsonMessage();
	public:
		template<typename T> T GetValueByName(std::string& name);
		template<typename T> T GetValueByName(const char* name);
		template<typename T> T SetValueByName(std::string& name, T value);
		template<typename T> T SetValueByName(const char*, T value);
		template<typename T> T& operator[] (std::string& name);
		template<typename T> T& operator[] (const char* name);
	private:
		class JsonMessageImplement;
		JsonMessageImplement* implData;
	};


	// 模仿了一下C#的多播委托，用于回调多个函数
	// 存在不安全性，需要在应用层保证注册的函数类型与实际调用的类型匹配
	class MulticastMessageDelegate
	{
	private:
		class IMessageStringCallable
		{
		public:
			virtual void Invoke(std::string& serializedMsg) = 0;
			virtual bool Equals(IMessageStringCallable* other) = 0;
			virtual ~IMessageStringCallable() {}
		};

		// 普通函数指针和静态成员函数
		template<typename TMessage>
		using NormalCallback = void (*)(TMessage);
		template<typename TMessage>
		class NormalMessageDelegate : public IMessageStringCallable
		{
		public:
			NormalMessageDelegate(NormalCallback<TMessage> callback)
			{
				this->CallbackFunction = callback;
			}
			~NormalMessageDelegate() {} // 类的所有资源都不属于自己，不要delete，后面两个类同理
		public:
			virtual void Invoke(std::string& serializedMsg) override
			{
				TMessage msg;
				msg.Deserialize(serializedMsg);
				(*this->CallbackFunction)(msg);
			}
			virtual bool Equals(IMessageStringCallable* other) override
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
			NormalCallback<TMessage> CallbackFunction;
		};

		// 对象成员函数
		template<typename TMessage, typename TObject>
		using ObjectMemberCallback = void (TObject::*)(TMessage);
		template<typename TMessage, typename TObject>
		class ObjectMemberMessageDelegate : public IMessageStringCallable
		{
		public:
			ObjectMemberMessageDelegate(ObjectMemberCallback<TMessage, TObject> callback, TObject* obj)
			{
				this->Obj = obj;
				this->MemberCallbackFunction = callback;
			}
			~ObjectMemberMessageDelegate() {}
		public:
			virtual void Invoke(std::string& serializedMsg) override
			{
				TMessage msg;
				msg.Deserialize(serializedMsg);
				(this->Obj->*MemberCallbackFunction)(msg);
			}
			virtual bool Equals(IMessageStringCallable* other) override
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
			ObjectMemberCallback<TMessage, TObject> MemberCallbackFunction;
		};

		// 函数对象
		template<typename TMessage, typename TObject>
		class FunctionObjectMessageDelegate : public IMessageStringCallable
		{
		public:
			FunctionObjectMessageDelegate(TObject& obj)
			{
				this->pObj = &obj;
			}
			~FunctionObjectMessageDelegate() {}
		public:
			virtual void Invoke(std::string& serializedMsg) override
			{
				TMessage msg;
				msg.Deserialize(serializedMsg);
				// this->pObj->operator()(typedMessage);
				(*this->pObj)(msg);
			}
			virtual bool Equals(IMessageStringCallable* other) override
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
			this->Delegates = new PointerIMessageCallable[maxCall]{ nullptr };
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

		// 注册一个普通函数或者静态成员函数，TMessage是必选模板
		template<typename TMessage>
		void Register(NormalCallback<TMessage> funtion)
		{
			this->ThrowIfMax();
			IMessageStringCallable* callback = new NormalMessageDelegate<TMessage>(funtion);
			this->Delegates[this->RegisteredCount] = callback;
			this->RegisteredCount++;
		}

		// 注册一个对象的成员函数，TMessage是必选模板，Tobject可以自动推导
		template<typename TMessage, typename TObject>
		void Register(ObjectMemberCallback<TMessage, TObject> memeberFunction, TObject& obj)
		{
			// 这个函数有两个参数，没法重载+=号，所以其他的也不重载了
			this->ThrowIfMax();
			IMessageStringCallable* callback = new ObjectMemberMessageDelegate<TMessage, TObject>(memeberFunction, &obj);
			this->Delegates[this->RegisteredCount] = callback;
			this->RegisteredCount++;
		}

		// 注册一个函数对象，TMessage是必选模板，Tobject可以自动推导
		template<typename TMessage, typename TObject>
		void Register(TObject& obj)
		{
			this->ThrowIfMax();
			IMessageStringCallable* callback = new FunctionObjectMessageDelegate<TMessage, TObject>(obj);
			this->Delegates[this->RegisteredCount] = callback;
			this->RegisteredCount++;
		}

		// 取消注册一个普通函数或者静态成员函数
		template<typename TMessage>
		void Unregister(NormalCallback<TMessage> funtion)
		{
			IMessageStringCallable* callback = new NormalMessageDelegate<TMessage>(funtion);
			this->FindAndRemove(callback);
		}

		// 取消注册一个对象的成员函数
		template<typename TMessage, typename TObject>
		void Unregister(ObjectMemberCallback<TMessage, TObject> memeberFunction, TObject& obj)
		{
			IMessageStringCallable* callback = new ObjectMemberMessageDelegate<TMessage, TObject>(memeberFunction, &obj);
			this->FindAndRemove(callback);
		}

		// 取消注册一个函数对象
		template<typename TMessage, typename TObject>
		void Unregister(TObject& obj)
		{
			IMessageStringCallable* callback = new FunctionObjectMessageDelegate<TMessage, TObject>(obj);
			this->FindAndRemove(callback);
		}

		void InvokeAll(const char* buf, int len)
		{
			std::string msgStr(buf, len);
			for (int i = 0; i < this->RegisteredCount; i++)
			{
				this->Delegates[i]->Invoke(msgStr);
			}
		}
	private:
		using PointerIMessageCallable = IMessageStringCallable*;
		PointerIMessageCallable* Delegates;
		int RegisteredCount;
		int MaxCount;

	private:
		inline void ThrowIfMax()
		{
			if (this->RegisteredCount == this->MaxCount)
			{
				throw CommunicateException("注册的回调函数达到上限");
			}
		}
		inline void FindAndRemove(IMessageStringCallable* pMessageCallable)
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
	};

	using MessageCallback = MulticastMessageDelegate;
}