#include <stdio.h>
#include <math.h>
#include <TinyROS/TinyROS.h>
#include <iostream>
#include "TinyROS/SharedMessageTypes.h"

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

	Node *NewNode(int x, int y, int b)
	{
		Node* pNode = nullptr;
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


	void FreeList(Node* pList)
	{
		Node* pNode = nullptr;
		while (pList)
		{
			pNode = pList;
			pList = pList->PNext;

			delete pNode;
		}
	}


	Node* DelNode(Node* pNode, Node* pList)
	{
		if (pList == nullptr) return pList;
		if (Compare(pNode, pList)) return pList->PNext;
		pList->PNext = DelNode(pNode, pList->PNext);
		return pList;
	}


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


	int IsGrandFather(Node* pNode, Node* pFather)
	{
		if (pFather == nullptr) return 0;
		if (pFather->PFather == nullptr) return 0;
		return Compare(pNode, pFather->PFather);
	}

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


	int* AStar(Node* start, int goalX, int goalY, int* map, int xMax, int yMax)
	{

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


RoboTax::MapMessage map;


int main()
{
	TinyROS::Node::Init("PathPlanner");
	
	TinyROS::Publisher* planPub;
	TinyROS::Subscriber* mapSub;
	TinyROS::Subscriber* goalSub;
	TinyROS::Subscriber* posSub;




	return 0;
}