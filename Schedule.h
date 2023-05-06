#pragma once

#include <vector>

using namespace std;

// ����ָ��ṹ��
struct schedulingCommond
{
    int takeStation; //�����
    int sellStation; //������
    int state;  //ִ��״̬��-1 ATTACK, 0��ʼ̬��1ȡ��̬��2�˻�̬��3���ɸ�ȡ��̬
    float efficient; //ָ���Ч��
};

extern bool newIncomeSwitch; // �Ƿ�ʹ�ø߼��ۺ�����
extern vector<vector<int>> s2sPath; //�ڵ���м�·��
extern vector<vector<int>> s2sPair;  //�ڵ��{i,j}
extern vector<float> s2sPriorty; //�ڵ����Ҫ��
extern vector<int> s2sRank; //�����ȼ�
extern int actionRank[5]; //�����ȼ�ӳ�䣨7��456->7��123->456,456,123��
extern vector<vector<float>> s2sDist;  //�ڵ����K*K
extern vector<vector<float>> s2sTimeLose; //����ʱ��˥��
extern float maxDist; //��Զ�����ڵ�ľ���max(s2sDist)
extern vector<vector<int>> criticalPair; // �ؼ�·���ڵ��
extern vector<float> criticalPairPriorty; //�ؼ�·���ڵ����Ҫ��
extern vector<int> criticalPairRank; //�ؼ�·���ڵ�Բ����ȼ�

/*-------------- ������ָ�� -----------------*/

// ��ǰָ��ʵ������
extern schedulingCommond cmd[4];

// ��һ�ε���ʷָ��ʵ������
extern schedulingCommond cmdOld[4];

// ������������ָ��ʵ������
extern schedulingCommond cmdVis[4];

// ��һ�ε�����վ��������������ʱˢ�£���������ʧ�ܺ�����ñ��գ� 0 take 1 sell
extern int lastOperationStation[4][2];

//һֱ��ִ����ͬ������˵�����������ܣ������������ֱײ��������������⣬��ʱֻ��ʱ����ײ
extern int sameCmdTime[4];

/*--------------�ڵ��ȷ��-----------------*/

// վ��׽��
void station_pair();

// վ��Ե�Ȩ�ؼ���
void move_priorty();

/*--------------̰��������-----------------*/

// ʵʱ̰��������
bool real_time_greedy_scheduler(int rob_i, bool newIncomeSwitch, bool visualSwitch, int rob_preempt);

// ����������
void scheduler_core();

// �ڹ���̨�Ͻ���״̬��ת�����
void state_wheel();

// ʵʱ����Ԥ�ܶ�������
bool real_time_runaway_scheduler(int rob_i, bool chooseFinishStationSwitch);

/*--------------�ؼ�·��������-----------------*/

// ��ȷ��sta_iվȱ��mat_i���Ϻ�Ѱ���ܹ��ϵ�վ��
void find_criticalPair(int sta_i, int mat_i);

// ��ѯվ7->8/9��priority
float sell7_search(int sta_i);

// �ؼ�·������
void critical_taskpair();

// ���Ծ�ָ���Ƿ���ڳ�ͻ���ܷ�ԭ��ָ�
bool test_old_cmd(int rob_i);

// �ؼ�·��������
void critical_scheduler();

// ������������
void bad_seller();