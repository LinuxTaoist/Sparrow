/*
********************************************************************************
* Copyright (C) 2021, xiang.D <dx_65535@163.com>.
* All right reserved.
*
* File Name   : main_properties.cc
* Author      :
* Version     : V1.0
* Description :
* Journal     : 2023-09-17 init v1.0
* Brief       : Blog address: https://blog.csdn.net/qq_38750572?spm=1001.2014.3001.5343
* Others      :
********************************************************************************
*/
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "SharedMemory.h"

using namespace std;

#define SPR_LOGD(fmt, args...)  printf("%d Properties D: " fmt, __LINE__, ##args)

#define SHARED_MEMORY_PATH      "/tmp/__property_service__"
#define SHARED_MEMORY_MAX_SIZE  (5 * 1024 * 1024)

int main(int argc, char * argv[])
{
    SharedMemory theSharedMemory(SHARED_MEMORY_PATH, SHARED_MEMORY_MAX_SIZE);

    for (int i = 0; i < 10; i++)
    {
        string key = "key" + to_string(i);
        string value = "value" + to_string(i);
        theSharedMemory.SetValue(key, value);
        SPR_LOGD("Set: key = %s, value = %s\n", key.c_str(), value.c_str());
    }

    for (int i = 0; i < 10; i++)
    {
        string value;
        string key = "key" + to_string(i);
        theSharedMemory.GetValue(key, value);
        SPR_LOGD("Get: key = %s, value = %s\n", key.c_str(), value.c_str());
    }

    return 0;
}
