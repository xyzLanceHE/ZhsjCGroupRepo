#pragma once
#include "TinyROS/SharedMessageTypes.h"
#include <fstream>

namespace RoboTax
{
	MapMessage GenerateZeroMap(int width, int height);
	MapMessage GenerateRandomMap(int width, int height, float obstruct);
	MapMessage DecodeFromPng(std::fstream& pngFileStream);
	std::fstream& EncodeToPng(MapMessage& map);
}