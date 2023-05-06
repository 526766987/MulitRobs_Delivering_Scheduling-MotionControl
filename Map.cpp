#include "Map.h"
#include "Global.h"
#include "Main.h"
#include "Path.h"
#include "Attack.h"
using namespace std;


int MAPKIND = 0; //��ͼ���ͳ�ʼ����ΪĬ��ֵ
bool wallMap[100][100] = { false }; // ǽ��ͼ
bool expandWall[100][100] = { false }; // ǽ����չ��ĵ�ͼ
bool chessBoard[101][101] = { false }; // Χ���ͼ


/*ʶ���Ƿ�Ϊ2��ͼ*/
bool map2_identify()
{
	// Map 2
	//��Կ����ĵ��Σ����Ե���Դ�㶼�ȽϷḻ����ͼ���ָ�Ϊ��������ͨ��������һ������Ϊ��ɫ���أ�ֻ�к�ɫ����̨�������˳�ʼΪ3��1������һ������Ϊ��ɫ���أ�ֻ����ɫ����̨�������˳�ʼΪ3��1�졣
	//���������ζ��˫��������һ����������ȫ����Ҫ����������Ʒ��ֻ��Ҫ��Ϊ�Է����ƶ��ϰ���
	
	//ע���ù���̨��ͨ��ȥʶ���������׵�
	
	bool attacker = false;
	int reachOurStationAmount[4] = { 0 };
	int reachEnemyStationAmount[4] = { 0 };
	for (int rob_i = 0; rob_i < 4; rob_i++)
	{
		for (int sta_i = 0; sta_i < state.K; sta_i++)
		{
			reachOurStationAmount[rob_i] += reachStation[1][rob_i][sta_i];
		}
		for (int sta_i = 0; sta_i < state.eK; sta_i++)
		{
			reachEnemyStationAmount[rob_i] += reachEnemyStation[rob_i][sta_i];
		}

		if ((reachOurStationAmount[rob_i] == state.K) && (reachEnemyStationAmount[rob_i] == 0))
		{
			// ������
		}
		else if ((reachOurStationAmount[rob_i] == 0) && (reachEnemyStationAmount[rob_i] == state.eK))
		{
			// attacker
			if (attacker == false) { attacker = true; } //�ҵ���attacker
			else { return false; } //map2�����ܻ���ֵڶ���attacker
		}
		else
		{
			//���Բ���ͼ2
			return false;
		}
	}
	return true;
}

/*ʶ���Ƿ�Ϊ4��ͼ*/
bool map4_identify()
{
	// Map 4
	//��Ϊ�����ĵ��Σ����ŵ�ͼ��ȫû����ɫ����̨��ֻ�к�ɫ����̨���Һ�ɫ����̨��ȽϷḻ��
	//����� ����ζ��������ȫ����Ҫ�������䣬ֻ��Ҫ��Ϊ�췽���ƶ��ϰ���

	//ע���ù���̨����ʶ��

	if ((state.K == 0) || (state.eK == 0))
	{
		return true;
	}
	return false;
}

/*ʶ���Ƿ���ͼ1������Ϊͼ3*/
bool cross(float p1[2], float p2[2], float p3[2], float p4[2])
{
    bool flag = true;
    if ((p1[0] > p2[0] ? p1[0] : p2[0]) < (p3[0] < p4[0] ? p3[0] : p4[0]) ||
        (p1[1] > p2[1] ? p1[1] : p2[1]) < (p3[1] < p4[1] ? p3[1] : p4[1]) ||
        (p3[0] > p4[0] ? p3[0] : p4[0]) < (p1[0] < p2[0] ? p1[0] : p2[0]) ||
        (p3[1] > p4[1] ? p3[1] : p4[1]) < (p1[1] < p2[1] ? p1[1] : p2[1]))
    {
        flag = false;
    }
    if ((((p1[0] - p3[0]) * (p4[1] - p3[1]) - (p1[1] - p3[1]) * (p4[0] - p3[0])) *
        ((p2[0] - p3[0]) * (p4[1] - p3[1]) - (p2[1] - p3[1]) * (p4[0] - p3[0]))) > 0 ||
        (((p3[0] - p1[0]) * (p2[1] - p1[1]) - (p3[1] - p1[1]) * (p2[0] - p1[0])) *
            ((p4[0] - p1[0]) * (p2[1] - p1[1]) - (p4[1] - p1[1]) * (p2[0] - p1[0]))) > 0)
    {
        flag = false;
    }

    return flag;
}

bool map1_not3_identify()
{
	//Map 1
	//��Կ����ĵ��Σ����Ե���Դ�㶼�ȽϷḻ��˫������·�����ڽ������ο���ϰ��ͼ1�������θ���������Դ����ḻ��
	//�������ȫ�ޱ��õĻ�������໥��ײ������ζ��˫�����н���������Ҫ���ݳ���״̬�������Ƿ���á�

	//Map 3
	//�����խ�ĵ��Σ�˫���и��Զ������أ�����֮����ڶ���·����ͨ�����һ����ⲿ����һЩ��ɢ�ĺ�������̨��ʹ�á����ο���ϰ��ͼ2��
	//�����˫����ѡ����Է�չ�����߸��ŶԷ��ƶ�������ռ��һЩ�ؼ���λ��

	//������
	//���ڵ��ο���/��խ���ж��ǲ��ɿ���
	//����ʹ������·���������ж�

	//������
	//��123��λ��ƽ����p1��456��λ��ƽ����p2
	//�ҷ�p1->p2��з�p1->p2���ڽ��棬��Ϊmap 1

	float p123Our[2], p456Our[2], p7Our[2], p123Enemy[2], p456Enemy[2], p7Enemy[2] = {0};
	int count123Our, count456Our, count7Our, count123Enemy, count456Enemy, count7Enemy = 0;
	for (int sta_i = 0; sta_i < state.K; sta_i++)
	{
		if (state.station[sta_i].type <= 3)
		{
			p123Our[0] += state.station[sta_i].p[0];
			p123Our[1] += state.station[sta_i].p[1];
			count123Our++;
		}
		else if (state.station[sta_i].type <= 6)
		{
			p456Our[0] += state.station[sta_i].p[0];
			p456Our[1] += state.station[sta_i].p[1];
			count456Our++;
		}
        else if (state.station[sta_i].type == 7)
        {
            p7Our[0] += state.station[sta_i].p[0];
            p7Our[1] += state.station[sta_i].p[1];
            count7Our++;
        }
	}

	p123Our[0] /= count123Our;
	p123Our[1] /= count123Our;
	p456Our[0] /= count456Our;
	p456Our[1] /= count456Our;
    if (count7Our > 0)
    {
        p7Our[0] /= count7Our;
        p7Our[1] /= count7Our;
    }

	for (int sta_i = 0; sta_i < state.eK; sta_i++)
	{
		if (state.enemyStation[sta_i].type <= 3)
		{
			p123Enemy[0] += state.enemyStation[sta_i].p[0];
			p123Enemy[1] += state.enemyStation[sta_i].p[1];
			count123Enemy++;
		}
		else if (state.enemyStation[sta_i].type <= 6)
		{
			p456Enemy[0] += state.enemyStation[sta_i].p[0];
			p456Enemy[1] += state.enemyStation[sta_i].p[1];
			count456Enemy++;
		}
        else if (state.enemyStation[sta_i].type == 7)
        {
            p7Enemy[0] += state.enemyStation[sta_i].p[0];
            p7Enemy[1] += state.enemyStation[sta_i].p[1];
            count7Enemy++;
        }
	}

	p123Enemy[0] /= count123Enemy;
	p123Enemy[1] /= count123Enemy;
    p456Enemy[0] /= count456Enemy;
    p456Enemy[1] /= count456Enemy;
    if (count7Enemy > 0)
    {
        p7Enemy[0] /= count7Enemy;
        p7Enemy[1] /= count7Enemy;
    }
    

	// �����ཻ
	// p123Our[2], p456Our[2], p123Enemy[2], p456Enemy[2]
    bool flag = true;
    flag |= cross(p123Our, p456Our, p123Enemy, p456Enemy);
    if ((count7Our > 0) && (count7Enemy > 0))
    {
        flag |= cross(p456Our, p7Our, p456Enemy, p7Enemy);
    }
    return flag;
	
}



/*��ͼʶ��*/
void map_identify()
{
	if (map4_identify())
	{
		MAPKIND = 4;
	}
	else if (map2_identify())
	{
		MAPKIND = 2;
	}
	else if (map1_not3_identify())
	{
		MAPKIND = 1;
	}
	else
	{
		MAPKIND = 3;
	}
}

/* ����expandWall��ͼ */
void wall_growth(bool cornerExpandSwitch)
{
    //�ı߽�
    for (int i = 0; i < 100; i++)
    {
        expandWall[0][i] = true;
        expandWall[99][i] = true;
        expandWall[i][0] = true;
        expandWall[i][99] = true;
    }

    //���
    int i = 0;
    for (int j = 1; j < 99; j++)
    {
        if (wallMap[i][j])
        {
            expandWall[i][j] = true;
            expandWall[i + 1][j] = true;
            expandWall[i][j + 1] = true;
            expandWall[i][j - 1] = true;
            if (cornerExpandSwitch == true)
            {
                expandWall[i + 1][j + 1] = true;
                expandWall[i + 1][j - 1] = true;
            }
        }
    }

    //�ұ�
    i = 99;
    for (int j = 1; j < 99; j++)
    {
        if (wallMap[i][j])
        {
            expandWall[i][j] = true;
            expandWall[i - 1][j] = true;
            expandWall[i][j + 1] = true;
            expandWall[i][j - 1] = true;
            if (cornerExpandSwitch == true)
            {
                expandWall[i - 1][j + 1] = true;
                expandWall[i - 1][j - 1] = true;
            }
        }
    }

    //�ϱ�
    int j = 0;
    for (int i = 1; i < 99; i++)
    {
        if (wallMap[i][j])
        {
            expandWall[i][j] = true;
            expandWall[i + 1][j] = true;
            expandWall[i - 1][j] = true;
            expandWall[i][j + 1] = true;
            if (cornerExpandSwitch == true)
            {
                expandWall[i + 1][j + 1] = true;
                expandWall[i - 1][j + 1] = true;
            }
        }
    }

    //�±�
    j = 99;
    for (int i = 1; i < 99; i++)
    {
        if (wallMap[i][j])
        {
            expandWall[i][j] = true;
            expandWall[i + 1][j] = true;
            expandWall[i - 1][j] = true;
            expandWall[i][j - 1] = true;
            if (cornerExpandSwitch == true)
            {
                expandWall[i + 1][j - 1] = true;
                expandWall[i - 1][j - 1] = true;
            }
        }
    }

    //���Ͻ�
    i = 0; j = 0;
    if (wallMap[i][j])
    {
        expandWall[i][j] = true;
        expandWall[i + 1][j] = true;
        expandWall[i][j + 1] = true;
        if (cornerExpandSwitch == true)
        {
            expandWall[i + 1][j + 1] = true;
        }
    }

    //���Ͻ�
    i = 99; j = 0;
    if (wallMap[i][j])
    {
        expandWall[i][j] = true;
        expandWall[i - 1][j] = true;
        expandWall[i][j + 1] = true;
        if (cornerExpandSwitch == true)
        {
            expandWall[i - 1][j + 1] = true;
        }
    }

    //���½�
    i = 0; j = 99;
    if (wallMap[i][j])
    {
        expandWall[i][j] = true;
        expandWall[i + 1][j] = true;
        expandWall[i][j - 1] = true;
        if (cornerExpandSwitch == true)
        {
            expandWall[i + 1][j - 1] = true;
        }
    }

    //���½�
    i = 99; j = 99;
    if (wallMap[i][j])
    {
        expandWall[i][j] = true;
        expandWall[i - 1][j] = true;
        expandWall[i][j - 1] = true;
        if (cornerExpandSwitch == true)
        {
            expandWall[i - 1][j - 1] = true;
        }
    }

    //��������
    for (int i = 1; i < 99; i++)
    {
        for (int j = 1; j < 99; j++)
        {
            if (wallMap[i][j])
            {
                expandWall[i][j] = true;
                expandWall[i + 1][j] = true;
                expandWall[i - 1][j] = true;
                expandWall[i][j + 1] = true;
                expandWall[i][j - 1] = true;
                if (cornerExpandSwitch == true)
                {
                    expandWall[i + 1][j + 1] = true;
                    expandWall[i - 1][j + 1] = true;
                    expandWall[i + 1][j - 1] = true;
                    expandWall[i - 1][j - 1] = true;
                }
            }
        }
    }
}

/* ����chessBoard��ͼ */
void chess_board_creat()
{
    for (int i = 0; i < 101; i++)
    {
        for (int j = 0; j < 101; j++)
        {
            //ǽ����i,j
            //��Ӧԭ�����ϵ��ĸ����ڸ�����(i,j)(i-1,j)(i,j-1)(i-1,j-1)
            if ((i == 0) || (j == 0) || (i == 100) || (j == 100))
            {
                chessBoard[i][j] = { true };
            }
            else if ((wallMap[i][j] == true) || (wallMap[i - 1][j] == true) || (wallMap[i][j - 1] == true) || (wallMap[i - 1][j - 1] == true))
            {
                chessBoard[i][j] = { true };
            }
        }
    }
}

