#pragma once
#include "TinyROS/SharedMessageTypes.h"
#include <fstream>

namespace RoboTax
{
	MapMessage GenerateZeroMap(int width, int height);
	
	MapMessage GenerateRandomMap(int width, int height, float obstruct);
	void DecodeFromPng(const char* filename, MapMessage& map);
	void EncodeToPng(MapMessage& map, const char* filename);
}