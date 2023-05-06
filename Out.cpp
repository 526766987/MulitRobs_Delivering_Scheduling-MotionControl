#include "Out.h"
#include <fstream>
#include "Global.h"
using namespace std;

ofstream outfile0;
ofstream outfile1;

/* ������ٶȵ���ָ�� */
void send_cmd_forward(int robot_id, float speed) {
    if (robot_id >= 0 && robot_id < 4 && speed >= -2 && speed <= 7) {
        if (TEAM == 0)
        {
            speed = (speed > 6) ? 6 : speed;
        }
        else
        {
            speed = (speed > 7) ? 7 : speed;
        }
        printf("forward %d %f\n", robot_id, speed);
    }
}

/* ������ٶȵ���ָ�� */
void send_cmd_rotate(int robot_id, float speed) {
    if (robot_id >= 0 && robot_id < 4 && speed >= -PI && speed <= PI) {
        printf("rotate %d %f\n", robot_id, speed);
    }
}

/* �������ָ�� */
void send_cmd_buy(int robot_id) {
    if (robot_id >= 0 && robot_id < 4) {
        printf("buy %d\n", robot_id);
    }
}

/* �������ָ�� */
void send_cmd_sell(int robot_id) {
    if (robot_id >= 0 && robot_id < 4) {
        printf("sell %d\n", robot_id);
    }
}

/* �������ָ�� */
void send_cmd_destroy(int robot_id) {
    if (robot_id >= 0 && robot_id < 4) {
        printf("destroy %d\n", robot_id);
    }
}