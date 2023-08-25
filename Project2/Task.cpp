//
//  Task.cpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/27.
//
#include <iostream>
#include "Task.hpp"
using namespace std;
void Task::Set_primary_local_or_cloud(){
    //计算Ti_local_min 并与T_cloud_total比较，判断任务初始是在本地还是云端
    double time_local_min = 999999;
    for (int i = 1; i < time_local.size(); i++){
        //cout <<"---" << time_local[i];
        time_local_min = min(time_local_min, time_local[i]);
        //cout << "\n===" << time_local_min << endl;
    }
    double time_cloud_total = time_send + time_cloud + time_receive;
    //cout << "$$$$$$" << time_cloud_total << endl;
    if (time_local_min > time_cloud_total){
        cloud_or_local = 0; //cloud
    }
    else{
        cloud_or_local = 1; //local
    }
    //cout << "*****" << cloud_or_local << endl;
}

void Task::set_wi(){
    if (cloud_or_local == 0){
        wi = time_send + time_cloud + time_receive;
    }
    else{
        int total_wi = 0;
        for (int i = 1; i < time_local.size(); i++){
            total_wi += time_local[i];
        }
        wi = (double)(total_wi / (double)(time_local.size()-1));
    }
    priority = wi;
}

