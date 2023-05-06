#include <cmath>
#include "Calculate.h"
#include "Global.h"
#include "Map.h"
#include "Schedule.h"

/* ��������ĵڼ������������� */
float col2x(int col) {
    return col * 0.5 + 0.25;
}

/* ��������ĵڼ������������� */
float row2y(int row) {
    return 50 - (row * 0.5 + 0.25);
}

/* ���������������ڼ��� */
int x2col(float x) {
    return floor(x / 0.5);
}

/* ���������������ڼ��� */
int y2row(float y) {
    return floor((50 - y) / 0.5);
}

/* ��������ĵڼ������������� */
int x2col_chess(float x)
{
    return floor((x + 0.25) / 0.5);
}

/* ��������ĵڼ������������� */
int y2row_chess(float y)
{
    return floor((50 - (y - 0.25)) / 0.5);
}

/* ���������������ڼ��� */
float col2x_chess(int col)
{
    return col * 0.5;
}

/* ���������������ڼ��� */
float row2y_chess(int row)
{
    return (100 - row) * 0.5;
}

/* �����پ��� */
float manhattan_distance(int p1_i, int p1_j, int p2_i, int p2_j)
{
    return (abs(p1_i - p2_i) + abs(p1_j - p2_j));
}

/* ������������ľ�������ͼ��Ե�ľ���*/
float cal_map_distance_d(float robot_coor[2])
{
    float cal_Map_Distance_D;
    //��������˵�����
    //float robot_coor[2];
    //������߿���ĸ�����
    float robot_Distance[4];

    robot_Distance[0] = fabs(0 - robot_coor[0]);
    robot_Distance[1] = fabs(50 - robot_coor[0]);
    robot_Distance[2] = fabs(0 - robot_coor[1]);
    robot_Distance[3] = fabs(50 - robot_coor[1]);

    //ð������
    float temp = 0.0;
    int key = 0;
    while (key < 4)
    {
        for (int i = 3; i > key; i--)
        {
            //�Ƚ���ƽ���˵Ĵ�С��������򽻻������򲻲���
            if (robot_Distance[i] < robot_Distance[i - 1])
            {
                temp = robot_Distance[i];
                robot_Distance[i] = robot_Distance[i - 1];
                robot_Distance[i - 1] = temp;
            }
        }
        //���ź����Ԫ�ز��ٲ�������
        key++;
    }

    cal_Map_Distance_D = robot_Distance[0];

    return cal_Map_Distance_D;
}

/* �ص�ǰ���򵽵�ͼ��Ե�ľ��� */
float cal_map_distance(int rob_i)
{
    float cla_Map_Distance, Map_cor_x = 0, Map_cor_y = 0;
    if (state.robot[rob_i].direction > (-1 * PI / 2) && state.robot[rob_i].direction < (PI / 2))
    {
        //����x=50�������ֱ�ߵĽ���
        Map_cor_y = tan(state.robot[rob_i].direction) * (50 - state.robot[rob_i].p[0]) + state.robot[rob_i].p[1];
        if (Map_cor_y >= 0 && Map_cor_y <= 50)
        {
            Map_cor_x = 50;
            Map_cor_y = Map_cor_y;
        }
        else if (Map_cor_y < 0)
        {
            Map_cor_x = state.robot[rob_i].p[0] + (0 - state.robot[rob_i].p[1]) / tan(state.robot[rob_i].direction);
            Map_cor_y = 0;
        }
        else if (Map_cor_y > 50)
        {
            Map_cor_x = state.robot[rob_i].p[0] + (50 - state.robot[rob_i].p[1]) / tan(state.robot[rob_i].direction);
            Map_cor_y = 50;
        }
    }
    else if ((state.robot[rob_i].direction > (-1 * PI / 2) && state.robot[rob_i].direction < (-1 * PI / 2)) || (state.robot[rob_i].direction > (PI / 2) && state.robot[rob_i].direction < PI))
    {

        //����x=0�������ֱ�ߵĽ���
        Map_cor_y = tan(state.robot[rob_i].direction) * (0 - state.robot[rob_i].p[0]) + state.robot[rob_i].p[1];
        if (Map_cor_y >= 0 && Map_cor_y <= 50)
        {
            Map_cor_x = 0;
            Map_cor_y = Map_cor_y;
        }
        else if (Map_cor_y < 0)
        {
            Map_cor_x = state.robot[rob_i].p[0] + (0 - state.robot[rob_i].p[1]) / tan(state.robot[rob_i].direction);
            Map_cor_y = 0;
        }
        else if (Map_cor_y > 50)
        {
            Map_cor_x = state.robot[rob_i].p[0] + (50 - state.robot[rob_i].p[1]) / tan(state.robot[rob_i].direction);
            Map_cor_y = 50;
        }
    }
    //�����Ե�ǰ���򵽵�ͼ�߿�֮��ľ���
    cla_Map_Distance = sqrt((Map_cor_x - state.robot[rob_i].p[0]) * (Map_cor_x - state.robot[rob_i].p[0]) + (Map_cor_y - state.robot[rob_i].p[1]) * (Map_cor_y - state.robot[rob_i].p[1]));

    return cla_Map_Distance;
}

/* Ŀ��Ƕ��뵱ǰ����н� */
float clamping_angle(int rob_i, float Target_Coord[2])
{
    /*���Ȼ�ȡ��ǰ����������*/
    float Robot_Coord[2];
    /*Ȼ��ͨ��������Ŀ������*/
    float Target_Vector[2];
    float rotate_v_r;
    float Alfa;
    float Beta = 0;

    Robot_Coord[0] = state.robot[rob_i].p[0];
    Robot_Coord[1] = state.robot[rob_i].p[1];

    Target_Vector[0] = Target_Coord[0] - Robot_Coord[0];
    Target_Vector[1] = Target_Coord[1] - Robot_Coord[1];

    /*Ȼ����㵱ǰ������x��������ļн�Alfa��Ŀ�꺽����x��������ļн�Beta*/
    Alfa = state.robot[rob_i].direction;

    //�ж�Ŀ��������������
    if (Target_Vector[0] > 0) //��һ��������
    {
        Beta = atan(Target_Vector[1] / Target_Vector[0]);
    }
    else if ((Target_Vector[0] < 0) && (Target_Vector[1] >= 0))//�ڶ����� +
    {
        Beta = PI + atan(Target_Vector[1] / Target_Vector[0]);
    }
    else if ((Target_Vector[0] < 0) && (Target_Vector[1] < 0))//��������
    {
        Beta = -1 * PI + atan(Target_Vector[1] / Target_Vector[0]);
    }
    else if ((Target_Vector[0] == 0) && (Target_Vector[1] > 0))
    {
        Beta = PI / 2;
    }
    else if ((Target_Vector[0] == 0) && (Target_Vector[1] < 0))
    {
        Beta = -1 * PI / 2;
    }
    //���Ƕ�������õ��н� ����˵�Ƕ���� ����-���� ����Ϊ0 ����ֵΪAlfa-Beta
    //clampingAngle[rob_i] = Beta - Alfa;
    float clampingAngle;
    if ((Beta - Alfa) >= PI)
    {
        clampingAngle = Alfa - Beta + 2 * PI;
    }
    else if ((Beta - Alfa) < (-1 * PI + 0.01))
    {
        clampingAngle = Alfa - Beta - 2 * PI;
    }
    else if (fabs(Beta - Alfa) < PI)
    {
        clampingAngle = Alfa - Beta;
    }
    else
    {
        clampingAngle = Alfa - Beta;
    }

    return clampingAngle; //�õ����ǽǶȲ���ֵ
}

/* �°��ۺ�·�̼��� */
float new_dist_cal(float distr2b, float distb2s, float theta) {
    return (pow((distr2b * (1 + (theta * mapparams[MAPKIND].ETA)) + distb2s), mapparams[MAPKIND].BETA));
}

/* ����ÿ�ֲ�Ʒ��ȫ�ּ�ֵ��G �� (1 - ��^-k) �� �� */
float cal_productValue(int type) {
    //�����Ͳ�Ʒ�������Ĺ���̨����
    int quantity = 0;
    switch (type)
    {
    case 1:
        quantity = stationType[4].quantity + stationType[5].quantity;
        break;
    case 2:
        quantity = stationType[4].quantity + stationType[6].quantity;
        break;
    case 3:
        quantity = stationType[5].quantity + stationType[6].quantity;
        break;
    case 4:
        quantity = stationType[7].quantity + stationType[9].quantity;
        break;
    case 5:
        quantity = stationType[7].quantity + stationType[9].quantity;
        break;
    case 6:
        quantity = stationType[7].quantity + stationType[9].quantity;
        break;
    case 7:
        quantity = stationType[8].quantity + stationType[9].quantity;
        break;
    default:
        break;
    }
    //return (stationType[type].gain * (log10(quantity + 1) / log10(mapparams[map_num].ALPHA)) * mapparams[map_num].MIU);
    return (stationType[type].gain * (1 - pow(mapparams[MAPKIND].ALPHA, -quantity)) * mapparams[MAPKIND].MIU);
}

/* ���ڼ���{ʱ���ֵϵ��}��{��ײ��ֵϵ��}�ĺ���f */
float f_func(float x, float maxX, float minRate)
{
    return (x < maxX) ? ((1.0f - sqrt(1.0f - pow(1.0f - x / maxX, 2))) * (1.0f - minRate) + minRate) : minRate;
}

/* �ж�ĳ����̨��ԭ�ϸ��Ƿ����ĳ����Ʒ */
bool pro_exsit(int type, int materials) {
    return materials & (1 << type);
}

/* �ۺ�������� */
float income_cal(int takeS, int sellS)
{
    // ֱ���ֽ���
    float income = stationType[state.station[takeS].type].gain;
    // ֱ���ֽ�����ʱ��˥��
    income = income * s2sTimeLose[takeS][sellS];
    // ����һ����Ʒ�ֽ���
    float futureIncome = stationType[state.station[sellS].type].gain;
    // �ж�һ�£��ټӹ��Ƿ���ò�Ʒ
    //�ж��������Ƿ��ǿ��ټӹ�վ
    if (state.station[sellS].type <= 7)
    {
        // �������в���
        int haveRcv = 0;
        for (int i = 1; i < 8; i++)
        {
            haveRcv += (state.station[sellS].materials >> i) % 2;
        }
        // ���в���Խ�࣬����δ������Խ��
        // ϵ����0.1���޲��ϣ�,0.2�����յ�1����,0.3�����յ�2����
        futureIncome = futureIncome * (0.1 * (haveRcv + 1));
    }
    //������ǿ��ټӹ��أ�Ԥ������Ϊ0
    else {
        futureIncome = 0;
    }
    //�����ۺ�����
    return (income + futureIncome);
}

/* �°��ۺ�������� */
float new_income_cal(int takeS, int sellS, float priorty)
{
    // �ۺ�����۸�
    float buycost = stationType[state.station[takeS].type].cost * (1 - priorty);
    // �������������в�����
    int haveRcv = 0;
    //�������Ƿ��ǿ��ټӹ�վ
    if (state.station[sellS].type <= 7)
    {
        for (int i = 1; i < 8; i++)
        {
            haveRcv += (state.station[sellS].materials >> i) % 2;
        }
    }
    // �ۺϳ��ۼ۸�
    float sellcost = stationType[state.station[takeS].type].sell * s2sTimeLose[takeS][sellS] * (1 + (haveRcv * mapparams[MAPKIND].OMEGA));
    // �����ۺ�����
    return (sellcost - buycost);
}