#pragma once

/*���γ���*/
static bool wallCorner[5] = { true,true,true,true,true }; //ǽ���Ƿ���չ�����expandWallͼ�ĵ���

/*ȫ�̶ֹ�ֵ����*/
extern int MAPKIND; //��ͼ���� 1,2,3,4
extern bool wallMap[100][100]; //ǽ��ͼ
extern bool expandWall[100][100]; // ǽ����չ��ĵ�ͼ(����̬ʹ��)
extern bool chessBoard[101][101]; // Χ���ͼ(ȡ��̬ʹ��)

/*��������*/
void map_identify(); //��ͼʶ��
void wall_growth(bool cornerExpandSwitch); //����expandWall��ͼ
void chess_board_creat(); //����chessBoard��ͼ
bool cross(float p1[2], float p2[2], float p3[2], float p4[2]); //�����ж�����������1��3��