#include <iostream>
#include <math.h>
#include <TinyROS/TinyROS.h>
#include "TinyROS/SharedMessageTypes.h"
#include <vector>
using namespace std;
namespace RoboTax
{
	vector<vector<int>> VectorizedMap(RoboTax::MapMessage map);
	vector<pair<int, int>> GetObstacle(pair<int, int>point1, pair<int, int>point2, vector<vector<int>> vectorMap);
	double DistCost(pair<int, int>point1, pair<int, int>point2);
	double VelocityCost(vector<pair<int, int>> pos);
	double ObstacleCost(vector<pair<int, int>> pos, vector<pair<int, int>> Obstacle);
}