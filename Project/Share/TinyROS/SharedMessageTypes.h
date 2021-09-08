#pragma once
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


}