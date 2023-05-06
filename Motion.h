#pragma once

// PID�����ṹ��
struct Rotate_PID_param
{
    float err_r;             		//����ƫ��ֵ
    float err_last_r;          		//������һ��ƫ��ֵ
    float Kp_r, Ki_r, Kd_r;         //�������״̬�±��������֡�΢��ϵ��
    float integral_r;          		//�������ֵ
    float v_r;             		    //�����ٶ�ֵ,�༴������
    float v_last_r;          		//������һ���ٶ�ֵ
    float old_target[2];
};

// PID����ʵ������
extern Rotate_PID_param rotate_PID_param[4];

// Ŀ��Ƕ��뵱ǰ����н�
extern float clampingAngle[4];

// PID������ʼ��
void rotate_PID_init(int rob_i);

// ����·���������ָ��
void motion_by_path();

// �˹��Ƴ��������˶�����core
void forward_rotate_control_APF(int rob_i, float Target_Coord[2]);

// ���ٶȿ�����
float forward_control(int rob_i, float Target_Coord[2]);

// ���ٶȿ�����
float rotate_control(int rob_i, float Target_Coord[2]);

// �˶�����(��)
void move();

// ��ײ����
float forward_control_cash(int rob_i, float Target_Coord[2]);

void crash_motion_control(int rob_i, float defensiveCoor[2]);