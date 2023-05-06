#include <vector>
#include <cmath>
#include "Schedule.h"
#include "Global.h"
#include "Path.h"
#include "Calculate.h"
#include "Out.h"
#include "Motion.h"
#include "Map.h"

using namespace std;

// �Ƿ�ʹ�ø߼��ۺ�����
bool newIncomeSwitch = false;

// ��ǰָ��
schedulingCommond cmd[4] = {
    {-1,-1,1,0},
    {-1,-1,1,0},
    {-1,-1,1,0},
    {-1,-1,1,0}
};

// ��һ�ε���ʷָ��
schedulingCommond cmdOld[4] = {
    {-1,-1,1,0},
    {-1,-1,1,0},
    {-1,-1,1,0},
    {-1,-1,1,0}
};

// ������������ָ��
schedulingCommond cmdVis[4] = {
    {-1,-1,1,0},
    {-1,-1,1,0},
    {-1,-1,1,0},
    {-1,-1,1,0}
};

vector<vector<int>> s2sPath; //�ڵ���м�·��
vector<vector<int>> s2sPair;  //�ڵ��{i,j}
vector<float> s2sPriorty; //�ڵ����Ҫ��
vector<int> s2sRank; //�����ȼ�
int actionRank[5] = { 5,4,3,2,1 }; //�����ȼ�ӳ�䣨7��456->7��123->456,456,123��
vector<vector<float>> s2sDist(50, vector<float>(50));  //�ڵ����K*K
vector<vector<float>> s2sTimeLose(50, vector<float>(50)); //����ʱ��˥��
float maxDist; //��Զ�����ڵ�ľ���max(s2sDist)

// ��һ�ε�����վ��������������ʱˢ�£���������ʧ�ܺ�����ñ��գ� 0 take 1 sell
int lastOperationStation[4][2] = { {-1,-1},{-1,-1},{-1,-1},{-1,-1} };

//һֱ��ִ����ͬ������˵�����������ܣ������������ֱײ
int sameCmdTime[4] = { 0,0,0,0 };

/* վ��׽�� */
void station_pair()
{
    // ��ͼ����
    all_flood(0);
    all_flood(1);

    maxDist = 0;
    for (int i = 0; i < state.K; i++)
    {
        for (int j = 0; j < state.K; j++)
        {
            //����һ���������ܵ���������վ������ĵ�ͼ�õ���chessBoard��chessBoard��ǽ�����Ƹ��ӿ��ɣ����Ͻ⼯�걸��
            if ((reachStation[1][0][i] && reachStation[1][0][j]) || (reachStation[1][1][i] && reachStation[1][1][j]) ||
                (reachStation[1][2][i] && reachStation[1][2][j]) || (reachStation[1][3][i] && reachStation[1][3][j]))
            {
                // ��֤�ܷ������һ��վ�ִ���һ��վ����������루��ͼexpandWall��
                if (a_star_search(state.station[i].p_ij[0] + state.station[i].p_ij[1] * 100, state.station[j].p_ij[0] + state.station[j].p_ij[1] * 100, 0, 0, 0,false))
                {
                    if ((state.station[i].type <= 7) &&
                        ((stationType[state.station[j].type].buyMaterials & (1 << state.station[i].type)) > 0))
                    {
                        s2sPair.push_back({ i,j });
                        s2sPath.push_back(path[0]);
                        s2sPriorty.push_back(-1); //Ԥ��
                        s2sRank.push_back(-1); //Ԥ��
                    }
                    s2sDist[i][j] = path[0].size() * 0.5;
                    s2sTimeLose[i][j] = f_func(s2sDist[i][j] / MAXV, 9000, 0.8); //Ⱥ���������ˣ����ﻹ��9000
                    maxDist = (maxDist < s2sDist[i][j]) ? s2sDist[i][j] : maxDist;
                }
                else
                {
                    s2sDist[i][j] = 10000; //û��·�������������
                    s2sTimeLose[i][j] = 0;
                }
            }
            else
            {
                s2sDist[i][j] = 10000; //û��·�������������
                s2sTimeLose[i][j] = 0;
            }
        }
    }
    move_priorty();
}

/* վ��Ե�Ȩ�ؼ��� */
void move_priorty()
{
    int sta_i, sta_j;
    for (int pair_i = 0; pair_i < s2sPair.size(); pair_i++)
    {
        sta_i = s2sPair[pair_i][0];
        sta_j = s2sPair[pair_i][1];
        // ����7
        if (state.station[sta_i].type == 7)
        {
            s2sPriorty[pair_i] = 1 * (1 - s2sDist[sta_i][sta_j] / maxDist);
            s2sRank[pair_i] = actionRank[0];
        }
    }
    for (int pair_i = 0; pair_i < s2sPair.size(); pair_i++)
    {
        sta_i = s2sPair[pair_i][0];
        sta_j = s2sPair[pair_i][1];
        // ����456
        if ((state.station[sta_i].type <= 6) && (state.station[sta_i].type >= 4))
        {
            float upRank = -1;
            for (int pair_j = 0; pair_j < s2sPair.size(); pair_j++)
            {
                if (s2sPair[pair_j][0] == sta_j)
                {
                    upRank = (upRank < s2sPriorty[pair_j]) ? s2sPriorty[pair_j] : upRank;
                }
            }
            if (upRank == -1)
            {
                s2sPriorty[pair_i] = 1 * (1 - s2sDist[sta_i][sta_j] / maxDist);
                s2sRank[pair_i] = actionRank[3];
            }
            else
            {
                s2sPriorty[pair_i] = upRank * (1 - s2sDist[sta_i][sta_j] / maxDist);
                s2sRank[pair_i] = actionRank[1];
            }
        }
    }
    for (int pair_i = 0; pair_i < s2sPair.size(); pair_i++)
    {
        sta_i = s2sPair[pair_i][0];
        sta_j = s2sPair[pair_i][1];
        // ����123
        if ((state.station[sta_i].type <= 3))
        {
            float upRank = -1;
            for (int pair_j = 0; pair_j < s2sPair.size(); pair_j++)
            {
                if (s2sPair[pair_j][0] == sta_j)
                {
                    upRank = (upRank < s2sPriorty[pair_j]) ? s2sPriorty[pair_j] : upRank;
                }
            }
            if (upRank == -1)
            {
                s2sPriorty[pair_i] = 1 * (1 - s2sDist[sta_i][sta_j] / maxDist);
                s2sRank[pair_i] = actionRank[4];
            }
            else
            {
                s2sPriorty[pair_i] = upRank * (1 - s2sDist[sta_i][sta_j] / maxDist);
                s2sRank[pair_i] = actionRank[2];
            }
        }
    }
}

/* ���������� */
void scheduler_core()
{
    //ָ�����
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 1)) {
            cmd[rob_i].state = 0;
            cmd[rob_i].takeStation = -1;
            cmd[rob_i].sellStation = -1;
            cmd[rob_i].efficient = 0;
        }
    }

    //�Ȱ���ʧ�ܵĻ����˽����
    bad_seller();

    /* �����ؼ�·�� */
    critical_taskpair();
    critical_scheduler();
    critical_taskpair();
    critical_scheduler();
    critical_taskpair();
    critical_scheduler();
    critical_taskpair();
    critical_scheduler();

    //���ؼ�·�������������̰������
    //�ؼ�·����cmd.state�ᱻ��ʱ����Ϊ3���Է�ֹ��̰�����������ǣ�������1
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 0)) {
            cmd[rob_i].state = 1;
        }
    }

    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        // �ڲ�����ָ���ͻ������½����������
        if ((cmd[rob_i].state == 1)) {
            // �޻���ʻ
            real_time_greedy_scheduler(rob_i, newIncomeSwitch, true, -1);
        }
        else
        {
            // ������ʻ��ָ��䣩
            cmdVis[rob_i].takeStation = -1;
            cmdVis[rob_i].sellStation = -1;
            cmdVis[rob_i].efficient = -1;
        }
    }

    // ��֤����������Ƿ��г�ͻ
    bool flag = true;
    int round = 0;
    while (flag)
    {
        // �㷨��5�������ɴ��ڳ�ͻ����ôǿ��ʹ�ó�ͻ����
        if (round > 5) {
            for (int rob_i = 0; rob_i < 4; rob_i++)
            {
                if (cmd[rob_i].state == 1)
                {
                    real_time_greedy_scheduler(rob_i, newIncomeSwitch, false, -1);
                }
            }
            break;
        }

        flag = false;

        for (int rob_i = 0; rob_i < 4; rob_i++)
        {
            if (cmdVis[rob_i].sellStation == -1) { continue; }
            for (int rob_j = rob_i + 1; rob_j < 4; rob_j++)
            {
                if (cmdVis[rob_i].sellStation == -1) { continue; } //�����޻�����ָ��
                
                //rob_i��rob_j�ĳ�ͻ��֤

                // ����س�ͻ||������+�������ͻ
                if ((cmdVis[rob_i].takeStation == cmdVis[rob_j].takeStation) ||
                    ((cmdVis[rob_i].sellStation == cmdVis[rob_j].sellStation) && (state.station[cmdVis[rob_i].takeStation].type == state.station[cmdVis[rob_j].takeStation].type)))
                {
                    flag = true;
                    schedulingCommond cmdVis_i = cmdVis[rob_i];
                    schedulingCommond cmdVis_j = cmdVis[rob_j];

                    //plan A:�滻��rob_i��cmd
                    real_time_greedy_scheduler(rob_i, newIncomeSwitch, true, rob_j);
                    schedulingCommond cmdVis_i_new = cmdVis[rob_i];
                    cmdVis[rob_i] = cmdVis_i;

                    //plan B:�滻��rob_j��cmd
                    real_time_greedy_scheduler(rob_j, newIncomeSwitch, true, rob_i);
                    schedulingCommond cmdVis_j_new = cmdVis[rob_j];
                    cmdVis[rob_j] = cmdVis_j;

                    //�Ƚ�plan
                    if ((cmdVis_i_new.efficient == 0) && (cmdVis_j_new.efficient == 0))
                    {
                        //���������˫����Ψһ�⣬˭������˭��
                        if (cmdVis_j.efficient > cmdVis_i.efficient)
                        {
                            cmdVis[rob_i].takeStation = -1;
                            cmdVis[rob_i].sellStation = -1;
                            cmdVis[rob_i].efficient = 0;
                        }
                        else
                        {
                            cmdVis[rob_j].takeStation = -1;
                            cmdVis[rob_j].sellStation = -1;
                            cmdVis[rob_j].efficient = 0;
                        }
                    }
                    else if (cmdVis_i_new.efficient + cmdVis_j.efficient > cmdVis_j_new.efficient + cmdVis_i.efficient)
                    {
                        //��i�����Ȱ�j������
                        cmdVis[rob_i] = cmdVis_i_new;
                    }
                    else
                    {
                        //��j�����Ȱ�i������
                        cmdVis[rob_j] = cmdVis_j_new;
                    }
                }
            }
        }
        round++;
    }

    if (!flag)
    {
        for (int rob_i = 0; rob_i < 4; rob_i++)
        {
            if (cmd[rob_i].state == 1)
            {
                cmd[rob_i].sellStation = cmdVis[rob_i].sellStation;
                cmd[rob_i].takeStation = cmdVis[rob_i].takeStation;
                cmd[rob_i].efficient = cmdVis[rob_i].efficient;
            }
        }
    }

    //���ؼ�·����state��ԭ��1
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if (cmd[rob_i].state == 3)
        {
            cmd[rob_i].state = 1;
        }
    }


}

/* �ڹ���̨�Ͻ���״̬��ת */
void state_wheel()
{
    // �����û����˵�Ԥ�ܶ�վ�������������ռ���˵�λ��
    for (int rob_i = 0; rob_i < 4; rob_i++) {
        if (cmd[rob_i].sellStation == -1)
        {
            cmd[rob_i].takeStation = -1;
        }
        cmdVis[rob_i].takeStation = -1;
        cmdVis[rob_i].sellStation = -1;
        cmdVis[rob_i].efficient = 0;
    }

    // ����״̬����������
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 2)) {
            //�����ͬ����ʵ��û���������ת��״̬
            if (state.robot[rob_i].bring == 0) { cmd[rob_i].state = 1; continue; }
            //�������زſ�������
            if (state.robot[rob_i].atStation == cmd[rob_i].sellStation) {
                if (!pro_exsit(state.robot[rob_i].bring, state.station[cmd[rob_i].sellStation].materials)) {
                    // ��������
                    send_cmd_sell(rob_i);

                    // ״̬����
                    cmd[rob_i].state = 1;
                    if (state.robot[rob_i].bring < 7) {
                        state.station[cmd[rob_i].sellStation].materials |= (1 << state.robot[rob_i].bring);
                    }
                    state.robot[rob_i].bring = 0;
                    rotate_PID_init(rob_i);

                    //��ֹû���ɹ�����һ��
                    lastOperationStation[rob_i][0] = cmd[rob_i].takeStation;
                    lastOperationStation[rob_i][1] = cmd[rob_i].sellStation;
                }
            }
        }
    }

    //��ʼ�µ���
    scheduler_core();

    //���û�����Ԥ�ܶ�
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].sellStation == -1) && (state.robot[rob_i].attacker == false))
        {
            if (real_time_runaway_scheduler(rob_i, true) == false)
            {
                real_time_runaway_scheduler(rob_i, false);
            }
        }
    }

    // �����ָ��
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmdOld[rob_i].state == cmd[rob_i].state) &&
            (cmdOld[rob_i].takeStation == cmd[rob_i].takeStation) &&
            (cmdOld[rob_i].sellStation == cmd[rob_i].sellStation))
        {
            sameCmdTime[rob_i]++;
        }
        else
        {
            cmdOld[rob_i].state = cmd[rob_i].state;
            cmdOld[rob_i].takeStation = cmd[rob_i].takeStation;
            cmdOld[rob_i].sellStation = cmd[rob_i].sellStation;
            cmdOld[rob_i].efficient = cmd[rob_i].efficient;
            sameCmdTime[rob_i] = 0;
        }
    }

    // �޻�״̬����������
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 1)) {
            // ����������ϻ�Я���˶����Ļ���˵��û�����ɹ�����ԭ��ʷָ��
            if (state.robot[rob_i].bring != 0) {
                cmd[rob_i].state = 2;
                cmd[rob_i].takeStation = lastOperationStation[rob_i][0];
                cmd[rob_i].sellStation = lastOperationStation[rob_i][1];
                continue;
            }
            //�����µ��Ⱥ������ؾ��ڴ˹���̨����ֱ���򣬲��������ﵽ������
            if (state.robot[rob_i].atStation == cmd[rob_i].takeStation) {
                //ǿ���ж�ʣ��ʱ�䲻�ɹ�������գ�
                if (state.timeStamp > DEADLINE - 250) { continue; }
                //�����ж���������
                if (state.station[cmd[rob_i].takeStation].beenOccupy == true) { continue; }
                if (state.station[state.robot[rob_i].atStation].finish == 1) {
                    if (cmd[rob_i].sellStation == -1) { continue; } //ΪԤ�ܶ����ã�ֻ��Ԥ�ܶ���û��ȷ��������
                    if (state.station[cmd[rob_i].takeStation].beenOccupy) { continue; }
                    if (state.station[cmd[rob_i].sellStation].beenOccupy) { continue; }
                    //�������
                    send_cmd_buy(rob_i);

                    //״̬����
                    cmd[rob_i].state = 2;
                    state.station[cmd[rob_i].takeStation].finish = 0;
                    state.robot[rob_i].bring = state.station[cmd[rob_i].takeStation].type;
                    rotate_PID_init(rob_i);
                }
            }
        }
    }
}


/* ʵʱ̰�������� */
bool real_time_greedy_scheduler(int rob_i, bool newIncomeSwitch, bool visualSwitch, int rob_preempt)
{
    float efficient = 0;
    vector<int> bestPair = { -1,-1 };
    vector<vector<int>>::iterator iter;
    int iterIndex = 0;
    //��ÿ����������Ͻ��е���
    for (iter = s2sPair.begin(); iter != s2sPair.end(); iter++) {
        if (state.station[(*iter)[0]].beenOccupy) { continue; }
        if (state.station[(*iter)[1]].beenOccupy) { continue; }
        //���������Ŀǰ������صľ��루ֱ�߾����жϣ�
        float r2tDist = sqrt(pow(state.robot[rob_i].p[0] - state.station[(*iter)[0]].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[(*iter)[0]].p[1], 2));
        //���������صľ��루·�߹滮�ĳ��о��룩
        float t2sDist = s2sDist[(*iter)[0]][(*iter)[1]];

        //������ؽ���ɸѡ
        //����ȥû�������վ�㹺��chessBoard
        if (reachStation[1][rob_i][(*iter)[0]] == false) { continue; }
        //���Ŀǰû�в������ж���������
        if (state.station[(*iter)[0]].finish == 0) {
            //Ŀǰ�������ģ����������
            if (state.station[(*iter)[0]].inBusy == -1) { continue; }
            //Ŀǰ������������ʱҲ���������ģ����������
            if (state.station[(*iter)[0]].inBusy > r2tDist / MAXV) { continue; };
        }

        //�������ؽ���ɸѡ
        //���������ģ����������
        if (((r2tDist + t2sDist) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { continue; }
        //���������ԭ���ϸ�����������ϣ����������
        if ((state.station[(*iter)[1]].materials & (1 << state.station[(*iter)[0]].type)) > 0) { continue; }

        if (visualSwitch == false) // ��ʵ����
        {
            //������ָ���
            bool flag = false;
            for (int i = 0; i < 4; i++)
            {
                //�ų��Լ�
                if (i == rob_i) { continue; }
                //���ܺ����������˵���ͬվȡ��
                if ((cmd[i].takeStation == (*iter)[0]) && (cmd[i].state == 1)) { flag = true; break; }
                //����������������������ͬ���ﵽ������
                if ((cmd[i].sellStation == (*iter)[1]) && (state.station[cmd[i].takeStation].type == state.station[(*iter)[0]].type)) { flag = true; break; }
                // �ؼ�·��ȡ��
                if ((cmd[i].takeStation == (*iter)[0]) && (cmd[i].state == 3)) { flag = true; break; }
            }
            if (flag == true) { continue; }
        }
        else // �������
        {
            if (rob_preempt >= 0)
            {
                //rob_preempt�����˵�������ų�
                if ((cmdVis[rob_preempt].takeStation == (*iter)[0]) && (cmdVis[rob_preempt].state == 1)) { continue; }
                //������rob_preempt������������ͬ���ﵽ������
                if ((cmdVis[rob_preempt].sellStation == (*iter)[1]) && (state.station[cmdVis[rob_preempt].takeStation].type == state.station[(*iter)[0]].type)) { continue; }
            }
            bool flag = false;
            for (int i = 0; i < 4; i++)
            {
                //�ų��Լ�
                if (i == rob_i) { continue; }
                //�����ر��˻�̬������Ԥ����������
                if ((cmd[i].state == 2) && (cmd[i].sellStation == (*iter)[1]) && (state.station[cmd[i].takeStation].type == state.station[(*iter)[0]].type)) { flag = true; break; }
                // �ؼ�·��ȡ��
                if ((cmd[i].takeStation == (*iter)[0]) && (cmd[i].state == 3)) { flag = true; break; }
            }
            if (flag == true) { continue; }
        }

        //�����Ϊ���н⣬��ʼ���㵱ǰ���Ч��
        //��ǰ�����ʵ����+Ԥ������
        float income = income_cal((*iter)[0], (*iter)[1]);

        // ��ͷ�ͷ�
        float punishment = 0;
        float theta = atan2(state.station[(*iter)[0]].p[1] - state.robot[rob_i].p[1], state.station[(*iter)[0]].p[0] - state.robot[rob_i].p[0]);
        theta = abs(theta - state.robot[rob_i].direction);
        theta = (theta > PI) ? (2 * PI - theta) : theta;
        if (theta > PI * 2 / 3)
        {
            punishment = 5;
        }

        //��ǰ���Ч��
        float local_efficient;
        if (newIncomeSwitch)
        {
            local_efficient = (new_income_cal((*iter)[0], (*iter)[1], s2sPriorty[iterIndex]) + stationType[state.station[(*iter)[1]].type].productValue) / new_dist_cal(r2tDist, t2sDist, (fabs(clamping_angle(rob_i, state.station[(*iter)[0]].p)) / PI));
        }
        else
        {
            local_efficient = income / (r2tDist + t2sDist + punishment);
        }

        //��ǰ���Ч�����ʷЧ��ߣ������Ϊ��ǰ���
        if (local_efficient > efficient)
        {
            efficient = local_efficient;
            bestPair[0] = (*iter)[0];
            bestPair[1] = (*iter)[1];
        }
        iterIndex++;
    }

    if (visualSwitch == false) // ��ʵ����
    {
        //�������ϸ�ֵ��cmd�����޽�Ҳ�ḳ-1
        cmd[rob_i].takeStation = bestPair[0];
        cmd[rob_i].sellStation = bestPair[1];
        cmd[rob_i].efficient = efficient;
    }
    else
    {
        //�������ϸ�ֵ��cmdVis�����޽�Ҳ�ḳ-1
        cmdVis[rob_i].takeStation = bestPair[0];
        cmdVis[rob_i].sellStation = bestPair[1];
        cmdVis[rob_i].efficient = efficient;
    }

    //����޽⣬�򷵻�false
    if (bestPair[0] == -1 || bestPair[1] == -1) {
        return false;
    }
    //���ָ��
    return true;
}


/* ʵʱ����Ԥ�ܶ������� */
bool real_time_runaway_scheduler(int rob_i, bool chooseFinishStationSwitch)
{
    // û����������Ļ����ˣ������ȵ������һ���в�Ʒ��û�����վ���ţ�������ĵط���
    float efficient = 0;
    vector<int> bestPair = { -1,-1 };
    vector<vector<int>>::iterator iter;
    int iterIndex = 0;

    float minDist = 10000;
    //��ÿ����������Ͻ��е���
    for (iter = s2sPair.begin(); iter != s2sPair.end(); iter++) {
        if (state.station[(*iter)[0]].beenOccupy) { continue; }
        if (state.station[(*iter)[1]].beenOccupy) { continue; }

        // ֻȥ123
        if (state.station[(*iter)[0]].type > 3) { continue; }

        //���������Ŀǰ������صľ��루ֱ�߾��룩
        float r2tDist = sqrt(pow(state.robot[rob_i].p[0] - state.station[(*iter)[0]].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[(*iter)[0]].p[1], 2));
        if (r2tDist >= minDist) { continue; }
        
        //������ؽ���ɸѡ
        //����ȥû�������վ�㹺��chessBoard��
        if (reachStation[1][rob_i][(*iter)[0]] == false) { continue; }
        if (chooseFinishStationSwitch)
        {
            //���Ŀǰû�в������ж���������
            if (state.station[(*iter)[0]].finish == 0) {
                //Ŀǰ�������ģ����������
                if (state.station[(*iter)[0]].inBusy == -1) { continue; }
                //Ŀǰ������������ʱҲ���������ģ����������
                if (state.station[(*iter)[0]].inBusy > r2tDist / MAXV) { continue; };
            }
        }

        // ������ָ���ͻ
        bool flag = false;
        for (int i = 0; i < 4; i++)
        {
            //�ų��Լ�
            if (i == rob_i) { continue; }
            //���������˵�������ų�
            if ((cmd[i].takeStation == (*iter)[0]) && (cmd[i].state == 1)) { flag = true; break; }
            //���������˵�������ų�
            if ((cmd[i].takeStation == (*iter)[0]) && (cmd[i].state == 3)) { flag = true; break; }
            // ���ܰѱ��������ĵط�����
            if (cmd[i].sellStation == (*iter)[0]) { flag = true; break; }
        }
        if (flag == true) { continue; }

        // ��¼����ѵ�;���
        bestPair[0] = (*iter)[0];
        minDist = r2tDist;

        iterIndex++;
    }

    //�������ϸ�ֵ��cmd�����޽�Ҳ�ḳ-1
    cmd[rob_i].takeStation = bestPair[0];
    cmd[rob_i].sellStation = -1;
    cmd[rob_i].efficient = 0;

    //����޽⣬�򷵻�false
    if (bestPair[0] == -1) {
        return false;
    }
    //���ָ��
    return true;
}


vector<vector<int>> criticalPair; // �ؼ�·���ڵ��
vector<float> criticalPairPriorty; //�ؼ�·���ڵ����Ҫ��
vector<int> criticalPairRank; //�ؼ�·���ڵ�Բ����ȼ�

/* ��ȷ��sta_iվȱ��mat_i���Ϻ�Ѱ���ܹ��ϵ�վ���*/
void find_criticalPair(int sta_i, int mat_i)
{
    //for (int rob_i = 0; rob_i < 4; rob_i++)
    //{
    //    // ��Ȼȱ���ϣ������Ѿ����˳�����
    //    if ((cmd[rob_i].sellStation == sta_i) && (state.station[cmd[rob_i].takeStation].type == mat_i))
    //    {
    //        return;
    //    }
    //}
    //for (int i = 0; i < state.K; i++)
    //{
    //    // ��Ȼȱ���ϣ������Ѿ���������
    //    if (state.station[i].type == mat_i)
    //    {
    //        if (state.station[i].inBusy > 0)
    //        {
    //            return;
    //        }
    //    }
    //}
    for (int pair_i = 0; pair_i < s2sPriorty.size(); pair_i++)
    {
        if (s2sPair[pair_i][1] != sta_i) { continue; }
        if (state.station[s2sPair[pair_i][0]].type != mat_i) { continue; }
        // վ�㱻ռ
        if (state.station[s2sPair[pair_i][0]].beenOccupy == true) { continue; }
        if (state.station[s2sPair[pair_i][1]].beenOccupy == true) { continue; }

        bool tempFlag = false;
        for (int cpair_i = 0; cpair_i < criticalPair.size(); cpair_i++)
        {
            // ����ͬ�Ķ�����ͬһ��վ
            if ((s2sPair[pair_i][1] == criticalPair[cpair_i][1]) &&
                (state.station[s2sPair[pair_i][0]].type == state.station[criticalPair[cpair_i][0]].type))
            {
                if (s2sDist[s2sPair[pair_i][0]][s2sPair[pair_i][1]] < s2sDist[criticalPair[cpair_i][0]][criticalPair[cpair_i][1]])
                {
                    criticalPair.erase(criticalPair.begin() + cpair_i);
                    criticalPairPriorty.erase(criticalPairPriorty.begin() + cpair_i);
                    criticalPairRank.erase(criticalPairRank.begin() + cpair_i);
                    break;
                }
                else
                {
                    tempFlag = true;
                    break;
                }
            }
        }
        if (tempFlag == true) { continue; }

        criticalPair.push_back(s2sPair[pair_i]);
        criticalPairPriorty.push_back(s2sPriorty[pair_i]);
        criticalPairRank.push_back(s2sRank[pair_i]);

        if (state.station[sta_i].type == 7) {
            // �¼���Ʒͬ��û������Ҳȱԭ��
            if ((state.station[s2sPair[pair_i][0]].finish == 0) && (state.station[s2sPair[pair_i][0]].inBusy <= 0))
            {
                for (int next_mat_i = 1; next_mat_i <= 3; next_mat_i++)
                {
                    if (((state.station[s2sPair[pair_i][0]].materials & (1 << next_mat_i)) == 0) &&
                        ((stationType[state.station[s2sPair[pair_i][0]].type].needMaterials & (1 << next_mat_i)) > 0))
                    {
                        //�¼���Ʒȱԭ��
                        find_criticalPair(s2sPair[pair_i][0], next_mat_i);
                    }
                }
            }
        }
    }
}

/* ��ѯվ7->8/9��priority */
float sell7_search(int sta_i)
{
    float highPriorty = 0;
    for (int pair_i = 0; pair_i < s2sPriorty.size(); pair_i++)
    {
        if (s2sPair[pair_i][0] == sta_i)
        {
            highPriorty = (highPriorty < s2sPriorty[pair_i]) ? s2sPriorty[pair_i] : highPriorty;
        }
    }
    return highPriorty;
}

/* �ؼ�·������ */
void critical_taskpair() {
    criticalPair.clear();
    criticalPairPriorty.clear();
    criticalPairRank.clear();
    vector<int> inHurry7; // ��û�п�ʼ������7
    vector<int> met7; // �Ѿ�ӵ�м���ԭ��

    for (int sta_i = 0; sta_i < state.K; sta_i++)
    {
        if ((state.station[sta_i].type == 7) && (state.station[sta_i].finish == 1)) //���Ѿ���������7��������
        {
            for (int pair_i = 0; pair_i < s2sPriorty.size(); pair_i++)
            {
                if (s2sPair[pair_i][0] == sta_i)
                {
                    // վ�㱻ռ
                    if (state.station[s2sPair[pair_i][0]].beenOccupy == true) { continue; }
                    if (state.station[s2sPair[pair_i][1]].beenOccupy == true) { continue; }
                    criticalPair.push_back(s2sPair[pair_i]);
                    criticalPairPriorty.push_back(s2sPriorty[pair_i]);
                    criticalPairRank.push_back(s2sRank[pair_i]);
                }
            }
        }

        //if ((state.station[sta_i].type == 7) && (state.station[sta_i].finish == 0) && (state.station[sta_i].inBusy <= 0)) //û�п�ʼ������7
        if (state.station[sta_i].type == 7)
        {
            // վ�㱻ռ
            if (state.station[sta_i].beenOccupy == true) { continue; }
            inHurry7.push_back(sta_i);
            int have = 0;
            for (int mat_i = 4; mat_i <= 6; mat_i++)
            {
                if ((state.station[sta_i].materials & (1 << mat_i)) > 0)
                {
                    have++;
                }
            }
            met7.push_back(sta_i);
        }
    }

    if (criticalPair.size() > 0)
    {
        for (int cpair_i = 0; cpair_i < criticalPair.size(); cpair_i++)
        {
            outdebug("criticalPair",criticalPair[cpair_i][0], criticalPair[cpair_i][1], criticalPairPriorty[cpair_i], criticalPairRank[cpair_i]);
        }
        return;
    }

    int maxMet = 0;
    int best7 = -1;
    float highPriorty = 0;
    for (int i = 0; i < inHurry7.size(); i++)
    {
        if ((maxMet < met7[i]) && (met7[i] != 3)) // ���Ȳ���ӵ�����ԭ���ϵ�7
        {
            highPriorty = sell7_search(inHurry7[i]); // 7->8/9�����ȼ�
            maxMet = met7[i];
            best7 = inHurry7[i];
        }
        else if ((maxMet == met7[i]) && (met7[i] != 3))
        {
            if (highPriorty < sell7_search(inHurry7[i])) // 7->8/9�����ȼ���������7���Ȳ���
            {
                highPriorty = sell7_search(inHurry7[i]);
                maxMet = met7[i];
                best7 = inHurry7[i];
            }
        }
    }

    if (best7 == -1) { return; }


    for (int mat_i = 4; mat_i <= 6; mat_i++)
    {
        if ((state.station[best7].materials & (1 << mat_i)) == 0)
        {
            find_criticalPair(best7, mat_i); // Ѱ�ҹؼ�վ���
        }
    }
}

/* ���Ծ�ָ���Ƿ���ڳ�ͻ���ܷ�ԭ��ָ� */
bool test_old_cmd(int rob_i)
{
    if (cmdOld[rob_i].state == 0) { return false; }
    if (cmdOld[rob_i].takeStation == -1) { return false; }
    if (cmdOld[rob_i].sellStation == -1) { return false; }
    if (state.station[cmd[rob_i].sellStation].beenOccupy) { return false; }

    int sta_i = cmdOld[rob_i].takeStation;
    int sta_j = cmdOld[rob_i].sellStation;

    bool flag = true;
    if (state.station[sta_i].finish == 0) {
        //Ŀǰ������������ʱҲ���������ģ����������
        float dis = sqrt(pow(state.robot[rob_i].p[0] - state.station[sta_i].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[sta_i].p[1], 2));
        if ((state.station[sta_i].inBusy < (dis / MAXV)) && (state.station[sta_i].inBusy > 0))
        {
            //�п��еĻ�����rob_i������ǰȥ����
            //�ж�һ�¸ÿ��л������Ƿ����ü�����
            //���������ģ����������
            if (((dis + s2sDist[sta_i][sta_j]) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { return false; }
            // Ҳ����˵���û����˿�������ʱ���������
            flag = false;
        }
    }
    else
    {
        float dis = sqrt(pow(state.robot[rob_i].p[0] - state.station[sta_i].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[sta_i].p[1], 2));
        if (((dis + s2sDist[sta_i][sta_j]) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { return false; }
        // Ҳ����˵���û����˿�������ʱ���������
        flag = false;
    }
    if (flag) { return false; } //��������������������

    if ((state.station[sta_i].materials & (1 << state.station[sta_i].type)) > 0) { return false; } //���У��޷��չ�

    flag = false;
    for (int i = 0; i < 4; i++)
    {
        if (i == rob_i) { continue; }
        if (((cmd[i].takeStation == sta_i) && (cmd[i].state == 1)) ||
            ((cmd[i].takeStation == sta_i) && (cmd[i].state == 3)) ||
            ((cmd[i].sellStation == sta_j) && (state.station[cmd[i].takeStation].type == state.station[sta_i].type)))
        {
            flag = true;
            break;
        }
    }
    if (flag) { return false; } //�����ѱ�ռ��

    // ��ֹ���������һ�����7/9
    if ((state.station[sta_j].type == 9) || (state.station[sta_j].type == 7))
    {
        int count = 0;
        for (int rob_i = 0; rob_i < 4; rob_i++)
        {
            if (cmd[rob_i].sellStation < 0) { continue; }
            if ((state.station[sta_j].type == state.station[cmd[rob_i].sellStation].type))
            {
                count++;
            }
        }
        if (cal_map_distance_d(state.station[sta_j].p) > 3)
        {
            if (count >= 3)
            {
                return false;
            }
        }
        else
        {
            if (count >= 1)
            {
                return false;
            }
        }
    }

    return true;
}

/* �ؼ�·�������� */
void critical_scheduler()
{
    // ����level
    vector<int> critical_rank_list = { 5,4,3,2,1 };
    //actionRank[0] = 5; // 7
    //actionRank[1] = 4; // 456->7
    //actionRank[2] = 3; // 123->456
    //actionRank[3] = 2; // 456
    //actionRank[4] = 1; // 123

    for (int rank_k = 0; rank_k < 5; rank_k++)
    {
        int rank = critical_rank_list[rank_k];
        float highest_priorty = 0;
        int sta_i = -1;
        int sta_j = -1;

        for (int pair_i = 0; pair_i < criticalPairRank.size(); pair_i++) {
            if (state.station[criticalPair[pair_i][0]].beenOccupy) { continue; }
            if (state.station[criticalPair[pair_i][1]].beenOccupy) { continue; }
            if (criticalPairRank[pair_i] == rank)
            {
                bool flag = true;
                if (state.station[criticalPair[pair_i][0]].finish == 0) {
                    //Ŀǰ������������ʱҲ���������ģ����������
                    for (int rob_i = 0; rob_i < 4; rob_i++)
                    {
                        if (cmd[rob_i].state == 0)
                        {
                            //�ų��������˵����˵�λ��
                            if (reachStation[1][rob_i][criticalPair[pair_i][0]] == false)
                            {
                                continue;
                            }
                            float dis = sqrt(pow(state.robot[rob_i].p[0] - state.station[criticalPair[pair_i][0]].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[criticalPair[pair_i][0]].p[1], 2));
                            if ((state.station[criticalPair[pair_i][0]].inBusy < (dis / MAXV)) && (state.station[criticalPair[pair_i][0]].inBusy > 0))
                            {
                                //�п��еĻ�����rob_i������ǰȥ����
                                //�ж�һ�¸ÿ��л������Ƿ����ü�����
                                //���������ģ����������
                                if (((dis + s2sDist[criticalPair[pair_i][0]][criticalPair[pair_i][1]]) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { continue; }
                                // Ҳ����˵���û����˿�������ʱ���������

                                //��֤�������ܷ���
                                if ((state.station[criticalPair[pair_i][1]].materials & (1 << state.station[criticalPair[pair_i][0]].type)) > 0) {
                                    //���ϸ����������ʿ���û�����ܰѳ�Ʒ���ߣ��ø��ӿճ���
                                    //����ʱ��ƷҲû�б��������������ߣ����������
                                    //�����������������޽�Ҫ�ڴ˹����Ʒ��
                                    float willbuyTime;
                                    for (int i = 0; i < 4; i++)
                                    {
                                        //�ų��Լ�
                                        if (i == rob_i) { continue; }
                                        //���У��������ﵽ����ʱ��
                                        if (cmd[i].takeStation == criticalPair[pair_i][1]) {
                                            float distr2b2 = sqrt(pow(state.robot[i].p[0] - state.station[criticalPair[pair_i][1]].p[0], 2) + pow(state.robot[i].p[1] - state.station[criticalPair[pair_i][1]].p[1], 2));
                                            willbuyTime = distr2b2 / (MAXV * mapparams[MAPKIND].SPEED);
                                        }
                                        //��û�У���ʱ��Ϊ�ܴ�
                                        willbuyTime = 10000;
                                    }
                                    if (((dis + s2sDist[criticalPair[pair_i][0]][criticalPair[pair_i][1]]) / (MAXV)) < willbuyTime) { continue; }
                                    //�������µ��� ����ʱԭ�ϸպÿճ��������������
                                    flag = false;
                                }
                                else
                                {
                                    //���ϸ��ȱ������ֱ������ȥ
                                    flag = false;
                                }
                            }
                        }
                    }
                }
                else
                {
                    //Ŀǰ�Ѳ���
                    for (int rob_i = 0; rob_i < 4; rob_i++)
                    {
                        if (cmd[rob_i].state == 0)
                        {
                            //�ų��������˵����˵�λ��
                            if (reachStation[1][rob_i][criticalPair[pair_i][0]] == false)
                            {
                                continue;
                            }
                            float dis = sqrt(pow(state.robot[rob_i].p[0] - state.station[criticalPair[pair_i][0]].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[criticalPair[pair_i][0]].p[1], 2));
                            //�ж�һ�¸ÿ��л������Ƿ����ü�����
                            //���������ģ����������
                            if (((dis + s2sDist[criticalPair[pair_i][0]][criticalPair[pair_i][1]]) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { continue; }
                            // Ҳ����˵���û����˿�������ʱ���������

                            //��֤�������ܷ���
                            if ((state.station[criticalPair[pair_i][1]].materials & (1 << state.station[criticalPair[pair_i][0]].type)) > 0) {
                                //���ϸ����������ʿ���û�����ܰѳ�Ʒ���ߣ��ø��ӿճ���
                                //����ʱ��ƷҲû�б��������������ߣ����������
                                //�����������������޽�Ҫ�ڴ˹����Ʒ��
                                float willbuyTime;
                                for (int i = 0; i < 4; i++)
                                {
                                    //�ų��Լ�
                                    if (i == rob_i) { continue; }
                                    //���У��������ﵽ����ʱ��
                                    if (cmd[i].takeStation == criticalPair[pair_i][1]) {
                                        float distr2b2 = sqrt(pow(state.robot[i].p[0] - state.station[criticalPair[pair_i][1]].p[0], 2) + pow(state.robot[i].p[1] - state.station[criticalPair[pair_i][1]].p[1], 2));
                                        willbuyTime = distr2b2 / (MAXV * mapparams[MAPKIND].SPEED);
                                    }
                                    //��û�У���ʱ��Ϊ�ܴ�
                                    willbuyTime = 10000;
                                }
                                if (((dis + s2sDist[criticalPair[pair_i][0]][criticalPair[pair_i][1]]) / (MAXV)) < willbuyTime) { continue; }
                                //�������µ��� ����ʱԭ�ϸպÿճ��������������
                                flag = false;
                            }
                            else
                            {
                                //���ϸ��ȱ������ֱ������ȥ
                                flag = false;
                            }
                        }
                    }
                }
                if (flag) { continue; } //��������������������

                //if ((state.station[criticalPair[pair_i][1]].materials & (1 << state.station[criticalPair[pair_i][0]].type)) > 0) { continue; } //���У��޷��չ�

                flag = false;
                for (int rob_i = 0; rob_i < 4; rob_i++)
                {
                    if (((cmd[rob_i].takeStation == criticalPair[pair_i][0]) && (cmd[rob_i].state == 1)) ||
                        ((cmd[rob_i].takeStation == criticalPair[pair_i][0]) && (cmd[rob_i].state == 3)) ||
                        ((cmd[rob_i].sellStation == criticalPair[pair_i][1]) && (state.station[cmd[rob_i].takeStation].type == state.station[criticalPair[pair_i][0]].type)))
                    {
                        flag = true;
                        break;
                    }
                }
                if (flag) { continue; } //�����ѱ�ռ��

                // ��ֹ���������ͬʱ����7/9��ס
                if ((state.station[criticalPair[pair_i][1]].type == 9) || (state.station[criticalPair[pair_i][1]].type == 7))
                {
                    int count = 0;
                    for (int rob_i = 0; rob_i < 4; rob_i++)
                    {
                        if (cmd[rob_i].sellStation < 0) { continue; }
                        if ((state.station[criticalPair[pair_i][1]].type == state.station[cmd[rob_i].sellStation].type))
                        {
                            count++;
                        }
                    }
                    if (cal_map_distance_d(state.station[criticalPair[pair_i][1]].p) > 3)
                    {
                        if (count >= 3)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if (count >= 1)
                        {
                            continue;
                        }
                    }
                }

                // ���в���Խ�࣬ԽӦ������
                float advance = 0;
                if (state.station[criticalPair[pair_i][1]].type <= 7)
                {
                    // �������в���
                    int haveRcv = 0;
                    for (int i = 1; i < 8; i++)
                    {
                        haveRcv += (state.station[criticalPair[pair_i][1]].materials >> i) % 2;
                    }
                    // �ݶ�Ϊ1.1���޲��ϣ�,1.2�����յ�1����,1.3�����յ�2����
                    advance += haveRcv + 1;
                }

                // û����������վ��Ӧ����������
                if (state.station[criticalPair[pair_i][1]].type <= 7)
                {
                    if ((state.station[criticalPair[pair_i][1]].finish == 0) && (state.station[criticalPair[pair_i][1]].inBusy <= 0))
                    {
                        advance += 5;
                    }
                }

                // �ۺ����ȶ�
                float priorty = criticalPairPriorty[pair_i] + advance;

                if (highest_priorty < priorty)
                {
                    highest_priorty = priorty;
                    sta_i = criticalPair[pair_i][0];
                    sta_j = criticalPair[pair_i][1];
                }
            }
        }

        if (sta_i != -1)
        {
            //Ѱ������ʵĿ��л�����
            float min_d = 100;
            int  best_rob = -1;
            for (int rob_i = 0; rob_i < 4; rob_i++)
            {
                if (cmd[rob_i].state == 0)
                {
                    //�ų��������˵����˵�λ��
                    if (reachStation[1][rob_i][sta_i] == false)
                    {
                        continue;
                    }

                    float d = sqrt(pow(state.robot[rob_i].p[0] - state.station[sta_i].p[0], 2) + pow(state.robot[rob_i].p[1] - state.station[sta_i].p[1], 2));

                    if (state.station[sta_i].finish == 1)
                    {
                        if (((d + s2sDist[sta_i][sta_j]) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { continue; }
                        if (d < min_d)
                        {
                            min_d = d;
                            best_rob = rob_i;
                        }
                    }
                    else
                    {
                        if (((d + s2sDist[sta_i][sta_j]) / (MAXV * mapparams[MAPKIND].SPEED)) * 50 > DEADLINE - state.timeStamp) { continue; }
                        if ((state.station[sta_i].inBusy < (d / MAXV)) && (state.station[sta_i].inBusy > 0))
                        {
                            if (d < min_d)
                            {
                                min_d = d;
                                best_rob = rob_i;
                            }
                        }
                    }
                }
            }

            // �ҵ��ˣ��ж����Ƿ��м�ֵ�л����µ�����
            if (best_rob != -1)
            {
                cmd[best_rob].takeStation = sta_i;
                cmd[best_rob].sellStation = sta_j;
                cmd[best_rob].state = 3;

                int rob_i = best_rob;
                if (cmdOld[rob_i].state != 0)
                {
                    // ������������Ĵ���̫�󣬾�����̫���ͻ�ԭ�Ͼ���
                    float dnew = sqrt(pow((state.station[cmd[rob_i].takeStation].p[0] - state.robot[rob_i].p[0]), 2) +
                        pow((state.station[cmd[rob_i].takeStation].p[0] - state.robot[rob_i].p[0]), 2));
                    float dold = sqrt(pow((state.station[cmdOld[rob_i].takeStation].p[0] - state.robot[rob_i].p[0]), 2) +
                        pow((state.station[cmdOld[rob_i].takeStation].p[0] - state.robot[rob_i].p[0]), 2));
                    if (dnew > dold * 3)
                    {
                        //�жϾ�ָ���Ƿ�����
                        if (test_old_cmd(rob_i))
                        {
                            //��ͷ����̫�󣬻�ԭָ��
                            cmd[rob_i].sellStation = cmdOld[rob_i].sellStation;
                            cmd[rob_i].takeStation = cmdOld[rob_i].takeStation;
                            cmd[rob_i].state = 1;
                            cmd[rob_i].efficient = cmdOld[rob_i].efficient;
                        }
                    }
                }
            }
            return;
        }
    }
    return;
}

//������������
int bad_seller_counting[4];
void bad_seller()
{
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        // �ͻ�̬��Ŀ��վ��ռ��
        if ((state.robot[rob_i].bring > 0) && (state.station[cmd[rob_i].sellStation].beenOccupy == true))
        {
            int maySellStation = -1;
            float newDistance = 0;
            //������ѯ�������۳�վ
            for (int pair_i = 0; pair_i < s2sPair.size(); pair_i++)
            {
                if (cmd[rob_i].takeStation != s2sPair[pair_i][0]) { continue; } // ������ͬһ����վ
                if ((state.station[s2sPair[pair_i][1]].materials & (1 << state.robot[rob_i].bring)) != 0) { continue; } // �۳�վ�ĸ��ӱ����ǿյ�
                if (state.station[s2sPair[pair_i][1]].beenOccupy == true) { continue; } //Ҳû�б�ռ��
                // ���Ǳ��˵�����վ
                bool tempFlag = false;
                for (int rob_k = 0; rob_k < 4; rob_k++)
                {
                    if (rob_i == rob_k) { continue; }
                    if ((cmd[rob_k].state == 2) && (cmd[rob_k].sellStation == s2sPair[pair_i][1]))
                    {
                        tempFlag = true;
                        break;
                    }
                }
                if (tempFlag) { continue; }

                if (maySellStation != -1)
                {
                    //�ͽ�����
                    if (newDistance > s2sDist[cmd[rob_i].sellStation][s2sPair[pair_i][1]])
                    {
                        maySellStation = s2sPair[pair_i][1];
                        newDistance = s2sDist[cmd[rob_i].sellStation][s2sPair[pair_i][1]];
                    }
                }
                else
                {
                    maySellStation = s2sPair[pair_i][1];
                    newDistance = s2sDist[cmd[rob_i].sellStation][s2sPair[pair_i][1]];
                }
            }
            if (maySellStation != -1)
            {
                cmd[rob_i].sellStation = maySellStation;
                bad_seller_counting[rob_i] = 0;
            }
            else
            {
                bad_seller_counting[rob_i]++;
                if (bad_seller_counting[rob_i] > 800)
                {
                    send_cmd_destroy(rob_i);
                    bad_seller_counting[rob_i] = 0;
                }
            }
        }
        else
        {
            bad_seller_counting[rob_i] = 0;
        }
    }
}
