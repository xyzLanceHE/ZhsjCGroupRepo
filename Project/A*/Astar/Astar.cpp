
#include "Astar.h"
int map[101][101] =
{
	{0,0,0,1,0,1,0,0,0},
	{0,0,0,1,0,1,0,0,0},
	{0,0,0,0,0,1,0,0,0},
	{0,0,0,1,0,1,0,1,0},
	{0,0,0,1,0,1,0,1,0},
	{0,0,0,1,0,0,0,1,0},
	{0,0,0,1,0,0,0,1,0}
};
Astar::Astar()
{
}
Astar::~Astar()
{
}
void Astar::search(Node* startPos, Node* endPos)
{
	if (startPos->x < 0 || startPos->x > row || startPos->y < 0 || startPos->y >col
		||
		endPos->x < 0 || endPos->x > row || endPos->y < 0 || endPos->y > col)
		return;
	Node* current;
	this->startPos = startPos;
	this->endPos = endPos;
	openList.push_back(startPos);
	//��Ҫ����飬�ѿ�ʼ�Ľڵ����openlist��ʼ�����Աߵ�8���ڵ㣬������곬����Χ����closelist��return ����Ѿ�����openlist�ͶԱȵ�ǰ�ڵ㵽���������Ǹ��ڵ��Gֵ�͵�ǰ�ڵ㵽ԭ�����ڵ��Gֵ ���ԭ����Gֵ�Ƚϴ� ���ù� �������¸�ֵGֵ ���ڵ� ��f ������½ڵ� ���뵽openlistֱ��opellistΪ�ջ��ҵ��յ�
	while (openList.size() > 0)
	{
		current = openList[0];
		if (current->x == endPos->x && current->y == endPos->y)
		{
			cout << "find the path" << endl;
			printMap();
			printPath(current);
			openList.clear();
			closeList.clear();
			break;
		}
		NextStep(current);
		closeList.push_back(current);
		openList.erase(openList.begin());
		sort(openList.begin(), openList.end(), compare);
	}
}
void Astar::checkPoit(int x, int y, Node* father, int g)
{
	if (x < 0 || x > row || y < 0 || y > col)
		return;
	if (this->unWalk(x, y))
		return;
	if (isContains(&closeList, x, y) != -1)
		return;
	int index;
	if ((index = isContains(&openList, x, y)) != -1)
	{
		Node* point = openList[index];
		if (point->g > father->g + g)
		{
			point->father = father;
			point->g = father->g + g;
			point->f = point->g + point->h;
		}
	}
	else
	{
		Node* point = new Node(x, y, father);
		countGHF(point, endPos, g);
		openList.push_back(point);
	}
}
void Astar::NextStep(Node* current)
{
	checkPoit(current->x - 1, current->y, current, WeightW);//��
	checkPoit(current->x + 1, current->y, current, WeightW);//��
	checkPoit(current->x, current->y + 1, current, WeightW);//��
	checkPoit(current->x, current->y - 1, current, WeightW);//��
	checkPoit(current->x - 1, current->y + 1, current, WeightWH);//����
	checkPoit(current->x - 1, current->y - 1, current, WeightWH);//����
	checkPoit(current->x + 1, current->y - 1, current, WeightWH);//����
	checkPoit(current->x + 1, current->y + 1, current, WeightWH);//����
}
int Astar::isContains(vector<Node*>* Nodelist, int x, int y)
{
	for (int i = 0; i < Nodelist->size(); i++)
	{
		if (Nodelist->at(i)->x == x && Nodelist->at(i)->y == y)
		{
			return i;
		}
	}
	return -1;
}
void Astar::countGHF(Node* sNode, Node* eNode, int g)
{
	int h = abs(sNode->x - eNode->x) * WeightW + abs(sNode->y - eNode->y) * WeightW;
	int currentg = sNode->father->g + g;
	int f = currentg + h;
	sNode->f = f;
	sNode->h = h;
	sNode->g = currentg;
}
bool Astar::compare(Node* n1, Node* n2)
{
	//printf("%d,%d",n1->f,n2->f);
	return n1->f < n2->f;
}
bool Astar::unWalk(int x, int y)
{
	if (map[x][y] == 1)
		return true;
	return false;
}
void Astar::printPath(Node* current)
{
	if (current->father != NULL)
		printPath(current->father);
	map[current->x][current->y] = 4;
	printf("(%d,%d)", current->x, current->y);
}
void Astar::printMap()
{
	for (int i = 0; i <= row; i++) {
		for (int j = 0; j <= col; j++) {
			printf("%d ", map[i][j]);
		}
		printf("\n");
	}
}