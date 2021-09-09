#include "MapTools.h"
#include <algorithm>
#include <random>
#include <time.h>

namespace RoboTax
{
	MapMessage GenerateZeroMap(int width, int height)
	{
		return MapMessage(width, height);
	}

	MapMessage GenerateRandomMap(int width, int height, float obstruct)
	{
		int len = width * height;
		MapMessage map(width, height);

		std::default_random_engine rng(time(nullptr));

		// 。。。生成随机障碍
		throw std::exception("not implemented");		
	}

}