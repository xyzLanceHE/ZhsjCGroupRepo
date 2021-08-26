#include "Messages.h"
#include <string.h>
#include <string>
#include <sstream>

namespace TinyROS
{
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
		char buf[64];
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
		return std::string(buf, 64);
	}

	TypeID Message::GetTypeID()
	{
		TypeID id(typeid(Message));
		return id;
	}
}