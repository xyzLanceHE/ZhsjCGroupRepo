#include "Messages.h"
#include <string.h>
#include <string>
#include <sstream>

namespace TinyROS
{
	SHA256Value::SHA256Value()
	{
		memset(this->value, 0, sizeof(*this));
	}

	SHA256Value::SHA256Value(const SHA256Value& other)
	{
		memmove(this->value, other.value, sizeof(*this));
	}

	SHA256Value& SHA256Value::operator=(SHA256Value& other)
	{
		memmove(this->value, other.value, sizeof(*this));
		return *this;
	}

	bool SHA256Value::operator==(SHA256Value& other)
	{
		int result = memcmp(this, &other, sizeof(SHA256Value));
		return result == 0;
	}

	bool SHA256Value::operator!=(SHA256Value& other)
	{
		return !this->operator==(other);
	}

	std::string SHA256Value::ToHexString(bool uppercase)
	{
		char buf[80]; // Win下64会越界，稍微调大一点（或许65就够了？）
		unsigned char* p = this->value;
		const char* format;
		if (uppercase)
		{
			format = "%02X";
		}
		else
		{
			format = "%02x";
		}
		for (int i = 0; i < 32; i++)
		{
#if __TINYROS_ON_WINDOWS_PRIDEF__
			sprintf_s(buf + i * 2, strlen(format), format, *p);
#else
			sprintf(buf + i * 2, format, *p);
#endif
			p++;
		}
		std::string s(buf, 64);
		return s;
	}

	TypeIDHash Message::GetTypeID()
	{
		SHA256Value sha;
		const std::string name("Message");
		SetSHA256InPlace(name.c_str(), name.size(), &sha);
		return sha;
	}

	bool SHA256ValueComparator::operator()(const SHA256Value& left, const SHA256Value& right) const
	{
		int compareResult = memcmp(&left, &right, sizeof(SHA256Value));
		return compareResult < 0;
	}

	
	TypeIDHash StringMessage::GetTypeID()
	{
		SHA256Value sha;
		const std::string name("StringMessage");
		SetSHA256InPlace(name.c_str(), name.size(), &sha);
		return sha;
	}

	std::string StringMessage::Serialize()
	{
		return this->Value;
	}

	void StringMessage::Deserialize(std::string& str)
	{
		this->Value = str;
	}

	StringMessage::StringMessage()
	{

	}

	StringMessage::StringMessage(std::string& str)
	{
		this->Value = str;
	}


	StringMessage::StringMessage(const char* str)
	{
		this->Value = str;
	}

	std::string StringMessage::GetValue()
	{
		return this->Value;
	}

}