//
//  main.cpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/27.
//

#include <iostream>
#include <vector>
#include "Task.hpp"
#include "Cloud.hpp"
#include "LocalCore.hpp"
#include "Operation.hpp"
using namespace std;
int main() {
    //任务数量
    //cout << "task_number:";
    int task_number;
    cin >> task_number;
    //本地线程数
    //cout << "core_number:";
    int core_number;
    cin >> core_number;
    int time_consumption;
    //每一个任务在每个线程上的时间消耗
    //cout << "Time consumption on each core:" << endl;
    vector <Task*> task_list;
    Task *t = new Task();
    task_list.push_back(t);
    //Task task_list[task_number+1];
    for (int i = 1; i <= task_number; i++){
        Task *t = new Task(i);
        t->set_time_local(0);
        for (int j = 0; j < core_number; j++){
            cin >> time_consumption;
            t->set_time_local(time_consumption);
        }
        //t->Set_primary_local_or_cloud();
        //t->set_wi();
        task_list.push_back(t);
    }
    int sending_time, process_time, receive_time; //定义cloud运行的发送/处理/接收时间
    //cout << "sending_time, process_time, receive_time:";
    cin >> sending_time >> process_time >> receive_time;
    for (int i = 1; i < task_list.size(); i++){
        task_list[i]->set_time_send(sending_time);
        task_list[i]->set_time_cloud(process_time);
        task_list[i]->set_time_receive(receive_time);
        task_list[i]->Set_primary_local_or_cloud();
        task_list[i]->set_wi();
    }
    //边的数量
    int edge_number;
    //cout << "edge_number:";
    cin >> edge_number;
    //前置和后继任务节点
    //cout << "pre and succ" << endl;
    int temp_start,temp_end;
    for (int i = 0; i < edge_number; i++){
        cin >> temp_start >> temp_end;
        task_list[temp_start]->set_succ(task_list[temp_end]);
        task_list[temp_end]->set_pre(task_list[temp_start]);
    }
    //第i个线程上的能量消耗
    //cout << "power:" << endl;
    vector <double> power;
    double power_temp;
    cin >> power_temp;
    power.push_back(power_temp);
    //double power[core_number+1];
    //cin >> power[0]; //send cloud power
    for (int i = 1; i <= core_number; i++){
        cin >> power_temp;
        power.push_back(power_temp);
    }
    //设置一个数组用于Operation类中的线程数量
    vector<LocalCore*> core;
    LocalCore *localcore = new LocalCore(0);
    core.push_back(localcore);
    for (int i = 1; i <= core_number; i++){
        //maybe bug
        LocalCore *localcore = new LocalCore(i);
        core.push_back(localcore);
    }
    Operation opr = Operation(core_number, core, task_list, power, sending_time, process_time, receive_time);
    cout << "============"  << endl;
    vector <LocalCore*> v;
    v = opr.get_core();
    cout << "final schedule:" << endl;
    for (int i = 1; i < opr.get_current_task_list().size(); i++){
        cout << opr.get_current_task_list()[i]->get_id() << " ";
    }
    cout << endl;
    for (int i = 1; i <= core_number; i++){
        cout << "Core" << i << ":   ";
        int start_time = 0;
        for (int j = 0; j < v[i]->get_current_local_task_list().size(); j++){
            while(start_time < v[i]->get_current_local_task_list()[j]->get_start_time_local()){
                cout << "_ ";
                start_time++;
            }
            while(start_time < v[i]->get_current_local_task_list()[j]->get_finish_time_local()){
                cout << v[i]->get_current_local_task_list()[j]->get_id() << " ";
                start_time++;
            }
        }
        cout << endl;
    }
    cout << "send:    ";
    int start_time = 0;
    for (int i = 0; i < opr.get_cloud().get_current_cloud_task_list().size(); i++){
        while(start_time < opr.get_cloud().get_current_cloud_task_list()[i]->get_finish_time_send() - sending_time){
            cout << "_ ";
            start_time++;
        }
        while(start_time < opr.get_cloud().get_current_cloud_task_list()[i]->get_finish_time_send()){
            cout << opr.get_cloud().get_current_cloud_task_list()[i]->get_id() << " ";
            start_time++;
        }
    }
    cout << endl;
    cout << "cloud:   ";
    start_time = 0;
    for (int i = 0; i < opr.get_cloud().get_current_cloud_task_list().size(); i++){
        while(start_time < opr.get_cloud().get_current_cloud_task_list()[i]->get_finish_time_cloud() - process_time){
            cout << "_ ";
            start_time++;
        }
        while(start_time < opr.get_cloud().get_current_cloud_task_list()[i]->get_finish_time_cloud()){
            cout << opr.get_cloud().get_current_cloud_task_list()[i]->get_id() << " ";
            start_time++;
        }
    }
    cout << endl;
    cout << "receive: ";
    start_time = 0;
    for (int i = 0; i < opr.get_cloud().get_current_cloud_task_list().size(); i++){
        while(start_time < opr.get_cloud().get_current_cloud_task_list()[i]->get_finish_time_receive() - receive_time){
            cout << "_ ";
            start_time++;
        }
        while(start_time < opr.get_cloud().get_current_cloud_task_list()[i]->get_finish_time_receive()){
            cout << opr.get_cloud().get_current_cloud_task_list()[i]->get_id() << " ";
            start_time++;
        }
    }
    cout << endl;
    cout << "time: " << opr.get_time() << endl;
    cout << "energy: " << opr.get_enerty() << endl;
    //cout << opr.get_time() << endl;
    return 0;
}
/*
 10
 3
 9 7 5
 8 6 5
 6 5 4
 7 5 3
 5 4 2
 7 6 4
 8 5 3
 6 4 2
 5 3 2
 7 4 2
 3 1 1
 15
 1 2
 1 3
 1 4
 1 5
 1 6
 2 8
 2 9
 3 7
 4 8
 4 9
 5 9
 6 8
 7 10
 8 10
 9 10
 0.5 1 2 4
 
 */
