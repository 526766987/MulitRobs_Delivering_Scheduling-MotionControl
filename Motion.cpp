#include <cmath>
#include "Motion.h"
#include "Global.h"
#include "Path.h"
#include "Attack.h"
#include "Out.h"
#include "Schedule.h"
#include "Calculate.h"
using namespace std;

Rotate_PID_param rotate_PID_param[4];

//��ͷ״̬��������
int rotate_model;
// Ŀ��Ƕ��뵱ǰ����н�
float clampingAngle[4] = {0};


/* �Ƕ�PID���Ƴ�ʼ�� */
void rotate_PID_init(int rob_i)
{
    rotate_PID_param[rob_i].err_r = 0;
    rotate_PID_param[rob_i].err_last_r = 0;
    rotate_PID_param[rob_i].Kp_r = 0.89;//����ֵ��Ҫ���Բ���ȷ��
    rotate_PID_param[rob_i].Ki_r = 0.0012;//
    rotate_PID_param[rob_i].Kd_r = 0.0;//
    rotate_PID_param[rob_i].integral_r = 0;
    rotate_PID_param[rob_i].v_r = 0;
    rotate_PID_param[rob_i].v_last_r = 0;
    rotate_PID_param[rob_i].old_target[0] = 0;
    rotate_PID_param[rob_i].old_target[1] = 0;
}

void motion_by_path()
{
    //�˶�����
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        //������
        if ((state.robot[rob_i].attacker) && (attackerState[rob_i] != 0) && (attackerState[rob_i] != 6))
        {
            continue;
        }
        // ��·��
        if (path[rob_i].size() == 0)
        {
            if (cmd[rob_i].takeStation != -1)
            {
                send_cmd_forward(rob_i, -2);
                send_cmd_rotate(rob_i, 0);
                continue;
            }
            else
            {
                //forward_rotate_control(rob_i, state.robot[rob_i].p);
                //forward_rotate_control_APF(rob_i, state.robot[rob_i].p);
                send_cmd_forward(rob_i, 0);
                send_cmd_rotate(rob_i, 0);
                continue;
            }
        }
        // ·����һ����
        if (path[rob_i].size() == 1)
        {
            float p[2];
            if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker))
            {
                p[0] = col2x_chess(path[rob_i][0] % 101);
                p[1] = row2y_chess(path[rob_i][0] / 101);
                p[0] += 0.25;
                p[1] -= 0.25;
            }
            else if (cmd[rob_i].state == 2)
            {
                p[0] = col2x(path[rob_i][0] % 100);
                p[1] = row2y(path[rob_i][0] / 100);
            }
            //forward_rotate_control(rob_i, p);
            forward_rotate_control_APF(rob_i, p);
            continue;
        }
        //·����������
        if (path[rob_i].size() == 2)
        {
            float p[2];
            if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker))
            {
                p[0] = col2x_chess(path[rob_i][1] % 101);
                p[1] = row2y_chess(path[rob_i][1] / 101);
                p[0] += 0.25;
                p[1] -= 0.25;
            }
            else if (cmd[rob_i].state == 2)
            {
                p[0] = col2x(path[rob_i][1] % 100);
                p[1] = row2y(path[rob_i][1] / 100);
            }
            if ((state.robot[rob_i].attacker) && (attackerState[rob_i] == 0))
            {
                if (TEAM == 0)
                {
                    for (int i = 0; i < cornerEnemyStation.size(); i++)
                    {
                        if (cornerEnemyStation[i] == currentAttackEnemyStation[rob_i])
                        {
                            switch (cornerEnemyStationKind[i])
                            {
                            case 0://������ǽ��
                                p[0] += -1;
                                p[1] += 1;
                                break;
                            case 1://������ǽ��
                                p[0] += 1;
                                p[1] += 1;
                                break;
                            case 2://������ǽ��
                                p[0] += 1;
                                p[1] += -1;
                                break;
                            case 3://������ǽ��
                                p[0] += -1;
                                p[1] += -1;
                                break;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < cornerEnemyStation.size(); i++)
                    {
                        if (cornerEnemyStation[i] == currentAttackEnemyStation[rob_i])
                        {
                            switch (cornerEnemyStationKind[i])
                            {
                            case 0://������ǽ��
                                p[0] += 0.3;
                                p[1] += -0.3;
                                break;
                            case 1://������ǽ��
                                p[0] += -0.3;
                                p[1] += -0.3;
                                break;
                            case 2://������ǽ��
                                p[0] += -0.3;
                                p[1] += +0.3;
                                break;
                            case 3://������ǽ��
                                p[0] += +0.3;
                                p[1] += +0.3;
                                break;
                            }
                            break;
                        }
                    }
                }
            }   
            outdebug("1620target:",path[rob_i][1],"���꣺",p[0],p[1]);
            //forward_rotate_control(rob_i, p);
            forward_rotate_control_APF(rob_i, p);
            //if (fabs(clamping_angle(rob_i, p)) < PI /8)
            //{
            //    send_cmd_forward(rob_i, 6 + (TEAM == 1));
            //}
            if (state.robot[rob_i].bring > 0)
            {
                if (state.station[cmd[rob_i].sellStation].enemyOccupy > 0)
                {
                    send_cmd_forward(rob_i, 6 + (TEAM == 1));
                }
            }
            else
            {
                if (state.station[cmd[rob_i].takeStation].enemyOccupy > 0)
                {
                    send_cmd_forward(rob_i, 6 + (TEAM == 1));
                }
            }
            continue;
        }
        //·����һ�ѵ�
        float p[2];
        if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker))
        {
            p[0] = col2x_chess(path[rob_i][1] % 101);
            p[1] = row2y_chess(path[rob_i][1] / 101);
        }
        else if (cmd[rob_i].state == 2)
        {
            p[0] = col2x(path[rob_i][1] % 100);
            p[1] = row2y(floor(path[rob_i][1] / 100));
        }
        //forward_rotate_control(rob_i, p);
        //forward_rotate_control_new(rob_i, p, path_state[rob_i]);
        forward_rotate_control_APF(rob_i, p);
        //if ((inSlow[rob_i] == true) && (nearestRobotDist[rob_i] < slowSpeedDist))//������ǰ�����������ǽǶȿ�����ԭ�е�
        //{
        //    slowSpeed = 5.5;
        //    if (nearestRobotDist[rob_i] < 1.25)
        //    {
        //        slowSpeed = 3.5;
        //    }
        //    //slowSpeed = sqrt(2 * 14 * (slowSpeedDist));//nearestRobotDist[rob_i] + 0.1
        //    send_cmd_forward(rob_i, slowSpeed);
        //    //outdebug("rob_i, slowSpeed,nearestRobotDist:",rob_i, slowSpeed,nearestRobotDist[rob_i]);
        //}
    }

}


// APF
void forward_rotate_control_APF(int rob_i, float Target_Coord[2])//����֡��Ϣ����������ź�Ŀ�������
{
    //�����˹��Ƴ�
    //���ȼ�����Χ8��̽��������
    //��������
    float Extend_distance;
    //�����ٶ�
    float Acceleration_forward, Acceleration_rotate, robot_mass, force_forward_max, force_rotate_max;
    float point_virtual_force_MAX;
    float robot_radius;

    // ��ѧ����
    if (state.robot[rob_i].bring == 0)
    {
        Extend_distance = 0.45 * 2.5; //�뾶����
        robot_radius = 0.45;
        Acceleration_forward = 19.6488;//ÿ�룬ֱ�߼��ٶ�
        Acceleration_rotate = 38.8125;//�Ǽ��ٶ�
        if (TEAM == 0)
        {
            robot_mass = 12.7234; //����
            point_virtual_force_MAX = 35; // ���������ֵ
            force_forward_max = 250; //ǰ��������
            force_rotate_max = 50; //ת��������
        }
        else if (TEAM == 1)
        {
            robot_mass = 9.5426;
            point_virtual_force_MAX = 25;
            force_forward_max = 187.5;
            force_rotate_max = 37.5;
        }

    }
    else
    {
        Extend_distance = 0.53 * 2.5;
        robot_radius = 0.53;
        Acceleration_forward = 14.1056;//ÿ��
        Acceleration_rotate = 20.1704;
        if (TEAM == 0)
        {
            robot_mass = 17.6494;
            point_virtual_force_MAX = 35;
            force_forward_max = 250;
            force_rotate_max = 50;
        }
        else if (TEAM == 1)
        {
            robot_mass = 13.2371;
            point_virtual_force_MAX = 25;
            force_forward_max = 187.5;
            force_rotate_max = 37.5;
        }
    }

    float forward_v_dt = 0; // �������ļ��ٶ�
    float rotate_v_dt = 0; // ��������ת����ٶ�

    float rotate_control_value, forward_control_value, robot_forward_force, robot_rotate_force;

    float robot_speed = sqrt(state.robot[rob_i].speed[0] * state.robot[rob_i].speed[0] + state.robot[rob_i].speed[1] * state.robot[rob_i].speed[1]);
    
    forward_control_value = forward_control(rob_i, Target_Coord); //ԭ����ǰ��Ŀ���ٶ�
    robot_forward_force = (forward_control_value - robot_speed) * 50 * robot_mass + 2; //ԭ����ǰ��ǣ����
    outdebug("�����˺ţ�", rob_i, "forward_control_value:", forward_control_value, "robot_speed", robot_speed);
    // ����force_forward_max
    if (robot_forward_force >= force_forward_max)
    {
        robot_forward_force = force_forward_max;
    }
    if (robot_forward_force <= (-1 * force_forward_max))
    {
        robot_forward_force = -1 * force_forward_max;
    }
    
    rotate_control_value = rotate_control(rob_i, Target_Coord);//ԭ����Ŀ��ת��
    robot_rotate_force = (rotate_control_value - state.robot[rob_i].angularSpeed) * 50 * 0.5 * robot_mass * robot_radius * robot_radius;//ԭ����ת��ǣ����
    //����force_rotate_max
    if (robot_rotate_force >= force_rotate_max)
    {
        robot_rotate_force = force_rotate_max;
    }
    if (robot_rotate_force <= (-1 * force_rotate_max))
    {
        robot_rotate_force = -1 * force_rotate_max;
    }

    // ǽ�ں����������������������
    //float virtual_force[30];
    float virtual_force_vector[30][2]; //���㣨30����
    float virtual_total_force_vector[2] = { 0,0 }; //��

    for (int i = 0; i < 30; i++)
    {
        int lasers_count = i * 12;
        if (state.robot[rob_i].lasers[lasers_count] < Extend_distance)
        {
            //��������������Ĵ�С���и��� ָ���½�
            float virtual_force = (2 * state.robot[rob_i].lasersObject[lasers_count] + 0.1) * point_virtual_force_MAX * exp(-2 * state.robot[rob_i].lasers[lasers_count] / Extend_distance);
                //ǽ��0���棬�����ˣ�1���档exp(-2)��-2��˥��ϵ��
            float angle = state.robot[rob_i].lasersMapDirection[lasers_count];
                //�����ĽǶ�+180��
            virtual_force_vector[i][0] = -1 * virtual_force * cos(angle);
            virtual_force_vector[i][1] = -1 * virtual_force * sin(angle);
        }
        else
        {
            virtual_force_vector[i][0] = 0;
            virtual_force_vector[i][1] = 0;
        }
        // �ۻ�����
        virtual_total_force_vector[0] = virtual_total_force_vector[0] + virtual_force_vector[i][0];
        virtual_total_force_vector[1] = virtual_total_force_vector[1] + virtual_force_vector[i][1];
    }
    //forѭ����ɵõ�����
    //�����ֽ⵽�����˵ĳ���ʹ�ֱ����
    //������������볯��ļн�
    outdebug("����������", virtual_total_force_vector[0], virtual_total_force_vector[1]);

    // ����
    float virtual_force_forward = virtual_total_force_vector[0] * cos(state.robot[rob_i].direction) + virtual_total_force_vector[1] * cos(PI / 2 - state.robot[rob_i].direction);
    float virtual_force_rotate = (-1) * virtual_total_force_vector[0] * sin(state.robot[rob_i].direction) + virtual_total_force_vector[1] * sin(PI / 2 - state.robot[rob_i].direction);
    outdebug("�����˺ţ�", rob_i, "robot_forward_force:", robot_forward_force);
    outdebug("�����˺ţ�", rob_i, "robot_rotate_force:", robot_rotate_force);
    outdebug("�����˺ţ�", rob_i, "virtual_force_forward:", virtual_force_forward);
    outdebug("�����˺ţ�", rob_i, "virtual_force_rotate:", virtual_force_rotate);

    // ���� virtual_force_forward, virtual_force_rotate
    // ʵ�� robot_forward_force��robot_rotate_force
    // ���ں�
    float forward_force_output = robot_forward_force + virtual_force_forward;
    float rotate_force_output = robot_rotate_force + virtual_force_rotate*0;
    //float forward_force_output = robot_forward_force;
    //float rotate_force_output = robot_rotate_force;
    forward_v_dt = (forward_force_output / robot_mass) / 50;
    rotate_v_dt = (rotate_force_output * 2) / (robot_mass * Extend_distance * Extend_distance) / 50;

    float forward_v_APF = robot_speed + forward_v_dt;
    float rotate_v_APF = state.robot[rob_i].angularSpeed + rotate_v_dt;

    outdebug("�����˺ţ�", rob_i, "forward_v_APF", forward_v_APF, "rotate_v_APF", rotate_v_APF);
    if (fabs(forward_v_APF) < 0.04)
    {
        forward_v_APF = 0.1;
    }

    //����ٶ�ָ��
    send_cmd_forward(rob_i, forward_v_APF);
    //send_cmd_rotate(rob_i, rotate_v_APF);

}


//�ٶȿ���
float forward_control(int rob_i, float Target_Coord[2])
{
    float Map_Distance, Target_Distance, Map_Distance_D;
    float rob_v_max;
    float rob_v;
    float Acceleration_map;
    /*���Ȼ�ȡ��ǰ����������*/
    float Robot_Coord[2];
    /*Ȼ��ͨ��������Ŀ������*/
    float Target_Vector[2];
    float aa = 0.05;
    Robot_Coord[0] = state.robot[rob_i].p[0];
    Robot_Coord[1] = state.robot[rob_i].p[1];
    Target_Vector[0] = Target_Coord[0] - Robot_Coord[0];
    Target_Vector[1] = Target_Coord[1] - Robot_Coord[1];

    //���㵱ǰ�ٶ�
    //rob_v = sqrt(state.robot[rob_i].speed[0] * state.robot[rob_i].speed[0] + state.robot[rob_i].speed[1] * state.robot[rob_i].speed[1]);
    //�����Ŀ������
    Target_Distance = sqrt(Target_Vector[0] * Target_Vector[0] + Target_Vector[1] * Target_Vector[1]);

    if (state.robot[rob_i].bring == 0)//�жϻ�������û��Я����Ʒ
    {
        Acceleration_map = 19.6488;//ÿ��
    }
    else
    {
        Acceleration_map = 14.1056;//ÿ��
    }

    float clampingAngle = clamping_angle(rob_i, Target_Coord);
    outdebug("�����˺ţ�", rob_i, "ǰ�����clampingAngle=", clampingAngle);
    if (fabs(clampingAngle) < (PI * 1 / 8))
    {
        if (Target_Distance >= 1.0)
        {
            rob_v = 7;
        }
        if (Target_Distance <= 1.0)
        {
            rob_v = sqrt(2 * Acceleration_map * (Target_Distance));
        }
        if (Target_Distance < 0.2)
        {
            rob_v = 0.01;
        }
    }
    else
    {
        rob_v = 0.05;
    }
    outdebug("�����˺ţ�", rob_i, "rob_v=", rob_v);

    send_cmd_forward(rob_i, rob_v);

    return rob_v;
}


//�õ�Ŀ��Ƕ�֮���ǽ���ת�ٿ���
float rotate_control(int rob_i, float Target_Coord[2])
{
    float rotate_v_r;

    float clampingAngle = clamping_angle(rob_i, Target_Coord);
    outdebug("�����˺ţ�", rob_i, "ת�����clampingAngle=", clampingAngle);
    //��Բ�ͬ�нǵ������������,��������
    if (clampingAngle > (PI * 1.5 / 8))// && (rotate_model == 0)
    {
        //rotate_PID_param[rob_i].err_r = 5*tan(PI*3/8)*0.2;
        //ֱ�Ӹ���ת��
        rotate_PID_param[rob_i].v_r = -1 * PI;//-1 * 1 * 5 * tan(PI * 3 / 8) * 0.8
        rotate_PID_param[rob_i].Ki_r = 0.000;//
        rotate_PID_param[rob_i].integral_r = -0.0;//
        //rotate_model = 1;
    }
    else if (clampingAngle < (-1 * PI * 1.5 / 8))
    {
        //rotate_PID_param[rob_i].err_r = -5*tan(PI*3/8)*0.2;
        rotate_PID_param[rob_i].v_r = PI;//1 * 5 * tan(PI * 3 / 8) * 0.8
        rotate_PID_param[rob_i].Ki_r = 0.000;//
        rotate_PID_param[rob_i].integral_r = 0.0;//
        //rotate_model = 2;
    }
    else if (fabs(clampingAngle) <= (PI * 1.5 / 8))
    {
        //rotate_model = 0;
        rotate_PID_param[rob_i].err_r = 10 * tan(0 - clampingAngle);//��ͬ����¶�err������Ҫ���� ����������-����ֵ
        rotate_PID_param[rob_i].Ki_r = 0.001;//
        ////outdebug("rotate_PID_param[rob_i].err_r",rotate_PID_param[rob_i].err_r);
        //����ۻ�
        rotate_PID_param[rob_i].integral_r += rotate_PID_param[rob_i].err_r;

        //����Ŀ��㣬��������ۻ�
        if ((rotate_PID_param[rob_i].old_target[0] != Target_Coord[0]) && (rotate_PID_param[rob_i].old_target[1] != Target_Coord[1]))
        {
            rotate_PID_param[rob_i].integral_r = 0;
            ////outdebug("integral_r = 0");
        }

        rotate_PID_param[rob_i].old_target[0] = Target_Coord[0];
        rotate_PID_param[rob_i].old_target[1] = Target_Coord[1];

        //PI�㷨ʵ��
        rotate_PID_param[rob_i].v_r = rotate_PID_param[rob_i].Kp_r * rotate_PID_param[rob_i].err_r +
            rotate_PID_param[rob_i].Ki_r * rotate_PID_param[rob_i].integral_r +
            rotate_PID_param[rob_i].Kd_r * (rotate_PID_param[rob_i].err_r - rotate_PID_param[rob_i].err_last_r);
        rotate_PID_param[rob_i].v_r = 1 * rotate_PID_param[rob_i].v_r;
        ////outdebug("rotate_PID_param[rob_i].v_r", rotate_PID_param[rob_i].v_r);

        //����
        rotate_PID_param[rob_i].err_last_r = rotate_PID_param[rob_i].err_r;
    }

    ////outdebug("clampingAngle[rob_i]", clampingAngle[rob_i]);


    rotate_v_r = rotate_PID_param[rob_i].v_r;
    //����Ϸ��Լ���
    if (rotate_v_r >= PI)
    {
        rotate_v_r = PI - 0.001;
    }
    if (rotate_v_r <= -1 * PI)
    {
        rotate_v_r = -1 * PI + 0.001;
    }
    //�������Ѿ��㹻С�ˣ���ת��Ϊ0
    if (fabs(clampingAngle) < (PI / 900))
    {
        rotate_v_r = 0;
        outdebug("fabs(rotate_PID_param[rob_i].err_r) < 0.01");
    }
    //���ݸ�ǰ���ٶȿ���
    rotate_PID_param[rob_i].v_r = rotate_v_r;

    //�������ֵ
    rotate_PID_param[rob_i].v_last_r = rotate_v_r;

    //������ٶ�ָ��
    send_cmd_rotate(rob_i, rotate_v_r);
    outdebug("�����˺ţ�", rob_i, "rotate_v_r=", rotate_v_r);
    return rotate_v_r;
}

/* �˶����ƣ��ܣ�*/
void move()
{
    //�˶�����
    for (int rob_i = 0; rob_i < 4; rob_i++)
    {
        // ����Ա״̬�Ĺ�����
        if ((state.robot[rob_i].attacker) && (attackerState[rob_i] != 0) && (attackerState[rob_i] != 6))
        {
            continue;
        }
        // ��·��
        if (path[rob_i].size() == 0)
        {
            if (cmd[rob_i].takeStation != -1)
            {
                send_cmd_forward(rob_i, -2);
                send_cmd_rotate(rob_i, 0);
                continue;
            }
            else
            {
                //forward_rotate_control(rob_i, state.robot[rob_i].p);
                //forward_rotate_control_APF(rob_i, state.robot[rob_i].p);
                send_cmd_forward(rob_i, 0);
                send_cmd_rotate(rob_i, 0);
                continue;
            }
        }
        // ·����һ����
        if (path[rob_i].size() == 1)
        {
            float p[2];
            if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker))
            {
                p[0] = col2x_chess(path[rob_i][0] % 101);
                p[1] = row2y_chess(path[rob_i][0] / 101);
                p[0] += 0.25;
                p[1] -= 0.25;
            }
            else if (cmd[rob_i].state == 2)
            {
                p[0] = col2x(path[rob_i][0] % 100);
                p[1] = row2y(path[rob_i][0] / 100);
            }
            //forward_rotate_control(rob_i, p);
            forward_rotate_control_APF(rob_i, p);
            continue;
        }
        //·����������
        if (path[rob_i].size() == 2)
        {
            float p[2];
            if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker))
            {
                p[0] = col2x_chess(path[rob_i][1] % 101);
                p[1] = row2y_chess(path[rob_i][1] / 101);
                p[0] += 0.25;
                p[1] -= 0.25;
            }
            else if (cmd[rob_i].state == 2)
            {
                p[0] = col2x(path[rob_i][1] % 100);
                p[1] = row2y(path[rob_i][1] / 100);
            }
            if ((state.robot[rob_i].attacker) && (attackerState[rob_i] == 0))
            {
                if (TEAM == 0)
                {
                    for (int i = 0; i < cornerEnemyStation.size(); i++)
                    {
                        if (cornerEnemyStation[i] == currentAttackEnemyStation[rob_i])
                        {
                            switch (cornerEnemyStationKind[i])
                            {
                            case 0://������ǽ��
                                p[0] += -1;
                                p[1] += 1;
                                break;
                            case 1://������ǽ��
                                p[0] += 1;
                                p[1] += 1;
                                break;
                            case 2://������ǽ��
                                p[0] += 1;
                                p[1] += -1;
                                break;
                            case 3://������ǽ��
                                p[0] += -1;
                                p[1] += -1;
                                break;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < cornerEnemyStation.size(); i++)
                    {
                        if (cornerEnemyStation[i] == currentAttackEnemyStation[rob_i])
                        {
                            switch (cornerEnemyStationKind[i])
                            {
                            case 0://������ǽ��
                                p[0] += 0.3;
                                p[1] += -0.3;
                                break;
                            case 1://������ǽ��
                                p[0] += -0.3;
                                p[1] += -0.3;
                                break;
                            case 2://������ǽ��
                                p[0] += -0.3;
                                p[1] += +0.3;
                                break;
                            case 3://������ǽ��
                                p[0] += +0.3;
                                p[1] += +0.3;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            outdebug("target:", path[rob_i][1], "���꣺", p[0], p[1]);

            forward_rotate_control_APF(rob_i, p);
            //if (state.robot[rob_i].attacker == true)
            //{
            //    forward_control_cash(rob_i, p);
            //}

            // ·�����е��ˣ�ֱ�ӳ�
            //if (state.robot[rob_i].attacker == false)
            {
                float theta = clamping_angle(rob_i, p);
                theta = (theta < 0) ? (-theta) : (2 * PI - theta);
                theta = round(theta / PI * 180);
                theta = (theta == 360) ? 0 : theta;
                int k = theta;
                if ((state.robot[rob_i].lasersObject[k] == 1) && (state.robot[rob_i].lasers[k] < 5))
                {
                    for (int emy_i = 0; emy_i < state.enemyRobot.size(); emy_i++)
                    {
                        if (sqrt(pow(state.robot[rob_i].p[0] - state.enemyRobot[emy_i].p[0], 2) + pow(state.robot[rob_i].p[1] - state.enemyRobot[emy_i].p[1], 2)) < 1.5)
                        {
                            send_cmd_forward(rob_i, 6 + (TEAM == 1));
                        }
                    }
                }
            }

            // Ŀ��վ�����е��ˣ��Ӵ�����ֱ�ӳ�
            if (state.robot[rob_i].attacker == false)
            {
                if (state.robot[rob_i].bring > 0)
                {
                    if (state.station[cmd[rob_i].sellStation].enemyOccupy > 0)
                    {
                        send_cmd_forward(rob_i, 6 + (TEAM == 1));
                    }
                }
                else
                {
                    if (state.station[cmd[rob_i].takeStation].enemyOccupy > 0)
                    {
                        send_cmd_forward(rob_i, 6 + (TEAM == 1));
                    }
                }
            }
            continue;
        }
        //·����һ�ѵ�
        float p[2];
        if ((cmd[rob_i].state == 1) || (state.robot[rob_i].attacker))
        {
            p[0] = col2x_chess(path[rob_i][1] % 101);
            p[1] = row2y_chess(path[rob_i][1] / 101);
        }
        else if (cmd[rob_i].state == 2)
        {
            p[0] = col2x(path[rob_i][1] % 100);
            p[1] = row2y(floor(path[rob_i][1] / 100));
        }
        forward_rotate_control_APF(rob_i, p);

        // ·�����е��ˣ�ֱ�ӳ�
        if (state.robot[rob_i].attacker == false)
        {
            float theta = clamping_angle(rob_i, p);
            theta = (theta < 0) ? (-theta) : (2 * PI - theta);
            theta = round(theta / PI * 180);
            theta = (theta == 360) ? 0 : theta;
            int k = theta;
            if ((state.robot[rob_i].lasersObject[k] == 1) && (state.robot[rob_i].lasers[k] < 1))
            {
                for (int emy_i = 0; emy_i < state.enemyRobot.size(); emy_i++)
                {
                    if (sqrt(pow(state.robot[rob_i].p[0] - state.enemyRobot[emy_i].p[0], 2) + pow(state.robot[rob_i].p[1] - state.enemyRobot[emy_i].p[1], 2)) < 1.5)
                    {
                        send_cmd_forward(rob_i, 6 + (TEAM == 1));
                    }
                }
            }
        }
    }
}



float forward_control_cash(int rob_i, float Target_Coord[2])
{
    float Map_Distance, Target_Distance, Map_Distance_D;
    float rob_v_max;
    float rob_v;
    float Acceleration_map;
    /*���Ȼ�ȡ��ǰ����������*/
    float Robot_Coord[2];
    /*Ȼ��ͨ��������Ŀ������*/
    float Target_Vector[2];
    float aa = 0.05;
    Robot_Coord[0] = state.robot[rob_i].p[0];
    Robot_Coord[1] = state.robot[rob_i].p[1];
    Target_Vector[0] = Target_Coord[0] - Robot_Coord[0];
    Target_Vector[1] = Target_Coord[1] - Robot_Coord[1];

    //���㵱ǰ�ٶ�
    //rob_v = sqrt(state.robot[rob_i].speed[0] * state.robot[rob_i].speed[0] + state.robot[rob_i].speed[1] * state.robot[rob_i].speed[1]);
    //�����Ŀ������
    Target_Distance = sqrt(Target_Vector[0] * Target_Vector[0] + Target_Vector[1] * Target_Vector[1]);

    if (state.robot[rob_i].bring == 0)//�жϻ�������û��Я����Ʒ
    {
        Acceleration_map = 19.6488;//ÿ��
    }
    else
    {
        Acceleration_map = 14.1056;//ÿ��
    }

    float clampingAngle = clamping_angle(rob_i, Target_Coord);
    outdebug("�����˺ţ�", rob_i, "ǰ�����clampingAngle=", clampingAngle);

    if (fabs(clampingAngle) < (PI * 1 / 8))
    {
        // if (Target_Distance >= 1.0)
        // {
        //     rob_v = 7;
        // }
        // if (Target_Distance <= 1.0)
        // {
        //     rob_v = sqrt(2 * Acceleration_map * (Target_Distance));
        // }
        // if (Target_Distance < 0.2)
        // {
        //     rob_v = 0.01;
        // }
        rob_v = 7;
    }
    else
    {
        rob_v = 3.5;
    }
    outdebug("�����˺ţ�", rob_i, "rob_v=", rob_v);

    send_cmd_forward(rob_i, rob_v);

    return rob_v;
}

