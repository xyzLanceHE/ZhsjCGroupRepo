#include <iostream>
#include <TinyROS/TinyROS.h>
#include "TinyROS/SharedMessageTypes.h"
#include <vector>
#include <math.H>
#include "Maptools/MapTools.h"

#define PI acos(-1)
#define Vmin 0.0  //最小速度
#define Vmax 5.0    //最大速度    单位 1格子/1s 1格子=0.1m
#define Wmin -60.0//最小角速度  单位：°
#define Wmax 60.0 //最大角速度
#define Va 0.5    //加速度
#define Wa 30.0   //角加速度
#define Vreso 0.01  //速度分辨率
#define Wreso 0.1 //角速度分辨率
#define Radius 2  //小车半径
#define Dt 0.5    //时间间隔
#define PredictTime 4.0  //模拟轨迹持续时间
#define Alpha 1.0 //距离权重
#define Belta 1.0 //速度权重
#define Gamma 1.0 //障碍物距离权重

using namespace std;
namespace RoboTax
{
	vector<pair<int, int>> GetObstacle(pair<int, int>point1, pair<int, int>point2, RoboTax::MapMessage map)
	{
		vector<pair<int, int>> obstacle;
		int startI, endI, startJ, endJ;
		if (point1.first <= point2.first)
		{
			startI = point1.first;
			endI = point2.first;
		}
		else
		{
			startI = point2.first;
			endI = point1.first;
		}
		if (point1.second <= point2.second)
		{
			startJ = point1.second;
			endJ = point2.second;
		}
		else
		{
			startJ = point2.second;
			endJ = point1.second;
		}
		for (int i = startI; i <= endI; i++)
		{
			for (int j = startJ; j < endJ; j++)
			{
				if (map.At(i, j) == 1)
				{
					obstacle.push_back(make_pair(i, j));
				}
			}
		}
		return obstacle;
	}
	double DistCost(vector<vector<float>> traj, pair<int, int> point2)//traj为vector<[横坐标，纵坐标，方向，线速度，角速度]>
	{
		vector<vector<float>> ::iterator iter;
		iter = --traj.end();
		return sqrt(pow(((*iter)[0] - point2.first), 2) + pow(((*iter)[1] - point2.second), 2));
	}
	double VelocityCost(vector<vector<float>> traj)
	{
		vector<vector<float>> ::iterator iter;
		iter = --traj.end();
		return Vmax - (*iter)[3];
	}
	double ObstacleCost(vector<vector<float>> traj, vector<pair<int, int>> Obstacle)
	{
		double minDistance = INFINITY;
		double currentDistance;
		vector<vector<float>> ::iterator iterTraj;
		vector<pair<int, int>> ::iterator iterObs;
		for (iterTraj = traj.begin(); iterTraj != traj.end(); iterTraj++)
		{
			for (iterObs = Obstacle.begin(); iterObs != Obstacle.end(); iterObs++)
			{
				currentDistance = sqrt(pow(((*iterTraj)[0] - (*iterObs).first), 2) + pow(((*iterTraj)[1] - (*iterObs).second), 2));
				if (currentDistance <= minDistance)
				{
					minDistance = currentDistance;
				}
			}
		}
		return 1 / minDistance;
	}
	vector<float> ActualVRange(vector<float> X)
	{
		vector<float> VRange(4);
		float VminActual = X[3] - Va * Dt;
		float VmaxActual = X[3] + Va * Dt;
		float WminActual = X[4] - Wa * Dt;
		float WmaxActual = X[4] + Wa * Dt;
		VRange[0] = max(VminActual, (float)Vmin);
		VRange[1] = min(VmaxActual, (float)Vmax);
		VRange[2] = max(WminActual, (float)Wmin);
		VRange[3] = min(WmaxActual, (float)Wmax);
		return VRange;
	}
	vector<float> ActMotion(vector<float> X, float V, float W)
	{
		vector<float> xNew(5);
		xNew[0] = X[0] + V * Dt * cos(X[2]);
		xNew[1] = X[1] + V * Dt * sin(X[2]);
		xNew[2] = X[2] + W * Dt;
		xNew[3] = V;
		xNew[4] = W;
		return xNew;
	}
	vector<vector<float>> GoTraj(vector<float> X, float V, float W)
	{
		vector<vector<float>> traj;
		float time = 0;
		while (time < PredictTime)
		{
			traj.push_back(ActMotion(X, V, W));
			time += Dt;
		}
		return traj;
	}
	vector<float> DwaCore(vector<float> X, pair<int, int> goal, vector<pair<int, int>> Obstacle)
	{
		vector<float> VRange(4);
		vector<float> VW(2);
		VRange = ActualVRange(X);
		vector<vector<float>> traj;
		float minScore = INFINITY;
		float DistScore, VScore, ObstacleScore, score;
		for (float V = VRange[0]; V < VRange[1]; V += Vreso)
		{
			for (float W = VRange[2]; W < VRange[3]; W += Wreso)
			{
				traj = GoTraj(X, V, W * PI / 180);
				DistScore = DistCost(traj, goal);
				VScore = VelocityCost(traj);
				ObstacleScore = ObstacleCost(traj, Obstacle);
				score = Alpha * DistScore + Belta * VScore + Gamma * ObstacleScore;
				if (minScore >= score)
				{
					minScore = score;
					VW[0] = V;
					VW[1] = W;
				}
			}
		}
		return VW;
	}
	vector<vector<float>> RunDWA(vector<float> startState, pair<int, int> goal, RoboTax::MapMessage map)
	{
		int i = 10;
		vector<float> VW(2), X = startState;
		vector<vector<float>> finalTraj;
		vector<pair<int, int>> obstacle;
		obstacle = GetObstacle(make_pair((int)startState[0], (int)startState[1]), goal, map);
		VW[0] = startState[3];
		VW[1] = startState[4];
		while (i != 0)
		{
			VW = DwaCore(startState, goal, obstacle);
			X = ActMotion(X, VW[0], VW[1]);
			finalTraj.push_back(X);
			if (sqrt(pow((X[0] - goal.first), 2) + pow((X[1] - goal.second), 2)) <= Radius)
			{
				std::cout << "Arrived" << endl;
				break;
			}
			i--;
			std::cout << i << endl;
		}
		return finalTraj;
	}

}
int main()
{
	RoboTax::MapMessage map = RoboTax::GenerateRandomMap(10, 10, 0.1);
	PrintMap(map);
	vector<vector<float>> finalTraj;
	vector<float> start(5);
	start[0] = 0;
	start[1] = 0;
	start[2] = 0;
	start[3] = 0;
	start[4] = 0;
	finalTraj = RunDWA(start, make_pair(8, 8), map);
}