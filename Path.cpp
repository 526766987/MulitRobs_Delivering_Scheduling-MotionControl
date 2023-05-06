#include "Path.h"
#include <vector>
#include <cmath>
#include "Global.h"
#include "Calculate.h"
#include "Map.h"
#include "Attack.h"
#include "Schedule.h"
#include "Out.h"

using namespace std;

 //�ܵ����վ
bool reachStation[2][4][50] = { false };
// �ܵ���Ļ�����
bool reachRobot[2][4][4] = { false };
// ·��
vector<vector<int>> path(4, vector<int>(0));
// �����˵�ס�ĸ���
int robotBlockPosition[2][4][13];

int safePlace[4] = { 0 };
float safePlaceDis[4] = { 0 };

bool inLeave[4] = { false };
bool inSlow[4] = { false };

/* �ж�λ���Ƿ񱻻�����ռ�� */
bool have_block(int rob_i, int p_i, int p_j, bool mapSwitch)
{
    int mapSize = 0; //��ͼ�ߴ�
    if (mapSwitch == 0) { mapSize = 100; }
    else if (mapSwitch == 1) { mapSize = 101; }

    for (int i = 0; i < 4; i++)
    {
        // �Լ������赲�Լ�
        if (i == rob_i) { continue; }

        // �����жϵ��Ƿ񱻻����˵�ס
        for (int j = 0; j < 13; j++)
        {
            if (p_i + p_j * mapSize == robotBlockPosition[mapSwitch][i][j])
            {
                return true;
            }
        }
    }
    return false;
}

/* ͳ�ƻ����˵�ס�ĸ��� */
void robot_block_init()
{
    int mapSwitch = 0;
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        {
            int p_i = x2col(state.robot[rob_i].p[0]);
            int p_j = y2row(state.robot[rob_i].p[1]);
            robotBlockPosition[mapSwitch][rob_i][0] = p_i + 0 + (p_j + 0) * 100;
            robotBlockPosition[mapSwitch][rob_i][1] = p_i + 0 + (p_j + 1) * 100;
            robotBlockPosition[mapSwitch][rob_i][2] = p_i + 0 + (p_j - 1) * 100;
            robotBlockPosition[mapSwitch][rob_i][3] = p_i + 1 + (p_j + 0) * 100;
            robotBlockPosition[mapSwitch][rob_i][4] = p_i + 1 + (p_j + 1) * 100;
            robotBlockPosition[mapSwitch][rob_i][5] = p_i + 1 + (p_j - 1) * 100;
            robotBlockPosition[mapSwitch][rob_i][6] = p_i - 1 + (p_j + 0) * 100;
            robotBlockPosition[mapSwitch][rob_i][7] = p_i - 1 + (p_j + 1) * 100;
            robotBlockPosition[mapSwitch][rob_i][8] = p_i - 1 + (p_j - 1) * 100;
            robotBlockPosition[mapSwitch][rob_i][9] = p_i + 2 + (p_j - 0) * 100;
            robotBlockPosition[mapSwitch][rob_i][10] = p_i - 2 + (p_j + 0) * 100;
            robotBlockPosition[mapSwitch][rob_i][11] = p_i - 0 + (p_j + 2) * 100;
            robotBlockPosition[mapSwitch][rob_i][12] = p_i - 0 + (p_j - 2) * 100;
        }
    }
    mapSwitch = 1;
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        {
            int p_i = x2col_chess(state.robot[rob_i].p[0]);
            int p_j = y2row_chess(state.robot[rob_i].p[1]);
            robotBlockPosition[mapSwitch][rob_i][0] = p_i + 0 + (p_j + 0) * 101;
            robotBlockPosition[mapSwitch][rob_i][1] = p_i + 0 + (p_j + 1) * 101;
            robotBlockPosition[mapSwitch][rob_i][2] = p_i + 0 + (p_j - 1) * 101;
            robotBlockPosition[mapSwitch][rob_i][3] = p_i + 1 + (p_j + 0) * 101;
            robotBlockPosition[mapSwitch][rob_i][4] = p_i + 1 + (p_j + 1) * 101;
            robotBlockPosition[mapSwitch][rob_i][5] = p_i + 1 + (p_j - 1) * 101;
            robotBlockPosition[mapSwitch][rob_i][6] = p_i - 1 + (p_j + 0) * 101;
            robotBlockPosition[mapSwitch][rob_i][7] = p_i - 1 + (p_j + 1) * 101;
            robotBlockPosition[mapSwitch][rob_i][8] = p_i - 1 + (p_j - 1) * 101;
            robotBlockPosition[mapSwitch][rob_i][9] = -1;
            robotBlockPosition[mapSwitch][rob_i][10] = -1;
            robotBlockPosition[mapSwitch][rob_i][11] = -1;
            robotBlockPosition[mapSwitch][rob_i][12] = -1;
        }
    }
}

/* �ӻ�����rob_i�������� */
void flood(int rob_i, bool mapSwitch)
{
    // ���룺
    // rob_i ��ǰ������
    // mapSwitch == 0����չ��ͼexpandWall(100*100)��mapSwitch == 1�����̵�ͼchessBoard(101*101)

    int mapSize = 0; //��ͼ�ߴ�
    if (mapSwitch == 0) { mapSize = 100; }
    else if (mapSwitch == 1) { mapSize = 101; }

    vector<int> frontier; //����̽������
    vector<vector<bool>> haveDetect(mapSize, vector<bool>(mapSize, false)); // ��¼�Ƿ�̽���

    int start;
    if (mapSwitch == 0)
    {
        start = x2col(state.robot[rob_i].p[0]) + y2row(state.robot[rob_i].p[1]) * mapSize;
    }
    else
    {
        start = x2col_chess(state.robot[rob_i].p[0]) + y2row_chess(state.robot[rob_i].p[1]) * mapSize;
    }

    frontier.push_back(start);

    while (frontier.size() > 0)
    {
        //ȡһ���飬���û�б�̽�����ô̽��һ�飬���̽������Ǿ�ֱ��ɾ��������
        int current, p_i, p_j;
        current = frontier[0];
        p_i = current % mapSize;
        p_j = current / mapSize;
        if (haveDetect[p_i][p_j] == true)
        {
            //�Ӷ�����ɾ����ǰ��
            frontier.erase(frontier.begin());
            continue;
        }
        haveDetect[p_i][p_j] = true;
        // �����ǰ����ĳ�������˻���ĳ��վ����޸�reachStation��reachRobot
        for (int i = 0; i < 4; i++)
        {
            if ((state.robot[i].p_ij_init[0] == p_i) && (state.robot[i].p_ij_init[1] == p_j))
            {
                reachRobot[mapSwitch][rob_i][i] = true;
            }
        }
        for (int i = 0; i < state.K; i++)
        {
            if ((state.station[i].p_ij[0] == p_i) && (state.station[i].p_ij[1] == p_j))
            {
                reachStation[mapSwitch][rob_i][i] = true;
            }
        }
        //����Χ�ĵ�ӽ���
        vector<vector<int>> neighbors;
        neighbors.push_back({ p_i - 1,p_j });
        neighbors.push_back({ p_i + 1,p_j });
        neighbors.push_back({ p_i ,p_j + 1 });
        neighbors.push_back({ p_i ,p_j - 1 });
        for (int nei_i = 0; nei_i < 4; nei_i++)
        {
            if ((neighbors[nei_i][0] >= 0) && (neighbors[nei_i][0] < mapSize) && (neighbors[nei_i][1] >= 0) && (neighbors[nei_i][1] < mapSize))//�Ǳ߽�
            {
                if (haveDetect[neighbors[nei_i][0]][neighbors[nei_i][1]] == false)//δ̽���
                {
                    if (mapSwitch == 0)
                    {
                        if (!(expandWall[neighbors[nei_i][0]][neighbors[nei_i][1]])) //��ǽ
                        {
                            //������ھ�д�����
                            frontier.push_back(neighbors[nei_i][0] + neighbors[nei_i][1] * mapSize);
                        }
                    }
                    else if (mapSwitch == 1)
                    {
                        if (!(chessBoard[neighbors[nei_i][0]][neighbors[nei_i][1]])) //��ǽ
                        {
                            //������ھ�д�����
                            frontier.push_back(neighbors[nei_i][0] + neighbors[nei_i][1] * mapSize);
                        }
                    }
                }
            }
        }
        //�Ӷ�����ɾ����ǰ��
        frontier.erase(frontier.begin());
    }
}

/* ���������򷺺飨�����������˴������Ļ����˲���Ҫ�ٷ��飬ֱ�Ӹ��ƣ�*/
void all_flood(bool mapSwitch)
{
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        bool haveBeenReach = false;
        int rob_j = 0;
        for (rob_j = 0; rob_j < 4; rob_j++)
        {
            if (reachRobot[mapSwitch][rob_j][rob_i] == true)
            {
                haveBeenReach = true;
                break;
            }
        }
        if (haveBeenReach)
        {
            for (int sta_i = 0; sta_i < state.K; sta_i++)
            {
                reachStation[mapSwitch][rob_i][sta_i] = reachStation[mapSwitch][rob_j][sta_i];
            }
            for (int i = 0; i < 4; i++)
            {
                reachRobot[mapSwitch][rob_i][i] = reachRobot[mapSwitch][rob_j][i];
            }
        }
        else
        {
            flood(rob_i, mapSwitch);
        }
    }
}

/* �ж���������֮����û��ǽ */
bool through_wall(int start, int goal, float stepLength, bool mapSwitch, bool robotBlockSwitch, int rob_i, bool enemyBlockSwitch)
{
    int mapSize = 0; //��ͼ�ߴ�
    if (mapSwitch == 0) { mapSize = 100; }
    else if (mapSwitch == 1) { mapSize = 101; }

    int p_i_s, p_j_s, p_i_g, p_j_g;
    p_i_s = start % mapSize;
    p_j_s = floor(start / mapSize);
    p_i_g = goal % mapSize;
    p_j_g = floor(goal / mapSize);

    float p_x_s, p_y_s, p_x_g, p_y_g;
    if (mapSwitch == 0)
    {
        p_x_s = col2x(p_i_s);
        p_y_s = row2y(p_j_s);
        p_x_g = col2x(p_i_g);
        p_y_g = row2y(p_j_g);
    }
    else if (mapSwitch == 1)
    {
        p_x_s = col2x_chess(p_i_s);
        p_y_s = row2y_chess(p_j_s);
        p_x_g = col2x_chess(p_i_g);
        p_y_g = row2y_chess(p_j_g);
    }

    float dist = sqrt(pow(p_x_s - p_x_g, 2) + pow(p_y_s - p_y_g, 2));
    int step = ceil(dist / stepLength); //ÿ����stepLength��С��һ���������ٲ�

    for (int i = 0; i <= step; i++)
    {
        int p_i, p_j;
        if (mapSwitch == 0)
        {
            p_i = x2col(p_x_s + (p_x_g - p_x_s) * i / step);
            p_j = y2row(p_y_s + (p_y_g - p_y_s) * i / step);
        }
        else if (mapSwitch == 1)
        {
            p_i = x2col_chess(p_x_s + (p_x_g - p_x_s) * i / step);
            p_j = y2row_chess(p_y_s + (p_y_g - p_y_s) * i / step);
        }

        // ��ͼ�߽�
        if ((p_i <= 0) || (p_i >= mapSize-1) || (p_j <= 0) || (p_j >= mapSize-1))
        {
            return true;
        }
        // ǽ
        if (mapSwitch == 0)
        {
            if (expandWall[p_i][p_j] == true) { return true; }
        }
        else if (mapSwitch == 1)
        {
            if (chessBoard[p_i][p_j] == true) { return true; }
            //if ((p_i + 1 < mapSize) && (chessBoard[p_i + 1][p_j] == true)){ return true; }
            //if ((p_j + 1 < mapSize) && (chessBoard[p_i][p_j + 1] == true)) { return true; }
            //if ((p_i - 1 >= 0) && (chessBoard[p_i - 1][p_j] == true)) { return true; }
            //if ((p_j - 1 >= 0) && (chessBoard[p_i][p_j - 1] == true)) { return true; }
            int dangBlockCount = 0;
            if ((p_i + 1 < mapSize) && (chessBoard[p_i + 1][p_j] == true)){ dangBlockCount++; }
            if ((p_j + 1 < mapSize) && (chessBoard[p_i][p_j + 1] == true)) { dangBlockCount++; }
            if ((p_i - 1 >= 0) && (chessBoard[p_i - 1][p_j] == true)) { dangBlockCount++; }
            if ((p_j - 1 >= 0) && (chessBoard[p_i][p_j - 1] == true)) { dangBlockCount++; }
            if (dangBlockCount >= 2) { return true; }
        }
        // ������
        //if (robotBlockSwitch && (have_block(rob_i, p_i, p_j, mapSwitch) == true)) { return true; }
        if (robotBlockSwitch)
        {
            float p_x = p_x_s + (p_x_g - p_x_s) * i / step;
            float p_y = p_y_s + (p_y_g - p_y_s) * i / step;
            for (int rob_k = 0; rob_k < 4; rob_k++)
            {
                if (rob_i == rob_k) { continue; }
                else
                {
                    if (sqrt((pow(state.robot[rob_k].p[0] - p_x, 2) + pow(state.robot[rob_k].p[1] - p_y, 2))) <= (0.45 * 2 + (state.robot[rob_k].bring > 0) * 0.08 + (state.robot[rob_i].bring > 0) * 0.08))
                    {
                        return true;
                    }
                }
            }
        }
        if (enemyBlockSwitch)
        {
            float p_x = p_x_s + (p_x_g - p_x_s) * i / step;
            float p_y = p_y_s + (p_y_g - p_y_s) * i / step;
            for (int emy_i = 0; emy_i < state.enemyRobot.size(); emy_i++)
            {
                if (state.enemyRobot[emy_i].p[0] == -1) { continue; }
                else
                {
                    //if (sqrt((pow(state.enemyRobot[emy_i].p[0] - p_x, 2) + pow(state.enemyRobot[emy_i].p[1] - p_y, 2))) <= (0.45 * 2 + 0.08 + (state.robot[rob_i].bring > 0) * 0.08) + 0.5)
                    if (sqrt((pow(state.enemyRobot[emy_i].p[0] - p_x, 2) + pow(state.enemyRobot[emy_i].p[1] - p_y, 2))) <= (0.45 + (state.robot[rob_i].bring > 0) * 0.08) + 0.1)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/* ȥ��·���е������ */
void path_simple(int rob_i, bool mapSwitch, bool robotBlockSwitch, bool enemyBlockSwitch)
{
    if (path[rob_i].size() <= 2) { return; } //·��ֻ��������Ͳ���Ҫ����

    //���ڷ������������
    int direction; //�������� (1���ң�-1����+mapSize���£�-mapSize����)
    direction = path[rob_i][1] - path[rob_i][0];
    //���һֱ����ͬһ�������ߣ����м�����ɾ��
    for (int i = 1; i < path[rob_i].size() - 1; i++)
    {
        if (direction == path[rob_i][i + 1] - path[rob_i][i])
        {
            path[rob_i].erase(path[rob_i].begin() + i);
            i--;//��ǰ�㱻�����Ժ�for�����i��һ����ʱ��Ҫ�����������һ
        }
        else
        {
            direction = path[rob_i][i + 1] - path[rob_i][i];
        }
    }

    if (path[rob_i].size() <= 2) { return; } //�ٴ���֤

    //����ת�۵���������
    bool flag = true;
    for (int i = 0; i < path[rob_i].size() - 2; i++)
    {
        // i��i+2֮��û��ǽ������i+1
        while ((i + 2 < path[rob_i].size() - 2) && (!(through_wall(path[rob_i][i], path[rob_i][i + 2], 0.25, mapSwitch, robotBlockSwitch, rob_i, enemyBlockSwitch))))
        {
            path[rob_i].erase(path[rob_i].begin() + i + 1);
            flag = true;
        }
    }

    return;
}

/* A star �Զ�Ѱ· */
bool a_star_search(int start, int goal, bool mapSwitch, bool robotBlockSwitch, int rob_i, bool enemyBlockSwitch)
{
    // ���룺
    // start ��㣬i+j*mapSize
    // goal �յ㣬i+j*mapSize
    // mapSwitch == 0����չ��ͼexpandWall(100*100)��mapSwitch == 1�����̵�ͼchessBoard(101*101)
    // robotBlockSwitch �Ƿ��ǻ����˵��谭
    // rob_i ��ǰ�����ˣ��ڳ�ʼ��ʱ�������ĳ�������ˣ��������0��λ�ã�

    // �����
    // path ·��

    path[rob_i].clear(); //·��

    int mapSize = 0; //��ͼ�ߴ�
    if (mapSwitch == 0) { mapSize = 100; }
    else if (mapSwitch == 1) { mapSize = 101; }

    int p_i, p_j; //��ǰ̽��λ��
    vector<int> frontier; //����̽������
    vector<float> frontierPriority; //�������ȶ�
    vector<vector<int>> cameFrom(mapSize, vector<int>(mapSize, -1)); //·���ϵ�ǰһ�񣨳�ʼ��Ϊ-1��
    vector<vector<float>> costSoFar(mapSize, vector<float>(mapSize)); //����ĵ�ǰ����
    vector<vector<bool>> haveDetect(mapSize, vector<bool>(mapSize)); // ��¼�Ƿ�̽���

    // ���
    p_i = start % mapSize;
    p_j = start / mapSize;
    frontier.push_back(start);
    frontierPriority.push_back(0);
    cameFrom[p_i][p_j] = start;
    costSoFar[p_i][p_j] = 0;
    haveDetect[p_i][p_j] = true;

    while (frontier.size() > 0)
    {
        int current_i, current;
        float currentCost;
        float minPriority;

        //Ѱ�Ҵ�����͵ķ���
        current_i = 0;
        current = frontier[current_i];
        p_i = current % mapSize;
        p_j = current / mapSize;
        minPriority = frontierPriority[current_i];
        currentCost = costSoFar[p_i][p_j];
        for (int i = 1; i < frontier.size(); i++)
        {
            if (minPriority > frontierPriority[i])
            {
                current_i = i;
                current = frontier[current_i];
                p_i = current % mapSize;
                p_j = current / mapSize;
                minPriority = frontierPriority[current_i];
                currentCost = costSoFar[p_i][p_j];
            }
        }

        //��⵱ǰ�����Ƿ����յ��
        if (current == goal) { break; }

        //Ѱ�ҵ�ǰ��������ڿ�
        vector<vector<int>> neighbors;
        // ����̽��
        neighbors.push_back({ p_i + 1,p_j });
        neighbors.push_back({ p_i ,p_j + 1 });
        neighbors.push_back({ p_i - 1,p_j });
        neighbors.push_back({ p_i ,p_j - 1 });
        // ����̽�⸽��
        if (mapSwitch == 0)
        {
            neighbors.push_back({ p_i - 1,p_j -1 });
            neighbors.push_back({ p_i + 1,p_j -1 });
            neighbors.push_back({ p_i - 1,p_j + 1 });
            neighbors.push_back({ p_i + 1,p_j + 1 });
        }
        
        for (int nei_i = 0; nei_i < neighbors.size(); nei_i++)
        {
            if ((neighbors[nei_i][0] >= 0) && (neighbors[nei_i][0] < mapSize) && (neighbors[nei_i][1] >= 0) && (neighbors[nei_i][1] < mapSize))
            {
                // ǽ
                if (mapSwitch == 0)
                {
                    if (expandWall[neighbors[nei_i][0]][neighbors[nei_i][1]]) { continue; }
                }
                else if (mapSwitch == 1)
                {
                    if (chessBoard[neighbors[nei_i][0]][neighbors[nei_i][1]]) { continue; }
                }
                // ������
                //if (robotBlockSwitch && (have_block(rob_i, neighbors[nei_i][0], neighbors[nei_i][1], mapSwitch)))
                //{
                //    continue;
                //}
                if (robotBlockSwitch)
                {
                    float p_x, p_y;
                    if (mapSwitch == 0)
                    {
                        p_x = col2x(neighbors[nei_i][0]);
                        p_y = row2y(neighbors[nei_i][1]);
                    }
                    else
                    {
                        p_x = col2x_chess(neighbors[nei_i][0]);
                        p_y = row2y_chess(neighbors[nei_i][1]);
                    }
                    bool tempFlag = false;
                    for (int rob_k = 0; rob_k < 4; rob_k++)
                    {
                        if (rob_i == rob_k) { continue; }
                        else
                        {
                            if (sqrt((pow(state.robot[rob_k].p[0] - p_x, 2) + pow(state.robot[rob_k].p[1] - p_y, 2))) <= (0.45 * 2 + (state.robot[rob_k].bring > 0) * 0.08 + (state.robot[rob_i].bring > 0) * 0.08) + 0.5)
                            {
                                tempFlag = true;
                            }
                        }
                    }
                    if (tempFlag) { continue; }
                }
                if (enemyBlockSwitch)
                {
                    float p_x, p_y;
                    if (mapSwitch == 0)
                    {
                        p_x = col2x(neighbors[nei_i][0]);
                        p_y = row2y(neighbors[nei_i][1]);
                    }
                    else
                    {
                        p_x = col2x_chess(neighbors[nei_i][0]);
                        p_y = row2y_chess(neighbors[nei_i][1]);
                    }
                    bool tempFlag = false;
                    for (int emy_i = 0; emy_i < state.enemyRobot.size(); emy_i++)
                    {
                        if (state.enemyRobot[emy_i].p[0] == -1) { continue; }
                        else
                        {
                            //if (sqrt((pow(state.enemyRobot[emy_i].p[0] - p_x, 2) + pow(state.enemyRobot[emy_i].p[1] - p_y, 2))) <= (0.45 * 2 + 0.08 + (state.robot[rob_i].bring > 0) * 0.08) + 0.5)
                            if (sqrt((pow(state.enemyRobot[emy_i].p[0] - p_x, 2) + pow(state.enemyRobot[emy_i].p[1] - p_y, 2))) <= (0.45 + (state.robot[rob_i].bring > 0) * 0.08) + 0.1)
                            {
                                tempFlag = true;
                            }
                        }
                    }
                    if (tempFlag) { continue; }
                }
                // �ǵ�ͼ�߽硢��ǽ���ǻ����˵ĸ���
                float neighborCost = currentCost + 1 + 0.414 * (nei_i >= 4); // �ȵ�ǰ��������һ��
                if ((haveDetect[neighbors[nei_i][0]][neighbors[nei_i][1]] == false) ||
                    (neighborCost < costSoFar[neighbors[nei_i][0]][neighbors[nei_i][1]])) // δ̽���Ŀ飬��Ϊ��̽���Ŀ��ҵ��˸�����·
                {
                    // �ѵ�ǰ���ۺ�·��д���
                    costSoFar[neighbors[nei_i][0]][neighbors[nei_i][1]] = neighborCost;
                    cameFrom[neighbors[nei_i][0]][neighbors[nei_i][1]] = current;
                    haveDetect[neighbors[nei_i][0]][neighbors[nei_i][1]] = true;
                    // ��ǰ+Ԥ�ڣ����յ�������پ��룩
                    float priority = neighborCost + manhattan_distance(neighbors[nei_i][0], neighbors[nei_i][1], goal % mapSize, goal / mapSize);
                    // ������ھӵ�д�����
                    frontier.push_back(neighbors[nei_i][0] + neighbors[nei_i][1] * mapSize);
                    frontierPriority.push_back(priority);
                }
            }
        }
        //�Ӷ�����ɾ����ǰ��
        frontier.erase(frontier.begin() + current_i);
        frontierPriority.erase(frontierPriority.begin() + current_i);
    }

    // ���н�����û��̽�⵽Ŀ�꣨��ֹ��շ��䣩
    if (haveDetect[goal % mapSize][goal / mapSize] == true)
    {
        // ��������·��
        int current = goal;
        path[rob_i].push_back(current);
        while (current != start)
        {
            current = cameFrom[current % mapSize][current / mapSize];
            path[rob_i].insert(path[rob_i].begin(), current);
        }
        return true;
    }

    return false;
}

/* ʵʱ·���滮 */
void real_time_path_search(int rob_i, bool robotBlockSwitch)
{
    // ·��ȷ��
    if (state.robot[rob_i].attacker == true) // ������
    {
        if (attackerState[rob_i] == 0)
        {
            path[rob_i].clear();
            int p_i = x2col_chess(state.robot[rob_i].p[0]);
            int p_j = y2row_chess(state.robot[rob_i].p[1]);
            int start = p_i + p_j * 101;
            p_i = x2col_chess(state.enemyStation[currentAttackEnemyStation[rob_i]].p[0]) - 1;
            p_j = y2row_chess(state.enemyStation[currentAttackEnemyStation[rob_i]].p[1]) - 1;
            int goal = p_i + p_j * 101;
            a_star_search(start, goal, 1, robotBlockSwitch, rob_i, false);
            path_simple(rob_i, 1, robotBlockSwitch, false);
        }
        else
        {
            path[rob_i].clear();
        }
        attacker_follow(rob_i);
        
        if (MAPKIND == 2)
        {
            crazy_attacker_follow(rob_i);
        }
    }
    else if (cmd[rob_i].takeStation == -1) // �˻���
    {
        path[rob_i].clear();
    }
    else if ((cmd[rob_i].state == 1)) {
        // ȡ����chessBoard
        path[rob_i].clear();
        int p_i = x2col_chess(state.robot[rob_i].p[0]);
        int p_j = y2row_chess(state.robot[rob_i].p[1]);
        int start = p_i + p_j * 101;
        int goal = state.station[cmd[rob_i].takeStation].p_ij[0] + state.station[cmd[rob_i].takeStation].p_ij[1] * 101;
        //if (TEAM == 0)
        //{
        //    a_star_search(start, goal, 1, robotBlockSwitch, rob_i, false);
        //    path_simple(rob_i, 1, robotBlockSwitch, false);
        //}
        //else
        {
            if (a_star_search(start, goal, 1, robotBlockSwitch, rob_i, true) == false)
            {
                a_star_search(start, goal, 1, robotBlockSwitch, rob_i, false);
                path_simple(rob_i, 1, robotBlockSwitch, false);
            }
            else
            {
                path_simple(rob_i, 1, robotBlockSwitch, true);
            }
        }
    }
    else if (cmd[rob_i].state == 2) {
        // �ͻ���expandWall
        path[rob_i].clear();
        int p_i = x2col(state.robot[rob_i].p[0]);
        int p_j = y2row(state.robot[rob_i].p[1]);
        int start = p_i + p_j * 100;
        int goal = state.station[cmd[rob_i].sellStation].p_ij[0] + state.station[cmd[rob_i].sellStation].p_ij[1] * 100;
        //if (TEAM == 0)
        //{
        //    a_star_search(start, goal, 0, robotBlockSwitch, rob_i, false);
        //    path_simple(rob_i, 0, robotBlockSwitch, false);
        //}
        //else
        {
            if (a_star_search(start, goal, 0, robotBlockSwitch, rob_i, true) == false)
            {
                a_star_search(start, goal, 0, robotBlockSwitch, rob_i, false);
                path_simple(rob_i, 0, robotBlockSwitch, false);
            }
            else
            {
                path_simple(rob_i, 0, robotBlockSwitch, true);
            }
        }
    }
}

/* ʵʱ·���滮 */
void real_time_path_search_for_all(bool robotBlockSwitch)
{
    for (int rob_i = 0; rob_i < 4; rob_i++) {
        real_time_path_search(rob_i, robotBlockSwitch);
    }
}

// ��֤·���Ƿ�ĳ�������赲
bool robotBlockByRobot[4][4] = {false};
float robotBlockByRobotDist[4][4] = {0};
int nearestRobot[4] = { -1, -1, -1, -1 };
float nearestRobotDist[4] = { 0 };
bool pathMap[4][100][100] = {false};
bool pathChess[4][101][101] = {false};

void path_block_by_robot(int rob_i, bool mapSwitch, float stepLength)
{
    if (path[rob_i].size() <= 1) { return; }

    int mapSize = 0; //��ͼ�ߴ�
    if (mapSwitch == 0) { mapSize = 100; }
    else if (mapSwitch == 1) { mapSize = 101; }

    for (int rob_k = 0; rob_k < 4; rob_k++)
    {
        if (rob_i == rob_k) { continue; }
        else
        {
            if (sqrt((pow(state.robot[rob_k].p[0] - state.robot[rob_i].p[0], 2) + pow(state.robot[rob_k].p[1] - state.robot[rob_i].p[1], 2))) <= (0.45 * 2 + (state.robot[rob_k].bring > 0) * 0.08 + (state.robot[rob_i].bring > 0) * 0.08) + 0.05)
            {
                if ((path[rob_k].size() >= 2) && (path[rob_i].size() >= 2))
                {
                    if (cmd[rob_k].state == 1)
                    {
                        float direct_i = atan2(row2y(floor(path[rob_i][1] / 100)) - state.robot[rob_k].p[1], col2x(path[rob_i][0] % 100) - state.robot[rob_i].p[0]);
                        float direct_k = atan2(row2y(floor(path[rob_k][1] / 100)) - state.robot[rob_k].p[1], col2x(path[rob_k][0] % 100) - state.robot[rob_k].p[0]);
                        if ((abs(direct_k - direct_i) < 0.1 * PI) || (abs(direct_k - direct_i) > 1.9 * PI))
                        {
                            //ͬ��
                            continue;
                        }
                    }
                }
                robotBlockByRobot[rob_i][rob_k] = true;
                nearestRobot[rob_i] = (nearestRobot[rob_i] == -1) ? rob_k : nearestRobot[rob_i];
                robotBlockByRobotDist[rob_i][rob_k] = (robotBlockByRobotDist[rob_i][rob_k] == 0) ? sqrt((pow(state.robot[rob_k].p[0] - state.robot[rob_i].p[0], 2) + pow(state.robot[rob_k].p[1] - state.robot[rob_i].p[1], 2))) : robotBlockByRobotDist[rob_i][rob_k];
                nearestRobotDist[rob_i] = (nearestRobotDist[rob_i] == 0) ? sqrt((pow(state.robot[rob_k].p[0] - state.robot[rob_i].p[0], 2) + pow(state.robot[rob_k].p[1] - state.robot[rob_i].p[1], 2))) : nearestRobotDist[rob_i];
            }
        }
    }


    float distance = 0;

    for (int i = 0; i < path[rob_i].size() - 1; i++)
    {
        int p_i_s, p_j_s, p_i_g, p_j_g;
        float p_x_s, p_y_s, p_x_g, p_y_g;
        p_i_s = path[rob_i][i] % mapSize;
        p_j_s = floor(path[rob_i][i] / mapSize);
        p_i_g = path[rob_i][i+1] % mapSize;
        p_j_g = floor(path[rob_i][i+1] / mapSize);
        if (mapSwitch == 0)
        {
            p_x_s = col2x(p_i_s);
            p_y_s = row2y(p_j_s);
            p_x_g = col2x(p_i_g);
            p_y_g = row2y(p_j_g);
        }
        else if (mapSwitch == 1)
        {
            p_x_s = col2x_chess(p_i_s);
            p_y_s = row2y_chess(p_j_s);
            p_x_g = col2x_chess(p_i_g);
            p_y_g = row2y_chess(p_j_g);
        }

        float dist = sqrt(pow(p_x_s - p_x_g, 2) + pow(p_y_s - p_y_g, 2));
        int step = ceil(dist / stepLength); //ÿ����stepLength��С��һ���������ٲ�
        float direct_i = atan2(p_y_g - p_y_s, p_x_g - p_x_s);
        for (int s = 0; s <= step; s++)
        {
            float p_x = p_x_s + (p_x_g - p_x_s) * s / step;
            float p_y = p_y_s + (p_y_g - p_y_s) * s / step;
            for (int rob_k = 0; rob_k < 4; rob_k++)
            {
                if (rob_i == rob_k) { continue; }
                else
                {
                    if (sqrt((pow(state.robot[rob_k].p[0] - p_x, 2) + pow(state.robot[rob_k].p[1] - p_y, 2))) <= (0.45 * 2 + (state.robot[rob_k].bring > 0) * 0.08 + (state.robot[rob_i].bring > 0) * 0.08) + 0.05)
                    {
                        if (path[rob_k].size() >= 2)
                        {
                            if (cmd[rob_k].state == 1)
                            {
                                float direct_k = atan2(row2y(floor(path[rob_k][1]/100))-state.robot[rob_k].p[1], col2x(path[rob_k][0] % 100) - state.robot[rob_k].p[0]);
                                if ((abs(direct_k - direct_i) < 0.1 * PI) || (abs(direct_k - direct_i) > 1.9 * PI))
                                {
                                    //ͬ��
                                    continue;
                                }
                            }
                        }
                        robotBlockByRobot[rob_i][rob_k] = true;
                        nearestRobot[rob_i] = (nearestRobot[rob_i] == -1) ? rob_k : nearestRobot[rob_i];
                        robotBlockByRobotDist[rob_i][rob_k] = (robotBlockByRobotDist[rob_i][rob_k] == 0) ? distance + dist * s / step : robotBlockByRobotDist[rob_i][rob_k];
                        nearestRobotDist[rob_i] = (nearestRobotDist[rob_i] == 0) ? distance + dist * s / step : nearestRobotDist[rob_i];
                    }
                }
            }

            //��¼��ͼ��Ϣ;
            pathMap[rob_i][x2col(p_x_s + (p_x_g - p_x_s) * s / step)][y2row(p_y_s + (p_y_g - p_y_s) * s / step)] = true;
            pathChess[rob_i][x2col_chess(p_x_s + (p_x_g - p_x_s) * s / step)][y2row_chess(p_y_s + (p_y_g - p_y_s) * s / step)] = true;
        }
        distance += dist; //��¼����
    }
    return;
}

// Ѱ��һ���ͽ���ȫ��
int safe_place(int rob_i, bool mapSwitch)
{
    // ���룺
    // rob_i ��ǰ������
    // mapSwitch == 0����չ��ͼexpandWall(100*100)��mapSwitch == 1�����̵�ͼchessBoard(101*101)

    int mapSize = 0; //��ͼ�ߴ�
    if (mapSwitch == 0) { mapSize = 100; }
    else if (mapSwitch == 1) { mapSize = 101; }

    vector<int> frontier; //����̽������
    vector<vector<bool>> haveDetect(mapSize, vector<bool>(mapSize, false)); // ��¼�Ƿ�̽���

    int start;
    if (mapSwitch == 0)
    {
        start = x2col(state.robot[rob_i].p[0]) + y2row(state.robot[rob_i].p[1]) * mapSize;
    }
    else
    {
        start = x2col_chess(state.robot[rob_i].p[0]) + y2row_chess(state.robot[rob_i].p[1]) * mapSize;
    }

    frontier.push_back(start);

    while (frontier.size() > 0)
    {
        //ȡһ���飬���û�б�̽�����ô̽��һ�飬���̽������Ǿ�ֱ��ɾ��������
        int current, p_i, p_j;
        current = frontier[0];
        p_i = current % mapSize;
        p_j = floor(current / mapSize);
        
        bool flag = false;
        for (int i = 0; i < 4; i++)
        {
            if (rob_i == i)
            {
                continue;
            }
            if (mapSwitch == 0)
            {
                flag = flag ||                                                     pathMap[i][p_i + 0][p_j + 0];
                flag = flag || ((p_j + 1 < mapSize)                             && pathMap[i][p_i + 0][p_j + 1]);
                flag = flag || ((p_j - 1 >= 0)                                  && pathMap[i][p_i + 0][p_j - 1]);
                flag = flag || ((p_i - 1 >= 0)                                  && pathMap[i][p_i - 1][p_j + 0]);
                flag = flag || ((p_i - 1 >= 0) && (p_j + 1 < mapSize)           && pathMap[i][p_i - 1][p_j + 1]);
                flag = flag || ((p_i - 1 >= 0) && (p_j - 1 >= 0)                && pathMap[i][p_i - 1][p_j - 1]);
                flag = flag || ((p_i + 1 < mapSize)                             && pathMap[i][p_i + 1][p_j + 0]);
                flag = flag || ((p_i + 1 < mapSize) && (p_j + 1 < mapSize)      && pathMap[i][p_i + 1][p_j + 1]);
                flag = flag || ((p_i + 1 < mapSize) && (p_j - 1 >= 0)           && pathMap[i][p_i + 1][p_j - 1]);
                flag = flag || ((p_j + 2 < mapSize)                             && pathMap[i][p_i + 0][p_j + 2]);
                flag = flag || ((p_j - 2 >= 0)                                  && pathMap[i][p_i + 0][p_j - 2]);
                flag = flag || ((p_i - 2 >= 0)                                  && pathMap[i][p_i - 2][p_j + 0]);
                flag = flag || ((p_i + 2 < mapSize)                             && pathMap[i][p_i + 2][p_j + 0]);
            }
            else
            {
                flag = flag ||                                                     pathChess[i][p_i + 0][p_j + 0];
                flag = flag || ((p_j + 1 < mapSize)                             && pathChess[i][p_i + 0][p_j + 1]);
                flag = flag || ((p_j - 1 >= 0)                                  && pathChess[i][p_i + 0][p_j - 1]);
                flag = flag || ((p_i - 1 >= 0)                                  && pathChess[i][p_i - 1][p_j + 0]);
                flag = flag || ((p_i - 1 >= 0)                                  && (p_j + 1 < mapSize) && pathChess[i][p_i - 1][p_j + 1]);
                flag = flag || ((p_i - 1 >= 0)                                  && (p_j - 1 >= 0) && pathChess[i][p_i - 1][p_j - 1]);
                flag = flag || ((p_i + 1 < mapSize)                             && pathChess[i][p_i + 1][p_j + 0]);
                flag = flag || ((p_i + 1 < mapSize)                             && (p_j + 1 < mapSize) && pathChess[i][p_i + 1][p_j + 1]);
                flag = flag || ((p_i + 1 < mapSize)                             && (p_j - 1 >= 0) && pathChess[i][p_i + 1][p_j - 1]);
                flag = flag || ((p_j + 2 < mapSize)                             && pathChess[i][p_i + 0][p_j + 2]);
                flag = flag || ((p_j - 2 >= 0)                                  && pathChess[i][p_i + 0][p_j - 2]);
                flag = flag || ((p_i - 2 >= 0)                                  && pathChess[i][p_i - 2][p_j + 0]);
                flag = flag || ((p_i + 2 < mapSize)                             && pathChess[i][p_i + 2][p_j + 0]);
            }
        }
        if (flag == false) { return current; }
        
        if (haveDetect[p_i][p_j] == true)
        {
            //�Ӷ�����ɾ����ǰ��
            frontier.erase(frontier.begin());
            continue;
        }
        haveDetect[p_i][p_j] = true;

        //����Χ�ĵ�ӽ���
        vector<vector<int>> neighbors;
        neighbors.push_back({ p_i - 1,p_j });
        neighbors.push_back({ p_i + 1,p_j });
        neighbors.push_back({ p_i ,p_j + 1 });
        neighbors.push_back({ p_i ,p_j - 1 });
        for (int nei_i = 0; nei_i < 4; nei_i++)
        {
            if ((neighbors[nei_i][0] >= 0) && (neighbors[nei_i][0] < mapSize) && (neighbors[nei_i][1] >= 0) && (neighbors[nei_i][1] < mapSize))//�Ǳ߽�
            {
                if (haveDetect[neighbors[nei_i][0]][neighbors[nei_i][1]] == false)//δ̽���
                {
                    //if (have_block(rob_i, neighbors[nei_i][0], neighbors[nei_i][1], mapSwitch)) { continue; }
                    float p_x, p_y;
                    if (mapSwitch == 0)
                    {
                        p_x = col2x(neighbors[nei_i][0]);
                        p_y = row2y(neighbors[nei_i][1]);
                    }
                    else
                    {
                        p_x = col2x_chess(neighbors[nei_i][0]);
                        p_y = row2y_chess(neighbors[nei_i][1]);
                    }
                    // �ھӽڵ��Ƿ�����������ռ��
                    bool tempFlag = false;
                    for (int rob_k = 0; rob_k < 4; rob_k++)
                    {
                        if (rob_i == rob_k) { continue; }
                        else
                        {
                            if (sqrt((pow(state.robot[rob_k].p[0] - p_x, 2) + pow(state.robot[rob_k].p[1] - p_y, 2))) <= (0.45 * 2 + (state.robot[rob_k].bring > 0) * 0.08 + (state.robot[rob_i].bring > 0) * 0.08))
                            {
                                tempFlag = true;
                            }
                        }
                    }
                    if (tempFlag && (start != current)) { continue; }
                    for (int emy_i = 0; emy_i < state.enemyRobot.size(); emy_i++)
                    {
                        //if (sqrt((pow(state.enemyRobot[emy_i].p[0] - p_x, 2) + pow(state.enemyRobot[emy_i].p[1] - p_y, 2))) <= (0.45 * 2 + 0.08 + (state.robot[rob_i].bring > 0) * 0.08))
                        if (sqrt((pow(state.enemyRobot[emy_i].p[0] - p_x, 2) + pow(state.enemyRobot[emy_i].p[1] - p_y, 2))) <= (0.45 + (state.robot[rob_i].bring > 0) * 0.08))
                        {
                            tempFlag = true;
                        }
                    }
                    if (tempFlag) { continue; }
                    // �ھӽڵ��Ƿ���ǽ
                    if (mapSwitch == 0)
                    {
                        if (!(expandWall[neighbors[nei_i][0]][neighbors[nei_i][1]])) //��ǽ
                        {
                            //������ھ�д�����
                            frontier.push_back(neighbors[nei_i][0] + neighbors[nei_i][1] * mapSize);
                        }
                    }
                    else if (mapSwitch == 1)
                    {
                        if (!(chessBoard[neighbors[nei_i][0]][neighbors[nei_i][1]])) //��ǽ
                        {
                            //������ھ�д�����
                            frontier.push_back(neighbors[nei_i][0] + neighbors[nei_i][1] * mapSize);
                        }
                    }
                }
            }
        }
        //�Ӷ�����ɾ����ǰ��
        frontier.erase(frontier.begin());
    }

    return -1;
}


void init_avoid()
{
    //��֤�Ƿ�ĳ�������赲
    //������Ϣ
    for (int rob_k = 0; rob_k < 4; rob_k++)
    {
        for (int rob_j = 0; rob_j < 4; rob_j++)
        {
            robotBlockByRobot[rob_k][rob_j] = false;
            robotBlockByRobotDist[rob_k][rob_j] = 0;
        }
        nearestRobot[rob_k] = -1;
        nearestRobotDist[rob_k] = 0;
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                pathMap[rob_k][i][j] = false;
                pathChess[rob_k][i][j] = false;
            }
            pathChess[rob_k][100][i] = false;
            pathChess[rob_k][i][100] = false;
        }
        pathChess[rob_k][100][100] = false;
    }

}

void path_block_by_robot_all()
{
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 1) || (cmd[rob_i].state == -1))
        {
            //ȡ����chessBoard
            path_block_by_robot(rob_i, 1, 0.25);
        }
        else
        {
            //�ͻ���extandWall
            path_block_by_robot(rob_i, 0, 0.25);
        }
    }
}

void safe_place_all()
{
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 1) || (cmd[rob_i].state == -1))
        {
            //ȡ����chessBoard
            safePlace[rob_i] = safe_place(rob_i, 1);
            if (safePlace[rob_i] != -1)
            {
                safePlaceDis[rob_i] = pow(col2x_chess(safePlace[rob_i] % 101) - state.robot[rob_i].p[0], 2) +
                    pow(row2y_chess(floor(safePlace[rob_i] / 101)) - state.robot[rob_i].p[1], 2);
            }
        }
        else
        {
            //�ͻ���extandWall
            safePlace[rob_i] = safe_place(rob_i, 0);
            if (safePlace[rob_i] != -1)
            {
                safePlaceDis[rob_i] = pow(col2x(safePlace[rob_i] % 100) - state.robot[rob_i].p[0], 2) +
                    pow(row2y(floor(safePlace[rob_i] / 100)) - state.robot[rob_i].p[1], 2);
            }
        }
    }
}


void avoid_choose()
{
    for (int rob_i = 0; rob_i < 3; rob_i++)
    {
        for (int rob_j = rob_i + 1; rob_j < 4; rob_j++)
        {
            bool blockFlag1 = false;
            bool blockFlag2 = false;
            if (robotBlockByRobot[rob_i][rob_j] == true)
            //if ((robotBlockByRobot[rob_i][rob_j] == true) && (robotBlockByRobotDist[rob_i][rob_j] < robBlockDistance[map_num]))
            {
                // rob_i��·����rob_j
                blockFlag1 = true;
            }
            if (robotBlockByRobot[rob_j][rob_i] == true)
            //if ((robotBlockByRobot[rob_j][rob_i] == true) && (robotBlockByRobotDist[rob_j][rob_i] < robBlockDistance[map_num]))
            {
                // rob_j��·����rob_i
                blockFlag2 = true;
            }
            if ((blockFlag1 == true) && (blockFlag2 == true))
            {
                if ((safePlace[rob_i] == -1) && (safePlace[rob_j] != -1))
                {
                    inLeave[rob_j] = true;
                    inSlow[rob_i] = true;
                }
                else if ((safePlace[rob_i] != -1) && (safePlace[rob_j] == -1))
                {
                    inLeave[rob_i] = true;
                    inSlow[rob_j] = true;
                }
                else if ((safePlace[rob_i] == -1) && (safePlace[rob_j] == -1))
                {
                    inLeave[rob_i] = true;
                    inLeave[rob_j] = true;
                    inSlow[rob_j] = true;
                    inSlow[rob_j] = true;
                }
                else
                {
                    if ((cmd[rob_i].takeStation >= 0) && (cmd[rob_j].takeStation >= 0))
                    {
                        int rk_i = (state.station[cmd[rob_i].takeStation].type <= 3) +
                            (state.station[cmd[rob_i].takeStation].type <= 6) +
                            (state.station[cmd[rob_i].takeStation].type <= 7);
                        int rk_j = (state.station[cmd[rob_j].takeStation].type <= 3) +
                            (state.station[cmd[rob_j].takeStation].type <= 6) +
                            (state.station[cmd[rob_j].takeStation].type <= 7);
                        if (rk_i > rk_j)
                        {
                            inLeave[rob_i] = true;
                            inSlow[rob_j] = true;
                        }
                        else if (rk_i < rk_j)
                        {
                            inLeave[rob_j] = true;
                            inSlow[rob_i] = true;
                        }
                        else
                        {
                            if (safePlaceDis[rob_i] < safePlaceDis[rob_j])
                            {
                                inLeave[rob_i] = true;
                                inSlow[rob_j] = true;
                            }
                            else
                            {
                                inLeave[rob_j] = true;
                                inSlow[rob_i] = true;
                            }
                        }
                    }
                    else if ((cmd[rob_i].takeStation >= 0) && (cmd[rob_j].takeStation < 0))
                    {
                        inLeave[rob_j] = true;
                        inSlow[rob_i] = true;
                    }
                    else if ((cmd[rob_i].takeStation < 0) && (cmd[rob_j].takeStation >= 0))
                    {
                        inLeave[rob_i] = true;
                        inSlow[rob_j] = true;
                    }
                    else
                    {
                        if ((cmd[rob_i].sellStation >= 0) && (cmd[rob_j].sellStation >= 0))
                        {
                            int rk_i = (state.station[cmd[rob_i].sellStation].type <= 3) +
                                (state.station[cmd[rob_i].sellStation].type <= 6) +
                                (state.station[cmd[rob_i].sellStation].type <= 7) +
                                (state.station[cmd[rob_i].sellStation].type <= 9);
                            int rk_j = (state.station[cmd[rob_j].sellStation].type <= 3) +
                                (state.station[cmd[rob_j].sellStation].type <= 6) +
                                (state.station[cmd[rob_j].sellStation].type <= 7) +
                                (state.station[cmd[rob_i].sellStation].type <= 9);
                            if (rk_i > rk_j)
                            {
                                inLeave[rob_i] = true;
                                inSlow[rob_j] = true;
                            }
                            else if (rk_i < rk_j)
                            {
                                inLeave[rob_j] = true;
                                inSlow[rob_i] = true;
                            }
                            else
                            {
                                if (safePlaceDis[rob_i] < safePlaceDis[rob_j])
                                {
                                    inLeave[rob_i] = true;
                                    inSlow[rob_j] = true;
                                }
                                else
                                {
                                    inLeave[rob_j] = true;
                                    inSlow[rob_i] = true;
                                }
                            }
                        }
                        else if ((cmd[rob_i].sellStation >= 0) && (cmd[rob_j].sellStation < 0))
                        {
                            inLeave[rob_j] = true;
                            inSlow[rob_i] = true;
                        }
                        else if ((cmd[rob_i].sellStation < 0) && (cmd[rob_j].sellStation >= 0))
                        {
                            inLeave[rob_i] = true;
                            inSlow[rob_j] = true;
                        }
                        if (safePlaceDis[rob_i] < safePlaceDis[rob_j])
                        {
                            inLeave[rob_i] = true;
                            inSlow[rob_j] = true;
                        }
                        else
                        {
                            inLeave[rob_j] = true;
                            inSlow[rob_i] = true;
                        }
                    }
                }
            }
        }
    }
}

void avoid_path_search()
{
for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if (inLeave[rob_i] == true)
        {
            path[rob_i].clear();
            real_time_path_search(rob_i, true);
            if (path[rob_i].size() >= 2) { continue; }
            if (safePlace[rob_i] == -1) { continue; }

            if ((cmd[rob_i].state == 1)||(state.robot[rob_i].attacker)) {
                // ȡ����chessBoard
                path[rob_i].clear();
                int p_i = x2col_chess(state.robot[rob_i].p[0]);
                int p_j = y2row_chess(state.robot[rob_i].p[1]);
                int start = p_i + p_j * 101;
                int goal = safePlace[rob_i];
                //if (TEAM == 0)
                //{
                //    a_star_search(start, goal, 1, 0, rob_i, false);
                //    path_simple(rob_i, 1, 0,false);
                //}
                //else
                {
                    if (a_star_search(start, goal, 1, 0, rob_i, true) == false)
                    {
                        a_star_search(start, goal, 1, 0, rob_i, false);
                        path_simple(rob_i, 1, 0, false);
                    }
                    else
                    {
                        path_simple(rob_i, 1, 0, true);
                    }
                }
                
            }
            else if (cmd[rob_i].state == 2) {
                // �ͻ���expandWall
                path[rob_i].clear();
                int p_i = x2col(state.robot[rob_i].p[0]);
                int p_j = y2row(state.robot[rob_i].p[1]);
                int start = p_i + p_j * 100;
                int goal = safePlace[rob_i];
                //if (TEAM == 0)
                //{
                //    a_star_search(start, goal, 0, 0, rob_i, false);
                //    path_simple(rob_i, 0, 0,false);
                //}
                //else
                {
                    if (a_star_search(start, goal, 0, 0, rob_i, true) == false)
                    {
                        a_star_search(start, goal, 0, 0, rob_i, false);
                        path_simple(rob_i, 0, 0, false);
                    }
                    else
                    {
                        path_simple(rob_i, 0, 0, true);
                    }
                }
            }
        }
    }
}

/* ·���������ܣ�*/
void path_search() {
    // ͳ�Ʊ������˶�ס�ĸ���
    robot_block_init();
    //·��ȷ������������������ǽ�ڣ�
    real_time_path_search_for_all(false);
}

/* ���ϣ���ȫ���������ͻ���þ��ߣ� */
void avoid() {
    //����������·��������ϵ
    for (int rob_k = 0; rob_k < 4; rob_k++)
    {
        for (int rob_j = 0; rob_j < 4; rob_j++)
        {
            robotBlockByRobot[rob_k][rob_j] = false;
            robotBlockByRobotDist[rob_k][rob_j] = 0;
        }
        nearestRobot[rob_k] = -1;
        nearestRobotDist[rob_k] = 0;
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                pathMap[rob_k][i][j] = false;
                pathChess[rob_k][i][j] = false;
            }
            pathChess[rob_k][100][i] = false;
            pathChess[rob_k][i][100] = false;
        }
        pathChess[rob_k][100][100] = false;
    }
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 1) || (cmd[rob_i].state == -1))
        {
            path_block_by_robot(rob_i, 1, 0.25);
        }
        else
        {
            path_block_by_robot(rob_i, 0, 0.25);
        }
    }

    // ��ȫ������
    int safePlace[4] = { 0 };
    float safePlaceDis[4] = { 0 };
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if ((cmd[rob_i].state == 1) || (cmd[rob_i].state == -1))
        {
            safePlace[rob_i] = safe_place(rob_i, 1);
            if (safePlace[rob_i] != -1)
            {
                safePlaceDis[rob_i] = pow(col2x_chess(safePlace[rob_i] % 101) - state.robot[rob_i].p[0], 2) +
                    pow(row2y_chess(floor(safePlace[rob_i] / 101)) - state.robot[rob_i].p[1], 2);
            }
        }
        else
        {
            safePlace[rob_i] = safe_place(rob_i, 0);
            if (safePlace[rob_i] != -1)
            {
                safePlaceDis[rob_i] = pow(col2x(safePlace[rob_i] % 100) - state.robot[rob_i].p[0], 2) +
                    pow(row2y(floor(safePlace[rob_i] / 100)) - state.robot[rob_i].p[1], 2);
            }
        }
    }

    outdebug("nearestRobot[]:", nearestRobot[0], nearestRobot[1], nearestRobot[2], nearestRobot[3]);
    outdebug("nearestRobotDist[]:", nearestRobotDist[0], nearestRobotDist[1], nearestRobotDist[2], nearestRobotDist[3]);
    outdebug("safePlace[]:", safePlace[0], safePlace[1], safePlace[2], safePlace[3]);

    //��ͻ�رܾ���
    bool inLeave[4];
    bool inSlow[4];
    for (int rob_i = 0; rob_i < 3; rob_i++)
    {
        for (int rob_j = rob_i + 1; rob_j < 4; rob_j++)
        {
            bool blockFlag1 = false;
            bool blockFlag2 = false;
            if (robotBlockByRobot[rob_i][rob_j] == true)
                //if ((robotBlockByRobot[rob_i][rob_j] == true) && (robotBlockByRobotDist[rob_i][rob_j] < robBlockDistance[map_num]))
            {
                // rob_i��·����rob_j
                blockFlag1 = true;
            }
            if (robotBlockByRobot[rob_j][rob_i] == true)
                //if ((robotBlockByRobot[rob_j][rob_i] == true) && (robotBlockByRobotDist[rob_j][rob_i] < robBlockDistance[map_num]))
            {
                // rob_j��·����rob_i
                blockFlag2 = true;
            }
            if ((blockFlag1 == true) && (blockFlag2 == true))
            {
                if ((safePlace[rob_i] == -1) && (safePlace[rob_j] != -1))
                {
                    inLeave[rob_j] = true;
                    inSlow[rob_i] = true;
                }
                else if ((safePlace[rob_i] != -1) && (safePlace[rob_j] == -1))
                {
                    inLeave[rob_i] = true;
                    inSlow[rob_j] = true;
                }
                else if ((safePlace[rob_i] == -1) && (safePlace[rob_j] == -1))
                {
                    inLeave[rob_i] = true;
                    inLeave[rob_j] = true;
                    inSlow[rob_j] = true;
                    inSlow[rob_j] = true;
                }
                else
                {
                    if ((cmd[rob_i].takeStation >= 0) && (cmd[rob_j].takeStation >= 0))
                    {
                        int rk_i = (state.station[cmd[rob_i].takeStation].type <= 3) +
                            (state.station[cmd[rob_i].takeStation].type <= 6) +
                            (state.station[cmd[rob_i].takeStation].type <= 7);
                        int rk_j = (state.station[cmd[rob_j].takeStation].type <= 3) +
                            (state.station[cmd[rob_j].takeStation].type <= 6) +
                            (state.station[cmd[rob_j].takeStation].type <= 7);
                        if (rk_i > rk_j)
                        {
                            inLeave[rob_i] = true;
                            inSlow[rob_j] = true;
                        }
                        else if (rk_i < rk_j)
                        {
                            inLeave[rob_j] = true;
                            inSlow[rob_i] = true;
                        }
                        else
                        {
                            if (safePlaceDis[rob_i] < safePlaceDis[rob_j])
                            {
                                inLeave[rob_i] = true;
                                inSlow[rob_j] = true;
                            }
                            else
                            {
                                inLeave[rob_j] = true;
                                inSlow[rob_i] = true;
                            }
                        }
                    }
                    else if ((cmd[rob_i].takeStation >= 0) && (cmd[rob_j].takeStation < 0))
                    {
                        inLeave[rob_j] = true;
                        inSlow[rob_i] = true;
                    }
                    else if ((cmd[rob_i].takeStation < 0) && (cmd[rob_j].takeStation >= 0))
                    {
                        inLeave[rob_i] = true;
                        inSlow[rob_j] = true;
                    }
                    else
                    {
                        if ((cmd[rob_i].sellStation >= 0) && (cmd[rob_j].sellStation >= 0))
                        {
                            int rk_i = (state.station[cmd[rob_i].sellStation].type <= 3) +
                                (state.station[cmd[rob_i].sellStation].type <= 6) +
                                (state.station[cmd[rob_i].sellStation].type <= 7) +
                                (state.station[cmd[rob_i].sellStation].type <= 9);
                            int rk_j = (state.station[cmd[rob_j].sellStation].type <= 3) +
                                (state.station[cmd[rob_j].sellStation].type <= 6) +
                                (state.station[cmd[rob_j].sellStation].type <= 7) +
                                (state.station[cmd[rob_i].sellStation].type <= 9);
                            if (rk_i > rk_j)
                            {
                                inLeave[rob_i] = true;
                                inSlow[rob_j] = true;
                            }
                            else if (rk_i < rk_j)
                            {
                                inLeave[rob_j] = true;
                                inSlow[rob_i] = true;
                            }
                            else
                            {
                                if (safePlaceDis[rob_i] < safePlaceDis[rob_j])
                                {
                                    inLeave[rob_i] = true;
                                    inSlow[rob_j] = true;
                                }
                                else
                                {
                                    inLeave[rob_j] = true;
                                    inSlow[rob_i] = true;
                                }
                            }
                        }
                        else if ((cmd[rob_i].sellStation >= 0) && (cmd[rob_j].sellStation < 0))
                        {
                            inLeave[rob_j] = true;
                            inSlow[rob_i] = true;
                        }
                        else if ((cmd[rob_i].sellStation < 0) && (cmd[rob_j].sellStation >= 0))
                        {
                            inLeave[rob_i] = true;
                            inSlow[rob_j] = true;
                        }
                        if (safePlaceDis[rob_i] < safePlaceDis[rob_j])
                        {
                            inLeave[rob_i] = true;
                            inSlow[rob_j] = true;
                        }
                        else
                        {
                            inLeave[rob_j] = true;
                            inSlow[rob_i] = true;
                        }
                    }
                }
            }
        }
    }

    // �رܻ����˵�·���ٹ滮��Ҫ�������ǰ����ȫ�㣬��Ϊ������������·��
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        if (inLeave[rob_i] == true)
        {
            path[rob_i].clear();
            real_time_path_search(rob_i, true);
            if (path[rob_i].size() >= 2) { continue; }
            if (safePlace[rob_i] == -1) { continue; }

            if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker)) {
                path[rob_i].clear();
                int p_i = x2col_chess(state.robot[rob_i].p[0]);
                int p_j = y2row_chess(state.robot[rob_i].p[1]);
                int start = p_i + p_j * 101;
                int goal = safePlace[rob_i];
                //if (TEAM == 0)
                //{
                //    a_star_search(start, goal, 1, 0, rob_i, false);
                //    path_simple(rob_i, 1, 0,false);
                //}
                //else
                {
                    if (a_star_search(start, goal, 1, 0, rob_i, true) == false)
                    {
                        a_star_search(start, goal, 1, 0, rob_i, false);
                        path_simple(rob_i, 1, 0, false);
                    }
                    else
                    {
                        path_simple(rob_i, 1, 0, true);
                    }
                }

            }
            else if (cmd[rob_i].state == 2) {
                path[rob_i].clear();
                int p_i = x2col(state.robot[rob_i].p[0]);
                int p_j = y2row(state.robot[rob_i].p[1]);
                int start = p_i + p_j * 100;
                int goal = safePlace[rob_i];
                //if (TEAM == 0)
                //{
                //    a_star_search(start, goal, 0, 0, rob_i, false);
                //    path_simple(rob_i, 0, 0,false);
                //}
                //else
                {
                    if (a_star_search(start, goal, 0, 0, rob_i, true) == false)
                    {
                        a_star_search(start, goal, 0, 0, rob_i, false);
                        path_simple(rob_i, 0, 0, false);
                    }
                    else
                    {
                        path_simple(rob_i, 0, 0, true);
                    }
                }

            }
        }
    }

    outdebug("path[].size:", path[0].size(), path[1].size(), path[2].size(), path[3].size());
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        for (int i = 0; i < path[rob_i].size(); i++)
        {
            outdebug("������", rob_i, "��·����", i, "Ϊ:", path[rob_i][i]);
        }
    }
}
