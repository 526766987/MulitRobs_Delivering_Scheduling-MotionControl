#pragma once
#include <fstream>
#include "Global.h"

using namespace std;

extern ofstream outfile0; //��������ļ���
extern ofstream outfile1; //�������ļ���

/*���������Ϣ���ļ� */
template<typename... Args>
static void outdebug(const Args&... args) {
    if(!outfile0.is_open()) {
		outfile0.open(FILE0);
	}
    if(!outfile1.is_open()) {
		outfile1.open(FILE1);
	}
    if(TEAM == 0) {
        outfile0 << "[BLUE TEAM] ";
        outfile0 << state.timeStamp << " : ";
        ((outfile0 << args << " "), ...);
        outfile0 << endl;
    } else if(TEAM == 1) {
        outfile1 << "[RED TEAM]  ";
        outfile1 << state.timeStamp << " : ";
        ((outfile1 << args << " "), ...);
        outfile1 << endl;
    }
}

/*���������Ϣ���ļ������� */
template<typename... Args>
static void outdebugline(const Args&... args) {
    if(!outfile0.is_open()) {
		outfile0.open(FILE0);
	}
    if(!outfile1.is_open()) {
		outfile1.open(FILE1);
	}
	((outfile1 << args << " "), ...);
}

/* ������ٶȵ���ָ�� */
void send_cmd_forward(int robot_id, float speed);

/* ������ٶȵ���ָ�� */
void send_cmd_rotate(int robot_id, float speed);

/* �������ָ�� */
void send_cmd_buy(int robot_id);

/* �������ָ�� */
void send_cmd_sell(int robot_id);

/* �������ָ�� */
void send_cmd_destroy(int robot_id);