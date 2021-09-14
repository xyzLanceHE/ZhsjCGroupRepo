#pragma once
#include<iostream>
#include<fstream>
#include<tuple>
#include<map>
#include<math.h>
#include<string>
#include <queue>
#include <vector>
#include <stack>
#include<algorithm>
using namespace std;

struct LOCATE
{
	int x_;
	int y_;
};

class D_star
{
public:
	explicit D_star();
	map<tuple<int, int>, double> Alldirec;   // ����ӽڵ�	

	map<tuple<int, int>, tuple<int, int>> b;  // ��¼����ָ�룬����һ���ڵ�ָ�򸸽ڵ㣬D*�Ƿ���ָ��
	map<tuple<int, int>, double> OPEN; // ���ڵ㼯
	map<tuple<int, int>, double> h;   // ·����ɢ��¼��
	map<tuple<int, int>, string> tag; // ��־��Open Closed New
	vector<tuple<int, int>> path;      // ��¼�滮·��������
	int count;                              // ��¼��������

	tuple<int, int> start;             // ��ʼ��λ��
	tuple<int, int> goal;              // ��ֹ��λ��

	double cost(tuple<int, int>&, tuple<int, int>&); // ŷʽ������㺯������ײ����һ�� ������
	void check_state(tuple<int, int>&);                   // �����Ϣ
	double get_kmin();                                         // ��ȡ���ڵ㼯��Сֵ
	tuple<tuple<int, int>, double> min_state();            // ��ȡ���ڵ㼯��Сֵ������Ԫ��
	void insert(tuple<int, int>&, double&);                // ���뿪�ڵ㼯������h��
	double process_state();                                    // D*�����㷨 ���ԭ����
	vector<tuple<int, int>> children(tuple<int, int>&);    // ��ȡ�ӽڵ�

	void modify_cost(tuple<int, int>&);                         // ���¶�̬�����½ڵ���Ϣ
	void modify(tuple<int, int>&);                              // ͬ�ϣ����ʹ��
	void get_path();                                                 // ��ȡ�滮·��
	void run();                                                      // ������ 
	void save_path(string);                                          // ����·����csv

	/*�˶��ϰ�����Ϣ*/
	double obs_r;
	tuple<int, int> obs_pos;
};

