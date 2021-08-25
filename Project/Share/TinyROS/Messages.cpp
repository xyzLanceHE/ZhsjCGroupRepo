#include "Messages.h"
#include <string.h>

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
}