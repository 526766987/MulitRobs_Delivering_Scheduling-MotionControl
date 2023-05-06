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
#include "Map.h"
#include "Calculate.h"
#include "Schedule.h"
#include "Motion.h"
#include "Attack.h"
#include "Out.h"
#include "Path.h"
#include "Defend.h"

using namespace std;

// ���ֲ�����ʼ����
void init_param() {
    //outdebug("��ͼ��ţ�", map_num);
    stationType[4].gain *= mapparams[MAPKIND].L2;
    stationType[5].gain *= mapparams[MAPKIND].L2;
    stationType[6].gain *= mapparams[MAPKIND].L2;
    stationType[7].gain *= mapparams[MAPKIND].L3;

    //�����ֵ
    for (int i = 1; i <= 7; i++) {
        stationType[i].productValue = cal_productValue(i);
    }
}

/* ��ȡ��ʼ����ͼ��Ϣ */
bool readMapUntilOK() {
    char line[1024];
    int row = 0;
    int robot_index = 0;
    int station_index = 0;
    int enemy_station_index = 0;

    //����ʶ��
    fgets(line, sizeof line, stdin);
    if (line[0] == 'B') {
        TEAM = 0;
    }
    else if (line[0] == 'R') {
        TEAM = 1;
    }

    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //��ʼ�����row�е�ͼ��Ϣ����state�ṹ�����
        for (int i = 0; i < 100; i++) {
            //ǽ��
            if (line[i] == '#') {
                wallMap[i][row] = true;
                continue;
            }
            //����Ϊ����
            if (TEAM == 0) {
                if (line[i] == 'A') {
                    state.robot[robot_index].p_ij_init[0] = i;
                    state.robot[robot_index].p_ij_init[1] = row;
                    state.robot[robot_index].p[0] = col2x(i);
                    state.robot[robot_index].p[1] = row2y(row);
                    outdebug(robot_index, "�Ż���������:", state.robot[robot_index].p[0], state.robot[robot_index].p[1]);
                    robot_index++;

                }
                else if (line[i] >= '1' && line[i] <= '9') {
                    state.station[station_index].p_ij[0] = i;
                    state.station[station_index].p_ij[1] = row;
                    state.station[station_index].p[0] = col2x(i);
                    state.station[station_index].p[1] = row2y(row);
                    state.station[station_index].type = line[i] - '0';
                    state.K += 1;
                    stationType[state.station[station_index].type].quantity += 1;
                    outdebug(station_index, "�Ź���̨����:", state.station[station_index].p[0], state.station[station_index].p[1], state.station[station_index].p_ij[0], state.station[station_index].p_ij[1]);
                    station_index++;
                }
                else if (line[i] >= 'a' && line[i] <= 'i') {
                    state.enemyStation[enemy_station_index].p_ij[0] = i;
                    state.enemyStation[enemy_station_index].p_ij[1] = row;
                    state.enemyStation[enemy_station_index].p[0] = col2x(i);
                    state.enemyStation[enemy_station_index].p[1] = row2y(row);
                    state.enemyStation[enemy_station_index].type = line[i] - 96;
                    state.eK += 1;
                    outdebug(enemy_station_index, "�ŵз�����̨����:", state.enemyStation[enemy_station_index].p[0], state.enemyStation[enemy_station_index].p[1], state.enemyStation[enemy_station_index].p_ij[0], state.enemyStation[enemy_station_index].p_ij[1]);
                    enemy_station_index++;
                }
                wallMap[i][row] = false;
            }
            //����Ϊ�췽
            else if (TEAM == 1) {
                if (line[i] == 'B') {
                    state.robot[robot_index].p_ij_init[0] = i;
                    state.robot[robot_index].p_ij_init[1] = row;
                    state.robot[robot_index].p[0] = col2x(i);
                    state.robot[robot_index].p[1] = row2y(row);
                    outdebug(robot_index, "�Ż���������:", state.robot[robot_index].p[0], state.robot[robot_index].p[1]);
                    robot_index++;

                }
                else if (line[i] >= 'a' && line[i] <= 'i') {
                    state.station[station_index].p_ij[0] = i;
                    state.station[station_index].p_ij[1] = row;
                    state.station[station_index].p[0] = col2x(i);
                    state.station[station_index].p[1] = row2y(row);
                    state.station[station_index].type = line[i] - 96;
                    state.K += 1;
                    stationType[state.station[station_index].type].quantity += 1;
                    outdebug(station_index, "�Ź���̨����:", state.station[station_index].p[0], state.station[station_index].p[1], state.station[station_index].p_ij[0], state.station[station_index].p_ij[1]);
                    station_index++;
                }
                else if (line[i] >= '1' && line[i] <= '9') {
                    state.enemyStation[enemy_station_index].p_ij[0] = i;
                    state.enemyStation[enemy_station_index].p_ij[1] = row;
                    state.enemyStation[enemy_station_index].p[0] = col2x(i);
                    state.enemyStation[enemy_station_index].p[1] = row2y(row);
                    state.enemyStation[enemy_station_index].type = line[i] - '0';
                    state.eK += 1;
                    outdebug(enemy_station_index, "�ŵз�����̨����:", state.enemyStation[enemy_station_index].p[0], state.enemyStation[enemy_station_index].p[1], state.enemyStation[enemy_station_index].p_ij[0], state.enemyStation[enemy_station_index].p_ij[1]);
                    enemy_station_index++;
                }
                wallMap[i][row] = false;
            }
        }
        row++;
    }
    return false;
}

/* ��ȡ֡��Ϣ */
bool readUntilOK() {
    char line[8848];
    int lineCount = 0;
    int K = state.K;
    //outdebug("��ʼ��ȡ֡����");
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        if (lineCount == 0)
        {
            //��һ������ 2 ����������ʾ֡��ţ��� 1 ��ʼ����������ǰ��Ǯ����
            sscanf(line, "%d", &state.cash);
            //outdebug("ǮǮ��", state.cash);
        }
        else if (lineCount == 1)
        {
            //�ڶ������� 1 ����������ʾ���Ϲ���̨������ K��K <= 50����
            int err = sscanf(line, "%d", &state.K);
            K = state.K;
            //outdebug(state.K);
        }
        else if (lineCount < 2 + K)
        {
            //���� K �����ݣ�ÿһ�б�ʾһ������̨���ֱ���������ʾ�����ݹ��ɣ����� 6 �����֣�
            int temp;
            int err = sscanf(line, "%d %f %f %d %d %d",
                &state.station[lineCount - 2].type,
                &state.station[lineCount - 2].p[0],
                &state.station[lineCount - 2].p[1],
                &state.station[lineCount - 2].inBusy,
                &state.station[lineCount - 2].materials,
                &temp);
            state.station[lineCount - 2].finish = (temp == 1);
        }
        else if (lineCount < 2 + K + 4)
        {
            //�������� 4 �����ݣ�ÿһ�б�ʾһ�������ˣ��ֱ������±������ʾ�����ݹ��ɣ�ÿ�� 10 �����֡�
            int err = sscanf(line, "%d %d %f %f %f %f %f %f %f %f",
                &state.robot[lineCount - 2 - K].atStation,
                &state.robot[lineCount - 2 - K].bring,
                &state.robot[lineCount - 2 - K].timeValue,
                &state.robot[lineCount - 2 - K].collisionValue,
                &state.robot[lineCount - 2 - K].angularSpeed,
                &state.robot[lineCount - 2 - K].speed[0],
                &state.robot[lineCount - 2 - K].speed[1],
                &state.robot[lineCount - 2 - K].direction,
                &state.robot[lineCount - 2 - K].p[0],
                &state.robot[lineCount - 2 - K].p[1]
            );
            //outdebug("��", lineCount - 2 - K, "�Ż����˸����Ĺ���̨��", state.robot[lineCount - 2 - K].atStation);
        }
        //�״Ｄ���ȡ
        else if (lineCount < 2 + K + 4 + 4)
        {
            //�� 4 �����ݣ�ÿһ�б�ʾһ�������˵�360�����⡣
            char* token = strtok(line, " ");
            int i = 0;
            int maxLaser_i = 0;
            int minLaser_i = 0;
            state.robot[lineCount - 2 - K - 4].sumLaser = 0;
            while (token != NULL) {
                //��ȡԭʼ��¼
                state.robot[lineCount - 2 - K - 4].lasers[i] = stof(token);
                //state.robot[lineCount - 2 - K - 4].lasers[i] = state.robot[lineCount - 2 - K - 4].lasers[i] < 5 ? state.robot[lineCount - 2 - K - 4].lasers[i] : 5;
                //���¼�ֵ����
                maxLaser_i = state.robot[lineCount - 2 - K - 4].lasers[i] > state.robot[lineCount - 2 - K - 4].lasers[maxLaser_i] ? i : maxLaser_i;
                minLaser_i = state.robot[lineCount - 2 - K - 4].lasers[i] < state.robot[lineCount - 2 - K - 4].lasers[minLaser_i] ? i : minLaser_i;
                // �󼤹����ͣ����ں����ռ��ԣ�ȣ�
                //state.robot[lineCount - 2 - K - 4].sumLaser += (state.robot[lineCount - 2 - K - 4].lasers[i] < robBlockDistance[map_num]) ? robBlockDistance[map_num] : state.robot[lineCount - 2 - K - 4].lasers[i];
                state.robot[lineCount - 2 - K - 4].sumLaser += state.robot[lineCount - 2 - K - 4].lasers[i];
                //��������ڵ�ͼԭ��ĽǶ� 
                float mapdirection = state.robot[lineCount - 2 - K - 4].direction + (static_cast<double>(i) / 360) * 2 * PI;
                if ((mapdirection < -PI) || (mapdirection > PI)) {
                    mapdirection = mapdirection - 2 * PI;
                }
                state.robot[lineCount - 2 - K - 4].lasersMapDirection[i] = mapdirection;
                //���㼤����㼰��Ӧ����
                float x = state.robot[lineCount - 2 - K - 4].p[0] + 1.001 * state.robot[lineCount - 2 - K - 4].lasers[i] * cos(mapdirection);
                float y = state.robot[lineCount - 2 - K - 4].p[1] + 1.001 * state.robot[lineCount - 2 - K - 4].lasers[i] * sin(mapdirection);
                //���õ��Ƿ�Ϊǽ��,����ǽ����Ϊ������
                if (!wallMap[x2col(x)][y2row(y)]) {
                    state.robot[lineCount - 2 - K - 4].lasersObject[i] = 1;
                }

                //���µ�����
                token = strtok(NULL, " ");
                i++;
                //outdebug("laser","[",i,"]=",state.robot[lineCount - 2 - K - 4].lasers[i]);
                //outfile << "laser[" << i << "] = " << state.robot[lineCount - 2 - K - 4].lasers[i] << endl;
            }
            state.robot[lineCount - 2 - K - 4].maxLaser_i = maxLaser_i;
            state.robot[lineCount - 2 - K - 4].minLaser_i = minLaser_i;
        }
        lineCount++;
    }
    return false;
}
