#include "MapTools.h"
#include <algorithm>
#include <random>
#include <time.h>
#include <iostream>

namespace RoboTax
{
	MapMessage GenerateZeroMap(int width, int height)
	{
		return MapMessage(width, height);
	}

	float CalculateObstructRate(MapMessage map)
	{
		int count = 0;
		for (int i = 0; i < map.GetHeight(); i++)
		{
			for (int j = 0; j < map.GetWidth(); j++)
			{
				if (int(map.At(i, j)) == 1)
				{
					count++;
				}
			}
		}
		return count*1.0f / (map.GetHeight() * map.GetWidth());
	}

	void InsertObstruct(MapMessage& map, int ObstructLen)
	{
		int obstructStartI, obstructStartJ, obstructEndI, obstructEndJ;
		std::default_random_engine rng(time(nullptr));
		std::uniform_int_distribution<unsigned> i(0, map.GetHeight());
		std::uniform_int_distribution<unsigned> j(0, map.GetWidth());
		obstructStartI = i(rng);
		obstructStartJ = j(rng);//随机生成插入障碍物的开始位置
		if (obstructStartI + ObstructLen < map.GetHeight())
		{
			obstructEndI = obstructStartI + ObstructLen;
		}
		else
		{
			obstructEndI = map.GetHeight() - 1;
		}//判断是否越界，越界就到边界位置
		if (obstructStartJ + ObstructLen < map.GetWidth())
		{
			obstructEndJ = obstructStartJ + ObstructLen;
		}
		else
		{
			obstructEndJ = map.GetWidth() - 1;
		}
		for (int m = obstructStartI; m <= obstructEndI; m++)
		{
			for (int n = obstructStartJ; n <= obstructEndJ; n++)
			{
				if (int(map.At(m, n))==0)
				{
					map.RefAt(m, n) = 1;
				}
			}
		}
		//return map;
	}
	MapMessage GenerateRandomMap( int width, int height, float maxObstruct)
	{
		int len = width * height;
		int maxObstructLen;

		if (height >= width)
		{
			maxObstructLen = int(width / 2);
		}
		else
		{
			maxObstructLen = int(width / 2);
		}

		MapMessage map(width, height);

		std::default_random_engine rng(time(nullptr));
		std::uniform_int_distribution<unsigned> u(1, maxObstructLen);

		while (CalculateObstructRate(map) < maxObstruct)
		{
			InsertObstruct(map, u(rng));
		}
		return map;
	}
	void EncodeToPng(MapMessage& map, std::fstream& pngFileStream)
	{
		;
	}
	MapMessage DecodeFromPng(std::fstream& pngFileStream)
	{

	}
}
int main()
{
	RoboTax::MapMessage map = RoboTax:: GenerateRandomMap( 10, 10, 0.1);

	for (int i = 0; i < map.GetHeight(); i++)
	{
		for (int j = 0; j < map.GetWidth(); j++)
		{
			std::cout << (int)map.At(i, j)<<" ";
		}
		std::cout << std::endl;
	}

}