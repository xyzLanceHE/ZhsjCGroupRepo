#pragma once
#include "TinyROSDef.h"


namespace TinyROS
{
	void SetSHA256InPlace(const char* pVal, int len, SHA256Value* pSHABuf);
	SHA256Value GetSHA(const char* pVal, int len);
}
