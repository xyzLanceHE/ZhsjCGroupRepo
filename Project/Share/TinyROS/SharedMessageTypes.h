#pragma once
#include "TinyROSDef.h"
#include "Messages.h"

/// <summary>
/// 于此处共享一些消息类型，以免在多个项目中反复定义
/// </summary>

namespace RoboTax
{

#pragma region 编辑规范
	// 1. 使用一对#pragma region/endregion 把一段括起来，
	//		这条编译器指令没有实际用途，但是使用Visual Studio浏览时可以将这段代码折叠/展开
	// 2. 使用注释注明这段内容的编写者，并说明消息的内容
	// 3. 结构/类型的定义放在头文件里，如果有成员函数，放在源文件里实现（模板类除外，模板类直接在头文件里实现）
#pragma endregion

#pragma region 小车速度消息
	// editor: 贺兰山
	// 小车速度结构体
	struct CarVelocity
	{
		float Linear;	// 线速度，单位m/s
		float Radius;	// 转弯半径，单位m
	};

	// 小车速度消息别名
	using CarVelocityMessage = TinyROS::SimpleObjectMessage<CarVelocity>;
#pragma endregion

#pragma region 地图消息
	// editor: 吴勇, 贺兰山

	// Obsolete
	// 地图结构体
	//struct Map
	//{
	//	int a[400];
	//};

	class MapMessage : public TinyROS::Message
	{
	public:
		// 继承自Message
		virtual TinyROS::TypeIDHash GetTypeID() override;
		virtual std::string Serialize() override;
		virtual void Deserialize(std::string& str) override;
		virtual TinyROS::Message* NewDeserialize(std::string& str) override;
		virtual ~MapMessage();
	public:
		// 创建一个空地图，长和宽等于0，数据指针为nullptr
		MapMessage();
		// 创建一个指定长宽的地图，所有元素都为0
		MapMessage(int width, int height);
		// 创建一个指定长宽的地图，并将数据复制过来。请保证数据长度匹配
		MapMessage(int width, int height, unsigned char* pData);
		// 拷贝构造
		MapMessage(const MapMessage& other);
		// 移动构造
		MapMessage(MapMessage&& other);
		// 获取地图上指定行、列的元素值，索引从0开始
	    unsigned char At(int row, int col);
		// 获取地图上指定行、列的元素的引用，索引从0开始
		unsigned char& RefAt(int row, int col);
		// 获取地图宽度（列数）
		int GetWidth();
		// 获取地图长度（行数）
		int GetHeight();
		// 将地图的数据复制到指定位置
		void CopyTo(unsigned char* pBuffer);
	private:
		int Width;
		int Height;
		unsigned char* pData;
		static TinyROS::TypeIDHash TypeHash;
	};

	// Obsolete
	// 小车速度消息别名
	//using MapMessage = TinyROS::SimpleObjectMessage<Map>;
#pragma endregion

#pragma region 坐标消息
		// editor: 贺兰山
	struct Coordinate2D //表示一个坐标
	{
		float x;
		float y;
	};

	// 坐标消息别名
	using Coordinate2DMessage = TinyROS::SimpleObjectMessage<Coordinate2D>;
#pragma endregion
}