#include "iostream"
#include <stdio.h>
#include <math.h>

#define PI 3.1415926
using namespace std;

struct location
{
	int x;
	int y;
};
//int a[20][20];
int targetx, targety;
int carx, cary;
int barrierx[20], barriery[20];
int xyMax(int barrierx[20])
{
	int maxX = 0;
	for(int i = 0; i < 20; i++)
	{
		if (barrierx[i] > maxX)
		{
			maxX = barrierx[i];
		}
	}
	return maxX;
}

int xyMin(int barrierx[20])
{
	int minX = 100;
	for (int i = 0; i < 20; i++)
	{
		if (barrierx[i] < minX && barrierx[i])
		{
			minX = barrierx[i];
		}
	}
	return minX;
}

//void setZero(int a[20][20])
//{
//	for (int i = 0; i < 20; i++)
//	{
//		for (int j = 0; j < 20; i++)
//		{
//			a[i][j] = 0;
//		}
//	}
//}

void setPoint(int a[20][20], int carx, int cary, int targetx, int targety)
{
	a[carx][cary] = 1;
	a[targetx][targety] = 2;
}

void getBarrierDir(double* barrierAngle, int carx, int cary, int maxBarrierX, int maxBarrierY, int minBarrierX, int minBarrierY)
{
	double maxAngle, minAngle, temp;
	maxAngle = atan((maxBarrierY - cary) / (maxBarrierY - carx)) / PI * 180.0;
	minAngle = atan((minBarrierY - cary) / (minBarrierY - carx)) / PI * 180.0;
	if (maxAngle < minAngle)
	{
		temp = maxAngle;
		maxAngle = minAngle;
		minAngle = temp;
	}
	barrierAngle[1] = maxAngle;
	barrierAngle[2] = minAngle;
}

double getDefaultDir(int carx, int cary, int targetx, int targety)
{
	double angle;
	angle = atan((targety - cary) / (targetx - carx))/PI * 180.0;
	return angle;
}


int canGoStraite(double defaultAngle, double maxAngle, double minAngle)
{
	if (defaultAngle > maxAngle || defaultAngle < minAngle)
	{
		return 1;
	}
	return 0;
}



int main()
{
	/*int a[10];
	int b;
	b = 10;
	char numberstring[((sizeof(a) * CHAR_BIT) + 2) / 3 + 2];
	sprintf(numberstring, "%d", a);
	func(numberstring);*/

	//stringstream strs;
	//strs << num;
	//string temp = strs.str();
	//char* charNum = (char*)temp.c_str();
	//func(charNum);

	system("pause");
	return 0;
}