#include "Messages.h"
#include "LibWrapper.h"

#include "OpenSSL/include/sha.h"

namespace TinyROS
{
	void SetSHA256InPlace(const char* pVal, int len, SHA256Value* pSHABuf)
	{
		SHA256(reinterpret_cast<const unsigned char*>(pVal),
			len, pSHABuf->value);
	}

	SHA256Value GetSHA(const char* pVal, int len)
	{
		SHA256Value sha;
		SetSHA256InPlace(pVal, len, &sha);
		return sha;
	}
}