#pragma once

#include <vector>

using namespace std;

// �ܵ����վ
extern bool reachStation[2][4][50];
// �ܵ���Ļ�����
extern bool reachRobot[2][4][4];
// ·��
extern vector<vector<int>> path;
// �����˵�ס�ĸ���
extern int robotBlockPosition[2][4][13];
extern bool pathMap[4][100][100];
extern bool pathChess[4][101][101];

/*--------------��̬�ϰ�-----------------*/

// ͳ�ƻ����˵�ס�ĸ���
void robot_block_init();

// �ж�λ���Ƿ񱻻�����ռ��
bool have_block(int rob_i, int p_i, int p_j, bool mapSwitch);

/*--------------Ѱ·�㷨-----------------*/

// �ӻ�����rob_i��������
void flood(int rob_i, bool mapSwitch);

// ���������򷺺飨�����������˴������Ļ����˲���Ҫ�ٷ��飬ֱ�Ӹ��ƣ�
void all_flood(bool mapSwitch);

// A star �Զ�Ѱ·
bool a_star_search(int start, int goal, bool mapSwitch, bool robotBlockSwitch, int rob_i, bool enemyBlockSwitch);

/*--------------·���滮-----------------*/

// �ж���������֮����û��ǽ
bool through_wall(int start, int goal, float stepLength, bool mapSwitch, bool robotBlockSwitch, int rob_i, bool enemyBlockSwitch);

// ȥ��·���е������
void path_simple(int rob_i, bool mapSwitch, bool robotBlockSwitch, bool enemyBlockSwitch);

// ʵʱ·���滮
void real_time_path_search(int rob_i, bool robotBlockSwitch);

// �ĸ�����滮
void real_time_path_search_for_all(bool robotBlockSwitch);

/*--------------������ϵ-----------------*/

extern bool robotBlockByRobot[4][4];
extern float robotBlockByRobotDist[4][4];
extern int nearestRobot[4];
extern float nearestRobotDist[4];
extern int robotAviod[4];
extern int safePlace[4];
extern float safePlaceDis[4];
extern bool inLeave[4];
extern bool inSlow[4];

// ��֤·���Ƿ�ĳ�������赲
void path_block_by_robot(int rob_i, bool mapSwitch, float stepLength);

// Ѱ��ĳ�����˵ľͽ���ȫ��
int safe_place(int rob_i, bool mapSwitch);

// ������Ϣ
void init_avoid();

// ����ȫ��������ռλ
void path_block_by_robot_all();

// Ѱ��ȫ����ȫ��
void safe_place_all();

// ����������ϵȷ���رܵĻ�����
void avoid_choose();

// ���·���滮
void avoid_path_search();

// ·���������ܣ�
void path_search();

// ���ϣ���ȫ���������ͻ���þ��ߣ�
void avoid();