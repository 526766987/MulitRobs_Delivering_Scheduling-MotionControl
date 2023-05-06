#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <random>
#include <numeric>
#include <functional>
#include "Global.h"
#include "Main.h"
#include "Readin.h"
#include "Map.h"
#include "Calculate.h"
#include "Schedule.h"
#include "Motion.h"
#include "Attack.h"
#include "Out.h"
#include "Path.h"
#include "Defend.h"

using namespace std;

/* ��ʼ����ǰ5��׼��ʱ�䣩 */
void init()
{
    //������ʼ��
    init_param();

    //��ͼ����ʶ�����ͼ���� // ��Map.cpp��
    wall_growth(true);
    chess_board_creat();
    outdebug("Map Create Done.");

    //վ����ԣ�����վ���Ȩ�ؼ��㣩 // ��Schedule.cpp��
    station_pair();
    outdebug("Station Pair Done.");

    //PID��ʼ�� // ��Motion.cpp��
    for (int rob_i = 0; rob_i < 4; rob_i++) {
        rotate_PID_init(rob_i);
    }

    //�����ؼ��ڵ���ж� // ��Attack.cpp��
    corner_enemy_station();
    pipe_enemy_station();
    corner_station();
    //�з�����վ���
    station_pair_attacker();
    //����������
    key_enemy_station_attacker();

    // ��ͼʶ�� // ��Map.cpp��
    map_identify();
    outdebug("mapKind", MAPKIND);

    //������ѡȡ
    if (MAPKIND == 1)
    {
        attacker_choose(0);
    }
    else if (MAPKIND == 2)
    {
        attacker_choose(1);
    }
    else if (MAPKIND == 3)
    {
        attacker_choose(1);
    }
    else if (MAPKIND == 4)
    {
        if (state.K == 0)
        {
            attacker_choose(4);
        }
        else
        {
            attacker_choose(0);
        }
    }
    outdebug("Attacker:", state.robot[0].attacker, state.robot[1].attacker, state.robot[2].attacker, state.robot[3].attacker);
}

// ������ڣ�������
int main() {

    readMapUntilOK();
    outdebug("Read Map DONE.");

    init();
    outdebug("INIT DONE.");

    puts("OK");
    fflush(stdout);

    int frameID;
    while (scanf("%d", &frameID) != EOF) {
        state.timeStamp = frameID;
        outdebug("--------��", state.timeStamp, "֡--------");
        readUntilOK();
        printf("%d\n", frameID);

        // �״�������Σ��վȷ�� // ��Defend.cpp��
        scouting();
        // ���Ȳ��ԣ�����״̬��ת��//��Schedule.cpp��
        state_wheel();
        // �������ԣ�������״̬��ת�� // ��Attacker.cpp��
        attack_wheel();
        // ʵʱ·���滮 // ��Path.cpp��
        path_search();
        // ���ϣ���ȫ���������ͻ���þ��ߣ� // ��Path.cpp��
        avoid();
        // �˶����� // 
        move();

        //��֡�������
        printf("OK\n");
        fflush(stdout);
    }
    outfile0.close();
    outfile1.close();
    return 0;
}
