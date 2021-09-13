#include "MapTools.h"
#include <algorithm>
#include <random>
#include <time.h>
#include "iostream"
#include "lodepng.h"
#include <stdlib.h>

namespace RoboTax
{
	MapMessage GenerateZeroMap(int width, int height)
	{
		return MapMessage(width, height);
	}

	//打印地图
	void PrintMap(MapMessage& map)
	{
		for (int i = 0; i < map.GetHeight(); i++)
		{
			for (int j = 0; j < map.GetWidth(); j++)
			{
				std::cout << (int)map.At(i, j) << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	//计算障碍物比率
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

	//随机插入随机大小的障碍物
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

	//根据设定障碍物比率随机生成地图
	MapMessage GenerateRandomMap( int width, int height, float maxObstruct)
	{
		int len = width * height;
		int maxObstructLen;

		if (height >= width)
		{
			maxObstructLen = int(width / 3);
		}
		else
		{
			maxObstructLen = int(width / 3);
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

	//将地图编码为png格式
	void EncodeToPng(MapMessage& map, const char* filename)
	{
		int width = map.GetWidth(), height = map.GetHeight();
		unsigned char* image = new unsigned char[(unsigned long long)(width) * height * 4];
		int x, y;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++) {
				image[4 * width * y + 4 * x + 0] = 255 - 255 * map.At(y, x);
				image[4 * width * y + 4 * x + 1] = 255 - 255 * map.At(y, x);
				image[4 * width * y + 4 * x + 2] = 255 - 255 * map.At(y, x);
				image[4 * width * y + 4 * x + 3] = 255;
			}
		unsigned error = lodepng_encode32_file(filename, image, width, height);
		if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
		//RoboTax::encodePng(filename, image, width, height);
		delete[]image;
	}

	//从png格式文件中载入地图
	void DecodeFromPng(const char* filename, MapMessage& map)
	{
		unsigned width, height;
		std::vector<unsigned char> image;
		unsigned error = lodepng::decode(image, width, height, filename);
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < 4 * width; j += 4)
			{
				/*std::cout << int(image[(int long long)i * 4 * width + j] / 255) << " ";*/
				map.RefAt(i, int(j / 4)) = int(1-image[(int long long)i * 4 * width + j] / 255);
			}
		}
	}
}
//测试
//int main()
//{
//	//随机生成地图
//	RoboTax::MapMessage map = RoboTax:: GenerateRandomMap( 100, 100, 0.2);
//	PrintMap(map);
//
//	const char* filename = "map.png";
//	//将地图编码为png格式
//	EncodeToPng(map, filename);
//
//	RoboTax::MapMessage map1(map.GetHeight(),map.GetWidth());
//	//从png图片中载入地图
//	DecodeFromPng("map.png", map1);
//	std::cout << "map decoded from png:" << std::endl;
//	PrintMap(map1);
//
//	return 0;
//}