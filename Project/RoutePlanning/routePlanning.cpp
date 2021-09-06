#include <stdio.h>
#include <math.h>
#include <TinyROS/TinyROS.h>
#include <iostream>
//#error 尽量使用constexpr而不是宏定义
//constexpr int xMax = 20;
//constexpr int yMax = 20;

namespace RoboTax
{
	struct Node
	{
		int X;//数组行号
		int Y;//数组列号
		bool IsBarrier;//标识能否扩展
		double G;//已经花费的代价
		double F;//预计到目标还要的代价
		Node* PNext;//子节点
		Node* PFather;//父节点
	};


	bool  Compare(struct Node* p1, struct Node* p2)//判断两个节点状态是否相同
	{
		if (p1->X == p2->X && p1->Y == p2->Y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

//#error 直接写成构造函数啊。C++的struct也是类，只是默认访问权限不同
	Node *NewNode(int x, int y, int b)
	{
		Node* pNode = nullptr;
//#error 写成构造函数之后，就不用分配内存了，在外面直接声明变量，或者使用new运算符分配变量并得到指针
//#error 使用C++的类型转换符而不是C风格的，如果写成构造函数甚至不需要转化
		pNode = new Node;
		if (pNode == nullptr) return nullptr;
		pNode->X = x;
		pNode->Y = y;
		pNode->IsBarrier = b;
		pNode->G = 0;
		pNode->F = 0;
		pNode->PFather = nullptr;
		pNode->PNext = nullptr;
		return pNode;
	}

//#error 建议封装一个List类，该方法作为析构函数
//#error 封装List类有助于简化函数参数
	void FreeList(Node* pList)
	{
		Node* pNode = nullptr;
		while (pList)
		{
			pNode = pList;
			pList = pList->PNext;
//#error 直接使用delete运算符即可，并且可以在析构函数中进行需要的处理
			delete pNode;
		}
	}

//#error 建议封装成类，此函数作为类的方法时，可以省去参数pList和返回值
	Node* DelNode(Node* pNode, Node* pList)
	{
		if (pList == nullptr) return pList;
		if (Compare(pNode, pList)) return pList->PNext;
		pList->PNext = DelNode(pNode, pList->PNext);
		return pList;
	}
//#error 建议封装成类，此函数作为类的方法时，可以省去参数pList和返回值
//#error 为什么不管pFather呢？ 链表里面只要前后排好就行，只用来检测是否在表里就好
	Node* AddToList(Node* pNode, Node* pList)
	{
		pNode->PNext = pList;
		return pNode;
	}

	Node* AddToListForOpen(Node* pNode, Node* pOpen)
	{
		if (pOpen == nullptr)  //OPEN表为空    
		{
			pNode->PNext = nullptr;
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

//#error 建议封装成类，此函数作为类的方法时，可以省去参数pList；
//#error 尾递归，建议改成循环
//#error 从功能上看，建议改名为Find
//#error 多说两句：话说张伟老师强调自己写底层，不过我还是觉得STL既然已经是C++标准，还是可以用的，比起这点数据结构，不如多了解点计算机原理来得实在
//#error 自己写链表的时候，也可以参考STL容器的一些常见风格，尤其是做好内存管理并提供迭代器，例如本函数，找到节点则返回该节点的迭代器，否则返回的迭代器==list.end()
	Node* IsNodeInList(Node* pNode, Node* pList)
	{
		//if (pList == nullptr) return nullptr;
		//if (Compare(pNode, pList)) return pList;
		//return IsNodeInList(pNode, pList->PNext);
		while (pList != nullptr)
		{
			if (Compare(pNode, pList))
			{
				break;
				return pList;
			}
			pList = pList->PNext;
		}
		return nullptr;
	}

//#error 建议封装成类，可以避免递归的写法
	//void PrintPath(Node* pGoal)
	//{
	//	if (pGoal == nullptr) return;
	//	PrintPath(pGoal->PFather);
	//	printf("(%d %d)\n", pGoal->X, pGoal->Y);
	//}

//#error 好像没见你赋值过pFather啊，怎么就用上了（如果赋值了当我没说）每拓展一个节点都会把新节点的父指针指向他
	int IsGrandFather(Node* pNode, Node* pFather)
	{
		if (pFather == nullptr) return 0;
		if (pFather->PFather == nullptr) return 0;
		return Compare(pNode, pFather->PFather);
	}

//#error 建议改名，因为此函数具有泛用性
//#error 建议改成inline
	inline bool IsReach(Node* pNode, int goalX, int goalY)
	{
		if (pNode->X == goalX && pNode->Y == goalY)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsLeagal(Node* pNode, int xMax, int yMax)
	{
		if (pNode->X < 0 ||
			pNode->Y < 0 ||
			pNode->IsBarrier == 1 ||
			pNode->X >= xMax ||
			pNode->Y >= yMax)
		{
			return false;
		}
		 return true;
	}

	inline int CalculateH(Node* pNode, int goalX, int goalY)
	{
		return(sqrt(pow((goalY - pNode->Y), 2) + pow((goalX - pNode->X), 2)));
	}

//#error 建议将数组参数改为指针，数组长度作为变量传入，以提高扩展性
	int* AStar(Node* start, int goalX, int goalY, int* map, int xMax, int yMax)
	{
//#error 如果封装List类，这里就使用List类型
//#error 关于List类的一些建议：List类内维护一些实用指针，例如链表两头的指针，以提高效率
//#error 使用List进行更加清晰的内存管理：内部的Node由List独占，从而在List的销毁时统一析构
//#error 其他地方使用Node的时候，直接声明值就好了，不需要声明指针
//#error 只有当Node加入或者从链表里返回时，以传值的形式进行插入和返回，然后在链表内部使用指针
		Node* pOpen = nullptr;//open表
		Node* pClose = nullptr;//close表
		Node* n = nullptr, *m = nullptr, *pNode = nullptr;
		int i, j;
		pOpen = start;
		pClose = nullptr;
		while (pOpen != nullptr)
		{
			n = pOpen;
			if (IsReach(n, goalX, goalY))
			{
				while (n)
				{
					map[n->X* yMax+n->Y] = 3;
					n = n->PFather;
				}
				FreeList(pOpen);
				FreeList(pClose);
				return map;
			}
			pOpen = pOpen->PNext;
			pClose = AddToList(n, pClose);
			//产生新一轮的popen
			for (i = -1; i <= 1; i++)
			{
				for (j = -1; j <= 1; j++)
				{
					if (n->X + i < 0 || n->X + i >= xMax || n->Y + j < 0 || n->Y + j >= yMax)
					{
						continue;
					}
					m = NewNode(n->X + i, n->Y + j, (map[(n->X + i) * yMax + (n->Y + j)] != 0));//拓展子节点
					if (IsGrandFather(m, n) || !IsLeagal(m, xMax, yMax))
					{
						delete m;
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
							delete pNode;
						}
						else
						{
							delete m;
						}
					}
					else if (IsNodeInList(m, pClose))
					{
						pNode = IsNodeInList(m, pClose);
						if (m->F < pNode->F)
						{
							pClose = DelNode(pNode, pClose);
							pOpen = AddToListForOpen(m, pOpen);
							delete pNode;
						}
						else
						{
							delete m;
						}
					}
					else
					{
						pOpen = AddToListForOpen(m, pOpen);
					}
				}
			}
		}
//#error 为什么只会return空指针呢  因为不返回空指针的结果在前面就返回了，返回空指针说明没找到
		return nullptr;
	}
	void RunAStar(int startX, int startY, int goalX, int goalY, int *map, int xMax, int yMax)
	{
		Node* start;
		Node* goal;
		start = NewNode(startX, startY, false);
		map = AStar(start, goalX, goalY, map, xMax, yMax);
		if (map != nullptr)
		{
			for (int i = 0; i < xMax; i++)
			{
				for (int j = 0; j < yMax; j++)
				{
					std::cout << map[i * yMax + j] <<"  ";
				}
				std::cout << std::endl;
			}
		}
		else std::cout << "搜索失败";
	}
}
int main()
{
	int xMax = 20;
	int yMax = 20;
	int map[400] = { 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
					 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
					 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
					 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
					 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
					 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
					 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0 };
	for (int i = 0; i < xMax; i++)
	{
		for (int j = 0; j < yMax; j++)
		{
			std::cout << map[i * yMax + j] << "  ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	RoboTax::RunAStar(19, 0, 0, 19, map, xMax, yMax);
	system("pause");
	return 0;
}