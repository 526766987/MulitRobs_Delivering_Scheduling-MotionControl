#pragma once

/*--------------����任-----------------*/

// ��������ĵڼ�������������
float col2x(int col);

// ��������ĵڼ�������������
float row2y(int row);

// ���������������ڼ���
int x2col(float x);

// ���������������ڼ���
int y2row(float y);

// ��������ĵڼ�������������
int x2col_chess(float x);

// ��������ĵڼ�������������
int y2row_chess(float y);

// ���������������ڼ���
float col2x_chess(int col);

// ���������������ڼ���
float row2y_chess(int row);

/*--------------�������-----------------*/

// �����پ���
float manhattan_distance(int p1_i, int p1_j, int p2_i, int p2_j);

// ������������ľ�������ͼ��Ե�ľ���
float cal_map_distance_d(float robot_coor[2]);

// �ص�ǰ���򵽵�ͼ��Ե�ľ���
float cal_map_distance(int rob_i);

// Ŀ��Ƕ��뵱ǰ����н�
float clamping_angle(int rob_i, float Target_Coord[2]);

// �°��ۺ�·�̼���
float new_dist_cal(float distr2b, float distb2s, float theta);

/*--------------��������-----------------*/

// ����ÿ�ֲ�Ʒ��ȫ�ּ�ֵ��G �� (1 - ��^-k) �� ��
float cal_productValue(int type);

// ���ڼ���{ʱ���ֵϵ��}��{��ײ��ֵϵ��}�ĺ���f
float f_func(float x, float maxX, float minRate);

// �Ƿ����ĳ��Ʒ
bool pro_exsit(int type, int materials);

// �ۺ��������
float income_cal(int takeS, int sellS);

// �°��ۺ��������
float new_income_cal(int takeS, int sellS, float s2spriorty);
