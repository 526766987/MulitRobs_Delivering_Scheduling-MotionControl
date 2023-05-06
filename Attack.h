#pragma once

#include <vector>

using namespace std;

extern vector<vector<int>> s2sEnemyPath; //�ڵ���м�·��
extern vector<vector<int>> s2sEnemyPair;  //�ڵ��{i,j}
extern vector<float> s2sEnemyPriorty; //�ڵ����Ҫ��
extern vector<int> s2sEnemyRank; //�����ȼ�
extern vector<vector<float>> s2sEnemyDist;  //�ڵ����K*K
extern float maxEnemyDist; //��Զ�����ڵ�ľ���max(s2sDist)
extern bool reachEnemyStation[4][50]; // �ܵ����վ
extern bool attackerInPostion[4]; // �������Ƿ��ڹ������λ
extern int attackerState[4]; //������״̬
extern float old_defensiveCoor[4][2]; //����Ŀ�������¼
extern int old_attackerState[4]; //������״̬��¼
extern int unattackTimer[4]; //��¼��ǰ������û�ез����ĳ���֡��
extern int currentAttackEnemyStation[4]; //��¼��ǰ����վ����

// �з�վ��׽��
void station_pair_attacker();
void flood_attacker(int rob_i);
void all_flood_attacker();

// վ��Ե�Ȩ�ؼ���
void move_priorty_attacker();

// �з�����վ
extern vector<int> cornerEnemyStation;
extern vector<int> cornerEnemyStationKind;
void corner_enemy_station();

// �з��ܵ�վ
extern vector<int> pipeEnemyStation;
void pipe_enemy_station();

// �з��ؼ��ڵ�
extern vector<int> keyEnemyStation;
void key_enemy_station_attacker();

// ������ѡ����
void attacker_choose(int attackerAmount);

// ����Ŀ��ʵʱѡ�����������Ƿ�ִ�гɹ�
void attack_target_choose(int rob_i);

//�����߸���
void attacker_follow(int rob_i);

// �������˶�����
void defensive_motion_control(int rob_i, float defensiveCoor[2]);

// ������״̬��ת
void attack_wheel();

//������׷β�ж�
void crazy_attacker_follow(int rob_i);