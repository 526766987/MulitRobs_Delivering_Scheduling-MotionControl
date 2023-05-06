#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <random>
#include <numeric>
#include <functional>
#include "Global.h"
#include "Main.h"
#include "Map.h"
#include "Calculate.h"
#include "Schedule.h"
#include "Motion.h"
#include "Attack.h"
#include "Out.h"
#include "Path.h"
#include "Defend.h"

using namespace std;

// ���ֲ�����ʼ����
void init_param();

// ��ȡ��ʼ����ͼ��Ϣ
bool readMapUntilOK();

// ��ȡ֡��Ϣ
bool readUntilOK();