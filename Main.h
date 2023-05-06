#pragma once
#include <vector>
using namespace std;

// ��ͼר�ò�����
struct MapParams
{
    float L2; //��������ֵ����
    float L3; //��������ֵ����
    float SPEED; //��ʵ����ٶ�ϵ��
    float BETA; //����ͷ�Ӱ������
    float OMEGA; //ԭ�Ͻ�ȱ�̶�Ӱ������
    float ETA; //��ͷ�ͷ�Ӱ������
    float ALPHA; //��Ʒȫ�ּ�ֵϵ��
    float MIU; //��Ʒȫ�ּ�ֵӰ������
};

// ����վ��Ϣ
struct StationType
{
    int type;   //����&��Ʒ����
    int needMaterials;  //�����䷽0b00000000 7654321*
    int buyMaterials;   //�չ�����0b00000000 7654321*
    int period; //��������
    int cost;   //�����
    int sell;   //ԭʼ���ۼ�
    int gain;   //�����Ͳ�Ʒ�Ĳ�ۣ�������
    int quantity;  //���๤��̨����
    float productValue; //���๤��̨��Ʒ�ļ�ֵ
};

// ����վ״̬
struct StationState
{
    int type;   //����
    float p[2]; //����
    int inBusy; //ʣ������ʱ�� -1û��������0���������+ʣ��ʱ��
    int materials;  //ԭ���ϸ�״̬0b0000000 7654321*
    bool finish;    //��Ʒ��״̬
    int p_ij[2]; //����λ��
    int enemyOccupy; //�Ƿ񱻵з�ռס��
    bool beenOccupy; //��
};

// �з�����վ��Ϣ
struct StationEnemy
{
    int type;   //����
    float p[2]; //����
    int p_ij[2]; //����λ��
};


// ������״̬
struct RobotState
{
    int atStation;  //��������̨id   -1û�й���̨��0~K-1����̨�±�
    int bring;  //Я��������
    float timeValue;    //ʱ���ֵϵ��
    float collisionValue;   //��ײ��ֵϵ��
    float angularSpeed; //���ٶ�
    float speed[2]; //���ٶ�
    float direction;    //����
    float p[2]; //����
    int p_ij_init[2]; //��ʼ����λ��
    float lasers[360]; //ԭʼ�ļ����״�ӻ����˳�����ʱ��˳���� 360 ���������ߣ�ȡÿ�����ߵ�����䳤��
    float lasersMapDirection[360]; //ԭʼ�ļ����״��Ӧ�±�Ļ��ڵ�ͼ�ĽǶ�
    float sumLaser; //�״���ֵ��
    float maxLaser_i; //����������±�
    float minLaser_i; //��̼��������±�
    int lasersObject[360]; //ÿ��������̽�⵽�����壬0ǽ�ڣ�1������
    bool attacker; //�Ƿ�Ϊ������
};

// �з�������״̬
struct EnemyState
{
    float p[2];
    int outTime;
};

// ֡״̬
struct State
{
    int timeStamp;  //ʱ���
    int cash;   //���
    int K;  //����վ����
    int eK; //�з�����վ����
    StationState station[50];  //����վ״̬
    StationEnemy enemyStation[50]; //�з�����վ��Ϣ
    RobotState robot[4];    //������״̬
    vector<EnemyState> enemyRobot; //�з�������״̬
};