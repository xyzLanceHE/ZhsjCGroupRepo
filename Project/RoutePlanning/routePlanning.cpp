#include <stdio.h>
#include <math.h>
#include "TinyROS/TinyROS.h"
#error 尽量使用constexpr而不是宏定义
#define xMax 20
#define yMax 20

////#error 致吴勇同志：你TM能不能好好看下文件，让你写在命名空间里你不写
////
////#error 请再次阅读命名规范
//
//struct location
//{
//	int x;
//	int y;
//};
////int a[20][20];
//int targetx, targety;
//int carx, cary;
//int barrierx[20], barriery[20];
//int xyMax(int barrierx[20])
//{
//	int maxX = 0;
//	for(int i = 0; i < 20; i++)
//	{
//		if (barrierx[i] > maxX)
//		{
//			maxX = barrierx[i];
//		}
//	}
//	return maxX;
//}
//
//int xyMin(int barrierx[20])
//{
//	int minX = 100;
//	for (int i = 0; i < 20; i++)
//	{
//		if (barrierx[i] < minX && barrierx[i])
//		{
//			minX = barrierx[i];
//		}
//	}
//	return minX;
//}
//
////void setZero(int a[20][20])
////{
////	for (int i = 0; i < 20; i++)
////	{
////		for (int j = 0; j < 20; i++)
////		{
////			a[i][j] = 0;
////		}
////	}
////}
//
//void setPoint(int a[20][20], int carx, int cary, int targetx, int targety)
//{
//	a[carx][cary] = 1;
//	a[targetx][targety] = 2;
//}
//
//void getBarrierDir(double* barrierAngle, int carx, int cary, int maxBarrierX, int maxBarrierY, int minBarrierX, int minBarrierY)
//{
//	double maxAngle, minAngle, temp;
//#error atan不包含象限信息，无法判断角度属于哪个象限，请考虑使用atan2
//#error 请考虑使用标准库rtod函数将弧度转化为角度
//	maxAngle = atan((maxBarrierY - cary) / (maxBarrierY - carx)) / PI * 180.0;
//	minAngle = atan((minBarrierY - cary) / (minBarrierY - carx)) / PI * 180.0;
//	if (maxAngle < minAngle)
//	{
//		temp = maxAngle;
//		maxAngle = minAngle;
//		minAngle = temp;
//	}
//	barrierAngle[1] = maxAngle;
//	barrierAngle[2] = minAngle;
//}
//
//double getDefaultDir(int carx, int cary, int targetx, int targety)
//{
//	double angle;
//	angle = atan((targety - cary) / (targetx - carx))/PI * 180.0;
//	return angle;
//}
//
//#error do you mean straight?
//int canGoStraite(double defaultAngle, double maxAngle, double minAngle)
//{
//	if (defaultAngle > maxAngle || defaultAngle < minAngle)
//	{
//		return 1;
//	}
//	return 0;
//}
//
//
//
//int main()
//{
//	/*int a[10];
//	int b;
//	b = 10;
//	char numberstring[((sizeof(a) * CHAR_BIT) + 2) / 3 + 2];
//	sprintf(numberstring, "%d", a);
//	func(numberstring);*/
//
//	//stringstream strs;
//	//strs << num;
//	//string temp = strs.str();
//	//char* charNum = (char*)temp.c_str();
//	//func(charNum);
//
//	system("pause");
//	return 0;
//}
#error 从模块功能上看，你应该把这些东西放在RoboTax命名空间下
#error 目前来说，基本上不会有什么应该放到TinyROS下的，除了自定义的Message类型（而且也不是非得放）
namespace TinyROS
{
#error TinyROS下恰好就有一个Node类，这样肯定编译不过
#error 这就是为什么要分命名空间
	struct Node
	{
		int X;//数组行号
		int Y;//数组列号
#error 建议使用bool类型，并且起一个意义更加明确的名字，我猜你想表达IsBarrier？
		int B;//标识能否扩展
#error int类型合适吗？
		int G;//已经花费的代价
		int F;//预计到目标还要的代价
#error 需要重复写struct关键字是C的特性，C++可以去掉，直接Node就好了，后面的函数参数、变量声明同理
		struct Node* PNext;//子节点
		struct Node* PFather;//父节点
	};


	bool  Compare(struct Node* p1, struct Node* p2)//判断两个节点状态是否相同
	{
		if (p1->X == p2->X && p1->Y == p2->Y)
#error 循环、判断即使只有一句，也要写大括号
			return true;
		else
			return false;
	}

#error 直接写成构造函数啊。C++的struct也是类，只是默认访问权限不同
	struct Node *NewNode(int x, int y, int b)
	{
#error 使用nullptr关键词，而不是NULL，建议直接查找替换
		struct Node* pNode = NULL;
#error 写成构造函数之后，就不用分配内存了，在外面直接声明变量，或者使用new运算符分配变量并得到指针
#error 使用C++的类型转换符而不是C风格的，如果写成构造函数甚至不需要转化
		pNode = (struct Node*)malloc(sizeof(struct Node));
		if (pNode == NULL) return NULL;
		pNode->X = x;
		pNode->Y = y;
		pNode->B = b;
		pNode->G = 0;
		pNode->F = 0;
		pNode->PFather = NULL;
		pNode->PNext = NULL;
		return pNode;
	}

#error 建议封装一个List类，该方法作为析构函数
#error 封装List类有助于简化函数参数
	void FreeList(struct Node* pList)
	{
		struct Node* pNode = NULL;
		while (pList)
		{
			pNode = pList;
			pList = pList->PNext;
#error 直接使用delete运算符即可，并且可以在析构函数中进行需要的处理
			free(pNode);
		}
	}

#error 建议封装成类，此函数作为类的方法时，可以省去参数pList和返回值
	struct Node* DelNode(struct Node* pNode, struct Node* pList)
	{
		if (pList == NULL) return pList;
		if (Compare(pNode, pList)) return pList->PNext;
		pList->PNext = DelNode(pNode, pList->PNext);
		return pList;
	}
#error 建议封装成类，此函数作为类的方法时，可以省去参数pList和返回值
#error 为什么不管pFather呢
	struct Node* AddToList(struct Node* pNode, struct Node* pList)
	{
		pNode->PNext = pList;
		return pNode;
	}

	struct Node* AddToListForOpen(struct Node* pNode, struct Node* pOpen)
	{
		if (pOpen == NULL)  //OPEN表为空    
		{
			pNode->PNext = NULL;
			return pNode;
		}
		if (pNode->F < pOpen->F)   //给定节点的f值小于OPEN表第一个节点的f值    
		{
			pNode->PNext = pOpen;    //插入到OPEN的最前面    
			return pNode;
		}
		pOpen->PNext = AddToListForOpen(pNode, pOpen->PNext);    //递归    
		return pOpen;
	}

#error 建议封装成类，此函数作为类的方法时，可以省去参数pList；
#error 尾递归，建议改成循环
#error 从功能上看，建议改名为Find
#error 多说两句：话说张伟老师强调自己写底层，不过我还是觉得STL既然已经是C++标准，还是可以用的，比起这点数据结构，不如多了解点计算机原理来得实在
#error 自己写链表的时候，也可以参考STL容器的一些常见风格，尤其是做好内存管理并提供迭代器，例如本函数，找到节点则返回该节点的迭代器，否则返回的迭代器==list.end()
	struct Node* IsNodeInList(struct Node* pNode, struct Node* pList)
	{
		if (pList == NULL) return NULL;
		if (Compare(pNode, pList)) return pList;
		return IsNodeInList(pNode, pList->PNext);
	}

#error 建议封装成类，可以避免递归的写法
	void PrintPath(struct Node* pGoal)
	{
		if (pGoal == NULL) return;
		PrintPath(pGoal->PFather);
		printf("(%d %d)\n", pGoal->X, pGoal->Y);
	}

#error 好像没见你赋值过pFather啊，怎么就用上了（如果赋值了当我没说）
	int IsGrandFather(struct Node* pNode, struct Node* pFather)
	{
		if (pFather == NULL) return 0;
		if (pFather->PFather == NULL) return 0;
		return Compare(pNode, pFather->PFather);
	}

#error 建议改名，因为此函数具有泛用性
#error 建议改成inline
	bool IsGoal(struct Node* pNode, int goalX, int goalY)
	{
		if (pNode->X == goalX && pNode->Y == goalY ) 
			return true;
		else
			return false;
	}

	bool IsLeagal(struct Node* pNode)
	{
		if (pNode->X<0 ||
			pNode->Y<0 ||
			pNode->B==1 || pNode->X >= xMax|| pNode->Y >= yMax)
			return false;
		 return true;
	}

#error 建议改成inline
	int CalculateH(struct Node* pNode, int goalX, int goalY)
	{
		return(pow((goalY - pNode->Y), 2) + pow((goalX - pNode->X), 2));
	}

#error 建议将数组参数改为指针，数组长度作为变量传入，以提高扩展性
	struct Node* AStar(struct Node* start, int goalX, int goalY, int map[xMax][yMax])
	{
#error 如果封装List类，这里就使用List类型
#error 关于List类的一些建议：List类内维护一些实用指针，例如链表两头的指针，以提高效率
#error 使用List进行更加清晰的内存管理：内部的Node由List独占，从而在List的销毁时统一析构
#error 其他地方使用Node的时候，直接声明值就好了，不需要声明指针
#error 只有当Node加入或者从链表里返回时，以传值的形式进行插入和返回，然后在链表内部使用指针
		struct Node* pOpen = NULL;//open表
		struct Node* pClose = NULL;//close表
		struct Node* n = NULL, *m = NULL, *pNode = NULL;
		int i, j;
		pOpen = start;
		pClose = NULL;
#error 建议使用具有显示bool类型的表达式
		while (pOpen)
		{
			n = pOpen;
			if (IsGoal(n, goalX, goalY)) return n;
			pOpen = pOpen->PNext;
			pClose = AddToList(n, pClose);
			//产生新一轮的popen
			for (i = -1; i <= 1; i++)
			{
				for (j = -1; j <= 1; j++)
				{
#error 如前所述，将NewNode写成构造函数，并使用new运算符，相应地使用delete运算符
					m = NewNode(n->X + i, n->Y + j, map[n->X + i][n->Y + j]);//拓展子节点
					if (IsGrandFather(m, n) || !IsLeagal(m))
					{
						free(m);
						continue;
					}
					m->PFather = n;
					m->G = n->G + 1;
					m->F = m->G + CalculateH(m, goalX, goalY);

					if (IsNodeInList(m, pOpen))
					{
						pNode = IsNodeInList(m, pOpen);
						if (m->F < pNode->F)
						{
							pOpen = AddToListForOpen(m, DelNode(pNode, pOpen));
							free(pNode);
						}
						else
						{
							free(m);
						}
					}
					else if (IsNodeInList(m, pClose))
					{
						pNode = IsNodeInList(m, pClose);
						if (m->F < pNode->F)
						{
							pClose = DelNode(pNode, pClose);
							pOpen = AddToListForOpen(m, pOpen);
							free(pNode);
						}
						else
						{
							free(m);
						}
					}
					else
					{
						pOpen = AddToListForOpen(m, pOpen);
					}
				}
			}
		}
#error 为什么只会return空指针呢
		return NULL;
	}
	void RunAStar(int startX, int startY, int goalX, int goalY, int map[xMax][yMax])
	{
		struct Node* start;
		struct Node* goal;
		start = NewNode(startX, startY, 0);
		goal = AStar(start, goalX, goalY, map);
		if (goal) PrintPath(goal);
		else printf("搜索失败");
	}
}