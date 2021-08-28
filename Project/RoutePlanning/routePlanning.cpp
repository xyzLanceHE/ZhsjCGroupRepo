#include <stdio.h>
#include <math.h>
#include "TinyROS/TinyROS.h"
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
namespace TinyROS
{
	struct Node
	{
		int X;//数组行号
		int Y;//数组列号
		int B;//标识能否扩展
		int G;//已经花费的代价
		int F;//预计到目标还要的代价
		struct Node* PNext;//子节点
		struct Node* PFather;//父节点
	};

	bool  Compare(struct Node* p1, struct Node* p2)//判断两个节点状态是否相同
	{
		if (p1->X == p2->X && p1->Y == p2->Y)
			return true;
		else
			return false;
	}

	struct Node *NewNode(int x, int y, int b)
	{
		struct Node* pNode = NULL;
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

	void FreeList(struct Node* pList)
	{
		struct Node* pNode = NULL;
		while (pList)
		{
			pNode = pList;
			pList = pList->PNext;
			free(pNode);
		}
	}

	struct Node* DelNode(struct Node* pNode, struct Node* pList)
	{
		if (pList == NULL) return pList;
		if (Compare(pNode, pList)) return pList->PNext;
		pList->PNext = DelNode(pNode, pList->PNext);
		return pList;
	}

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

	struct Node* IsNodeInList(struct Node* pNode, struct Node* pList)
	{
		if (pList == NULL) return NULL;
		if (Compare(pNode, pList)) return pList;
		return IsNodeInList(pNode, pList->PNext);
	}

	void PrintPath(struct Node* pGoal)
	{
		if (pGoal == NULL) return;
		PrintPath(pGoal->PFather);
		printf("(%d %d)\n", pGoal->X, pGoal->Y);
	}

	int IsGrandFather(struct Node* pNode, struct Node* pFather)
	{
		if (pFather == NULL) return 0;
		if (pFather->PFather == NULL) return 0;
		return Compare(pNode, pFather->PFather);
	}

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

	int CalculateH(struct Node* pNode, int goalX, int goalY)
	{
		return(pow((goalY - pNode->Y), 2) + pow((goalX - pNode->X), 2));
	}

	struct Node* AStar(struct Node* start, int goalX, int goalY, int map[xMax][yMax])
	{
		struct Node* pOpen = NULL;//open表
		struct Node* pClose = NULL;//close表
		struct Node* n = NULL, *m = NULL, *pNode = NULL;
		int i, j;
		pOpen = start;
		pClose = NULL;
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