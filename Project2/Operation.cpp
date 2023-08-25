//
//  Operation.cpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/29.
//

#include <iostream>
#include <vector>
#include <stack>
#include <ctime>
#include <time.h>
#include <chrono>
#include <ratio>
#include "Operation.hpp"
using namespace std::chrono;
Operation::Operation(int n, vector<LocalCore*>& core, vector<Task*> &task_list, vector<double>& power, int sending_time, int process_time, int receive_time){
    core_number = n;
    this->core = core;
    this->task_list = task_list;
    this->power = power;
    this->sending_time = sending_time;
    this->process_time = process_time;
    this->receive_time = receive_time;
    this->current_task_list = this->task_list;
    this->cloud = Cloud(sending_time, process_time, receive_time);
    for (int i = 1; i < task_list.size(); i++){
        cout << task_list[i]->get_cloud_or_local() << " ";
    }
    cout << endl;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
    task_prioritizing();
    
    task_migration();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double,std::ratio<1,1000>> duration_mcs = duration_cast<duration<double, std::ratio<1,1000>>>(t2 - t1);
    std::cout << "Running time of task prioritizing: " << duration_mcs.count() << "ms" << std::endl;
}

bool Operation::cmp(Task* &a, Task* &b){
    return a->get_priority() > b->get_priority();
}

void Operation::calculate_priority(int t){
    if (current_task_list[t]->get_pre().size() == 0){
        return ;
    }
    for (int i = 0; i < current_task_list[t]->get_pre().size(); i++){
        current_task_list[t]->get_pre()[i]->set_priority(max(current_task_list[t]->get_pre()[i]->get_priority(), current_task_list[t]->get_pre()[i]->get_wi() + current_task_list[t]->get_priority()));
        calculate_priority(current_task_list[t]->get_pre()[i]->get_id());
    }
}

void Operation::task_prioritizing(){
    for (int i = 1; i < task_list.size(); i++){
        if (task_list[i]->get_succ().size() == 0){
            task_list[i]->set_exit_task(1);
            exit_task_list.push_back(i);
        }
        else if (task_list[i]->get_pre().size() == 0){
            task_list[i]->set_entrance_task(1);
        }
    }
    for (int i = 0; i < exit_task_list.size(); i++){
        calculate_priority(exit_task_list[i]);
    }
    
    
    sort(current_task_list.begin()+1, current_task_list.end(), cmp);
    for (int i = 1; i < current_task_list.size(); i++){
        cout << current_task_list[i]->get_priority() << " ";
    }
    cout << endl;
    for (int i = 1; i < current_task_list.size(); i++){
        cout<< current_task_list[i]->get_cloud_or_local() << " ";
    }
    cout << endl;
    for (int i = 1; i < current_task_list.size(); i++){
        if (current_task_list[i]->get_cloud_or_local() == 0){ //cloud task
            double maxx1 = 0;
            double maxx2 = 0;
            for (int j = 0; j < current_task_list[i]->get_pre().size(); j++){
                maxx2 = max(current_task_list[i]->get_pre()[j]->get_finish_time_local(), current_task_list[i]->get_pre()[j] -> get_finish_time_send());
                maxx1 = max(maxx1, maxx2);
            }
            
            //current_task_list[i]->set_time_send(maxx1); //设置上传时间(在这个时间点开始上传，上传完成后，开始执行
            //current_task_list[i]->set_exe_on_core_number(0);
            current_task_list[i]->set_ready_time_send(maxx1);
            maxx1 = max(maxx1, cloud.get_finish_sending_time());
            current_task_list[i]->set_finish_time_send(maxx1 + sending_time); //设置开始执行的时间
            current_task_list[i]->set_finish_time_cloud(maxx1 + sending_time + process_time); //设置任务执行完的时间
            current_task_list[i]->set_finish_time_receive(maxx1 + sending_time + process_time + receive_time); //设置收到结果的时间 即该任务整个完成
            cloud.set_current_cloud_task_list(current_task_list[i]);
            cloud.set_finish_sending_time(maxx1 + sending_time);
            cloud.set_finish_time(maxx1 + sending_time + process_time + receive_time); //不一定有用
        }
        else if(current_task_list[i]->get_cloud_or_local() == 1){ //local task
            double ready_time_on_local = 0; //所有本地core上可以开始的最早时间
            double maxx1 = 0;
            for (int j = 0; j < current_task_list[i]->get_pre().size(); j++){
                maxx1 = max(current_task_list[i]->get_pre()[j]->get_finish_time_local(), current_task_list[i]->get_pre()[j] -> get_finish_time_receive());
                ready_time_on_local = max(maxx1, ready_time_on_local);
            }
            double ready_time_on_cloud = 0;
            maxx1 = 0;
            for (int j = 0; j < current_task_list[i]->get_pre().size(); j++){
                maxx1 = max(current_task_list[i]->get_pre()[j]->get_finish_time_local(), current_task_list[i]->get_pre()[j] -> get_finish_time_send());
                ready_time_on_cloud = max(maxx1, ready_time_on_cloud);
            }
            double finish_time = 99999999;
            double temp_finish_time;
            int exe_core_num = 1; //记录在哪个core上运行
            for (int j = 1; j <= core_number; j++){ //local
                temp_finish_time = core[j]->get_finish_time()>ready_time_on_local?core[j]->get_finish_time():ready_time_on_local;
                temp_finish_time += current_task_list[i]->get_time_local(j); //获取任务i在core j上的运行时间 += finish time
                if (temp_finish_time < finish_time){
                    finish_time = temp_finish_time;
                    exe_core_num = j;
                }
            }
            if (ready_time_on_cloud < cloud.get_finish_sending_time()){ //此时cloud上还有其他任务在执行
                ready_time_on_cloud = cloud.get_finish_sending_time();
            }
            if (ready_time_on_cloud + sending_time + process_time + receive_time < finish_time){ //cloud time < local time->切换到cloud运行
                current_task_list[i]->set_cloud_or_local(0);
                current_task_list[i]->set_ready_time_send(ready_time_on_cloud);
                current_task_list[i]->set_finish_time_send(ready_time_on_cloud + sending_time);
                current_task_list[i]->set_finish_time_cloud(ready_time_on_cloud + sending_time + process_time);
                current_task_list[i]->set_finish_time_receive(ready_time_on_cloud + sending_time + process_time + receive_time);
                cloud.set_current_cloud_task_list(current_task_list[i]);
                cloud.set_finish_sending_time(ready_time_on_cloud + sending_time);
                cloud.set_finish_time(ready_time_on_cloud + sending_time + process_time + receive_time); //不一定有用
            }
            else{
                current_task_list[i]->set_exe_on_core_number(exe_core_num);
                current_task_list[i]->set_ready_time_local(ready_time_on_local);
                current_task_list[i]->set_start_time_local(finish_time-current_task_list[i]->get_time_local(exe_core_num));
                current_task_list[i]->set_finish_time_local(finish_time);
                core[exe_core_num]->set_finish_time(finish_time);
                core[exe_core_num]->set_current_local_task_list(current_task_list[i]);
            }
        }
    }
    double total_energy = 0;
    for (int i = 1; i <= core_number; i++){
        for (int j = 0; j < core[i]->get_current_local_task_list().size(); j++){
            total_energy += (core[i]->get_current_local_task_list()[j]->get_finish_time_local() - core[i]->get_current_local_task_list()[j]->get_start_time_local()) * power[i];
        }
    }
    for (int i = 0; i < cloud.get_current_cloud_task_list().size(); i++){
        total_energy += (sending_time) * power[0];
    }
    energy = total_energy;
    time = 0;
    for (int i = 1; i <= core_number; i++){
        time = max(time, core[i]->get_finish_time());
    }
    for (int i = 1; i < current_task_list.size(); i++){
        cout << current_task_list[i]->get_priority() << " ";
    }
    for (int i = 1; i < current_task_list.size(); i++){
        cout << current_task_list[i]->get_exe_on_core_number() << " ";
    }
    cout << endl;
    cout << endl;
    time = max(time, cloud.get_finish_time());
    max_time = time * 1.5;
}

vector <Task*> temp_current_task_list;
vector<LocalCore*> core1;
Cloud cloud1;
vector <Task*> task_list1; //用于之后确定好一个最优解后 更新最开始task的各种时间


vector <Task*> best_task_list_after_changing_each_task;  //用于暂时存储 任务 i 移动到 core j上的最优结果
vector<LocalCore*> best_core_after_changing_each_task;
Cloud best_cloud;
vector <Task*> best_task_list1;

void Operation::task_migration(){
    cout << time << " " << energy << endl;
    while (1){
        //int flagggg = 0; //如果更新了 变1
        best_task_list_after_changing_each_task.clear();
        Task *t = new Task();
        temp_current_task_list.push_back(t);
        
        best_core_after_changing_each_task.clear();
        LocalCore *localcore1 = new LocalCore(0);
        best_core_after_changing_each_task.push_back(localcore1);
        for (int k = 1; k <= core_number; k++){
            LocalCore *localcore1 = new LocalCore(k);
            best_core_after_changing_each_task.push_back(localcore1);
        }
        
        best_cloud = Cloud(sending_time, process_time,receive_time);
        
        best_task_list1.clear();
        Task *t11 = new Task();
        best_task_list1.push_back(t11);
        for (int k = 1; k < task_list.size(); k++){
            Task *t11 = new Task(k);
            best_task_list1.push_back(t11);
        }
        double max_delta_energy = 0;
        double max_delta_energy_by_delta_time = 0;
        double time111 = 0;
        double energy111 = 0;
        int flag_exit_or_not = 0;
        int flag = 0;
        
        for (int i = 1; i < task_list.size(); i++){
            if (task_list[i]->get_exe_on_core_number() == 0){//排除了从cloud往local移动的情况
                continue;
            }
            for (int j = 0; j <= core_number; j++){
                if(task_list[i]->get_exe_on_core_number() == j) //排除原本线程往自己线程移动的情况
                    continue;
                temp_current_task_list.clear();
                Task *t = new Task();
                temp_current_task_list.push_back(t);
                
                core1.clear();
                LocalCore *localcore = new LocalCore(0);
                core1.push_back(localcore);
                for (int k = 1; k <= core_number; k++){
                    LocalCore *localcore = new LocalCore(k);
                    localcore->set_current_local_task_list(core[k]->get_current_local_task_list());
                    core1.push_back(localcore);
                }
                cloud1 = Cloud(sending_time,process_time,receive_time);
                cloud1.set_current_cloud_task_list(cloud.get_current_cloud_task_list());
                
                task_list1.clear(); //用于之后确定好一个最优解后 更新最开始task的各种时间
                Task *t1 = new Task();
                task_list1.push_back(t1);
                //Task task_list[task_number+1];
                for (int k = 1; k < task_list.size(); k++){
                    Task *t1 = new Task(k);
                    t1->set_ready_time_send(0);
                    t1->set_exe_on_core_number(task_list[k]->get_exe_on_core_number());
                    
                    task_list1.push_back(t1);
                }
                
                
                
                
                //**************
                //cout << "owowowowowowowowo" << endl;
                kernal_algorithm(i,j);
                //**************
                //
                
                
                //int flag = 0;
                double temp_time = 0;
                for (int k = 1; k <= core_number; k++){
                    temp_time = max(core1[k]->get_finish_time(), temp_time);
                }
                temp_time = max(temp_time, cloud1.get_finish_time());
                //cout << endl << endl << endl << "max time:::::" << max_time << endl << endl << endl ;
                if (temp_time > max_time)
                    continue;
                double delta_time = temp_time - time; //时间变化量
                double temp_energy = 0;
                for (int k = 1; k <= core_number; k++){
                    for (int p = 0; p < core1[k]->get_current_local_task_list().size(); p++){
                        //temp_energy += (core1[k]->get_current_local_task_list()[p]->get_finish_time_local() - core1[k]->get_current_local_task_list()[p]->get_start_time_local()) * power[k];
                        temp_energy += (task_list1[core1[k]->get_current_local_task_list()[p]->get_id()]->get_finish_time_local() - task_list1[core1[k]->get_current_local_task_list()[p]->get_id()]->get_start_time_local()) * power[k];
                    }
                }
                for (int k = 0; k < cloud1.get_current_cloud_task_list().size(); k++){
                    temp_energy += (sending_time) * power[0];
                }
                double delta_energy = energy - temp_energy;
                
                //cout << "time::" << temp_time << "     energy::" << temp_energy << endl;
                //cout << "delta time::" << delta_time << "    delta energy::" << delta_energy << endl;
                if (delta_energy <= 0)
                    continue;
                cout << "第" << i << "个任务，放到第" <<j << "个线程上，得到的时间:" << temp_time << ",能量损耗为：" << temp_energy << endl;
                cout << "此次调整，时间的变化量为：" << delta_time << ", 能量的变化量为：" << delta_energy << ",比变化量为：" << delta_energy/delta_time << endl;
                cout << endl;
                
                if (delta_time == 0 && delta_energy > 0 && delta_energy > max_delta_energy){ //更新最优解 往新开辟的空间去更新
                    max_delta_energy = delta_energy;
                    best_task_list_after_changing_each_task = temp_current_task_list;
                    for (int k = 1; k <= core_number; k++){
                        best_core_after_changing_each_task[k]->set_current_local_task_list(core1[k]->get_current_local_task_list());
                        best_core_after_changing_each_task[k]->set_finish_time(core1[k]->get_finish_time());
                        best_core_after_changing_each_task[k]->set_current_local_task_list(core1[k]->get_current_local_task_list());
                    }
                    for (int k = 1; k < task_list.size(); k++){
                        best_task_list1[k]->set_finish_time_local(task_list1[k]->get_finish_time_local());
                        best_task_list1[k]->set_start_time_local(task_list1[k]->get_start_time_local());
                        best_task_list1[k]->set_exe_on_core_number(task_list1[k]->get_exe_on_core_number());
                        best_task_list1[k]->set_ready_time_send(task_list1[k]->get_ready_time_send());
                        best_task_list1[k]->set_finish_time_send(task_list1[k]->get_finish_time_send());
                        best_task_list1[k]->set_finish_time_cloud(task_list1[k]->get_finish_time_cloud());
                        best_task_list1[k]->set_finish_time_receive(task_list1[k]->get_finish_time_receive());
                        best_task_list1[k]->set_ready_time_local(task_list1[k]->get_ready_time_local());
                    }
                    
                    best_cloud.set_current_cloud_task_list(cloud1.get_current_cloud_task_list());
                    best_cloud.set_finish_sending_time(cloud1.get_finish_sending_time());
                    
                    time111 = temp_time;
                    energy111 = temp_energy;
                    flag = 1;
                    flag_exit_or_not = 1;
                    continue;
                }
                else if (flag == 0){
                    //cout << 123456 << endl << endl;
                    double a = double(delta_energy / delta_time);
                    if (a > max_delta_energy_by_delta_time){
                        max_delta_energy_by_delta_time = a;
                        
                        best_task_list_after_changing_each_task = temp_current_task_list;
                        
                        for (int k = 1; k <= core_number; k++){
                            best_core_after_changing_each_task[k]->set_current_local_task_list(core1[k]->get_current_local_task_list());
                            best_core_after_changing_each_task[k]->set_finish_time(core1[k]->get_finish_time());
                            best_core_after_changing_each_task[k]->set_current_local_task_list(core1[k]->get_current_local_task_list());
                        }
                        for (int k = 1; k < task_list.size(); k++){
                            best_task_list1[k]->set_finish_time_local(task_list1[k]->get_finish_time_local());
                            best_task_list1[k]->set_start_time_local(task_list1[k]->get_start_time_local());
                            best_task_list1[k]->set_exe_on_core_number(task_list1[k]->get_exe_on_core_number());
                            best_task_list1[k]->set_ready_time_send(task_list1[k]->get_ready_time_send());
                            best_task_list1[k]->set_finish_time_send(task_list1[k]->get_finish_time_send());
                            best_task_list1[k]->set_finish_time_cloud(task_list1[k]->get_finish_time_cloud());
                            best_task_list1[k]->set_finish_time_receive(task_list1[k]->get_finish_time_receive());
                            best_task_list1[k]->set_ready_time_local(task_list1[k]->get_ready_time_local());
                        }
                        
                        best_cloud.set_current_cloud_task_list(cloud1.get_current_cloud_task_list());
                        best_cloud.set_finish_sending_time(cloud1.get_finish_sending_time());
                        
                        time111 = temp_time;
                        energy111 = temp_energy;
                        
                        flag_exit_or_not = 1;
                        continue;
                    }
                }
            }
        }
        if (flag_exit_or_not == 0){
            break;
        }
        else{
            current_task_list = best_task_list_after_changing_each_task;
            for (int k = 1; k < task_list.size(); k++){
                task_list[k]->set_ready_time_send(best_task_list1[k]->get_ready_time_send());
                task_list[k]->set_finish_time_send(best_task_list1[k]->get_finish_time_send());
                task_list[k]->set_finish_time_cloud(best_task_list1[k]->get_finish_time_cloud());
                task_list[k]->set_finish_time_receive(best_task_list1[k]->get_finish_time_receive());
                task_list[k]->set_start_time_local(best_task_list1[k]->get_start_time_local());
                task_list[k]->set_finish_time_local(best_task_list1[k]->get_finish_time_local());
                task_list[k]->set_exe_on_core_number(best_task_list1[k]->get_exe_on_core_number());
                task_list[k]->set_ready_time_local(best_task_list1[k]->get_ready_time_local());
            }
            for (int k = 1; k <= core_number; k++){
                core[k]->set_current_local_task_list(best_core_after_changing_each_task[k]->get_current_local_task_list());
                core[k]->set_finish_time(best_core_after_changing_each_task[k]->get_finish_time());
            }
            cloud.set_finish_sending_time(best_cloud.get_finish_sending_time());
            cloud.set_current_cloud_task_list(best_cloud.get_current_cloud_task_list());
            time = time111;
            energy = energy111;
            //flagggg = 1;
        }
        cout << "一轮结束" << endl;
        cout << "time:" << time << "; energy:" << energy << endl;
        for (int k = 1; k <= core_number; k++){
            for (int p = 0; p < core[k]->get_current_local_task_list().size(); p++){
                cout << "id:" << core[k]->get_current_local_task_list()[p]->get_id() << " core:" << core[k]->get_current_local_task_list()[p]->get_exe_on_core_number() << " start time:" << core[k]->get_current_local_task_list()[p]->get_start_time_local() << " finish_time:" << core[k]->get_current_local_task_list()[p]->get_finish_time_local() << endl;
            }
        }
        for (int k = 0; k < cloud.get_current_cloud_task_list().size(); k++){
            cout << "id:" << cloud.get_current_cloud_task_list()[k]->get_id() << " start time:" << cloud.get_current_cloud_task_list()[k]->get_ready_time_send() << " finish time:" << cloud.get_current_cloud_task_list()[k]->get_finish_time_send() << endl;
        }
        for (int i = 1; i <= core_number; i++){
            int start_time = 0;
            for (int j = 0; j < core[i]->get_current_local_task_list().size(); j++){
                while(start_time < core[i]->get_current_local_task_list()[j]->get_start_time_local()){
                    cout << "_ ";
                    start_time++;
                }
                while(start_time < core[i]->get_current_local_task_list()[j]->get_finish_time_local()){
                    cout << core[i]->get_current_local_task_list()[j]->get_id() << " ";
                    start_time++;
                }
            }
            cout << endl;
        }
        //cout << "send: ";
        int start_time = 0;
        for (int i = 0; i < cloud.get_current_cloud_task_list().size(); i++){
            while(start_time < cloud.get_current_cloud_task_list()[i]->get_finish_time_send() - sending_time){
                cout << "_ ";
                start_time++;
            }
            while(start_time < cloud.get_current_cloud_task_list()[i]->get_finish_time_send()){
                cout << cloud.get_current_cloud_task_list()[i]->get_id() << " ";
                start_time++;
            }
        }
        cout << endl;
        //cout << "cloud: ";
        start_time = 0;
        for (int i = 0; i < cloud.get_current_cloud_task_list().size(); i++){
            while(start_time < cloud.get_current_cloud_task_list()[i]->get_finish_time_cloud() - process_time){
                cout << "_ ";
                start_time++;
            }
            while(start_time < cloud.get_current_cloud_task_list()[i]->get_finish_time_cloud()){
                cout << cloud.get_current_cloud_task_list()[i]->get_id() << " ";
                start_time++;
            }
        }
        cout << endl;
        //cout << "receive: ";
        start_time = 0;
        for (int i = 0; i < cloud.get_current_cloud_task_list().size(); i++){
            while(start_time < cloud.get_current_cloud_task_list()[i]->get_finish_time_receive() - receive_time){
                cout << "_ ";
                start_time++;
            }
            while(start_time < cloud.get_current_cloud_task_list()[i]->get_finish_time_receive()){
                cout << cloud.get_current_cloud_task_list()[i]->get_id() << " ";
                start_time++;
            }
        }
        cout << endl;
    }
}


void Operation::kernal_algorithm(int i_task, int j_core){
    //test
    
    
    
    //在ori线程上删除该任务 取task_list[i_task]->get_exe_on_core_number
    //cout <<core1[task_list[i_task]->get_exe_on_core_number()]->get_current_local_task_list().size() << endl;
    for (int i = 0; i < core1[task_list[i_task]->get_exe_on_core_number()]->get_current_local_task_list().size(); i++){
        if (core1[task_list[i_task]->get_exe_on_core_number()]->get_current_local_task_list()[i] == task_list[i_task]){
            //cout << 1 << endl;
            core1[task_list[i_task]->get_exe_on_core_number()]->Erase(i);
            break;
        }
    }
    //test
    
    
    //
    //在目标线程上添加任务i
    //注意在最后一个位置插入和在第一个位置插入
    if (j_core >= 1){ //s(target) is local
        if (core1[j_core]->get_current_local_task_list().size() == 0){
            core1[j_core]->set_current_local_task_list(task_list[i_task]);
        }
        else if (core1[j_core]->get_current_local_task_list()[0]->get_start_time_local() >= task_list[i_task]->get_ready_time_local()){
            core1[j_core]->Insert_task(0, task_list[i_task]);//get_current_local_task_list().insert(core1[j_core]->get_current_local_task_list().begin(), task_list[i_task]);
        }
        else if (core1[j_core]->get_current_local_task_list()[core1[j_core]->get_current_local_task_list().size()-1]->get_start_time_local() <= task_list[i_task]->get_ready_time_local()){
            core1[j_core]->set_current_local_task_list(task_list[i_task]);
        }
        else{
            int n = (int)core1[j_core]->get_current_local_task_list().size();
            for (int k = 1; k < n; k++){
                if (core1[j_core]->get_current_local_task_list()[k]->get_start_time_local() >= task_list[i_task]->get_ready_time_local() && core1[j_core]->get_current_local_task_list()[k-1]->get_start_time_local() <= task_list[i_task]->get_ready_time_local()){
                    core1[j_core]->Insert_task(k, task_list[i_task]);//get_current_local_task_list().insert(core1[j_core]->get_current_local_task_list().begin()+i, task_list[i_task]);
                    break;
                }
            }
        }
    }
    else if (j_core == 0){
        if (cloud1.get_current_cloud_task_list().size() == 0){
            cloud1.set_current_cloud_task_list(task_list[i_task]);
        }
        else if (cloud1.get_current_cloud_task_list()[0]->get_ready_time_send() >= task_list[i_task]->get_ready_time_local()){
            cloud1.Insert_task(0, task_list[i_task]);//get_current_cloud_task_list().insert(cloud1.get_current_cloud_task_list().begin(), task_list[i_task]);
        }
        else if (cloud1.get_current_cloud_task_list()[cloud1.get_current_cloud_task_list().size()-1]->get_ready_time_send() <= task_list[i_task]->get_ready_time_local()){
            //cloud1.get_current_cloud_task_list().push_back(task_list[i_task]);
            cloud1.set_current_cloud_task_list(task_list[i_task]);
        }
        else{
            int n = (int)cloud1.get_current_cloud_task_list().size();
            //cout << n << endl;
            //cout << cloud1.get_current_cloud_task_list().size() << endl;
            for (int k = 1; k < n; k++){
                if (cloud1.get_current_cloud_task_list()[k]->get_ready_time_send() >= task_list[i_task]->get_ready_time_local() && cloud1.get_current_cloud_task_list()[k-1]->get_ready_time_send() <= task_list[i_task]->get_ready_time_local()){
                    cloud1.Insert_task(k, task_list[i_task]);
                    break;
                }
            }
        }
    }
    
    
    
    //-------------------------no problem
    
    //
    
    //
    
    
    
    
    task_list1[i_task]->set_exe_on_core_number(j_core);
    
    int ready1[task_list.size()];
    int ready2[task_list.size()];
    stack<int> s;
    //初始化ready1
    for (int i = 1; i < task_list.size(); i++){
        ready1[i] = (int)task_list[i]->get_pre().size();
        ready2[i] = 1;
    }
    //初始化ready2
    for (int i = 1; i <= core_number; i++){
        if (core1[i]->get_current_local_task_list().size() != 0){
            ready2[core1[i]->get_current_local_task_list()[0]->get_id()] = 0;
        }
    }
    //cout << "<<<<<>>>>>" << cloud1.get_current_cloud_task_list()[0]->get_id() << endl;
    if (cloud1.get_current_cloud_task_list().size() != 0){
        ready2[cloud1.get_current_cloud_task_list()[0]->get_id()] = 0;
    }
    for (int i = 1; i < task_list.size(); i++){
        if (ready1[i] == 0 && ready2[i] == 0){
            s.push(i);
            ready1[i] = -1;
            ready2[i] = -1;
        }
    }
    
    
    while(!s.empty()){
        int core_num1;
        Task *current_task = task_list1[s.top()];
        if (s.top() == i_task){
            core_num1 = j_core;
        }
        else{
            core_num1 = task_list[current_task->get_id()]->get_exe_on_core_number();
        }
        s.pop();
        //schedule
        if (core_num1 == 0){
            double maxx1 = 0;
            double maxx2 = 0;
            for (int i = 0; i < task_list[current_task->get_id()]->get_pre().size(); i++){
                maxx2 = max(task_list1[task_list[current_task->get_id()]->get_pre()[i]->get_id()]->get_finish_time_local(), task_list1[task_list[current_task->get_id()]->get_pre()[i]->get_id()]->get_finish_time_send());
                maxx1 = max(maxx1, maxx2);
            }
            maxx1 = max(maxx1, cloud1.get_finish_sending_time());
            task_list1[current_task->get_id()]->set_ready_time_send(maxx1);
            task_list1[current_task->get_id()]->set_finish_time_send(maxx1 + sending_time);
            task_list1[current_task->get_id()]->set_finish_time_cloud(maxx1 + sending_time + process_time);
            task_list1[current_task->get_id()]->set_finish_time_receive(maxx1 + sending_time + process_time + receive_time);
            task_list1[current_task->get_id()]->set_cloud_or_local(0);
            double sum = maxx1 + sending_time;
            cloud1.set_finish_time(sum + process_time + receive_time);
            cloud1.set_finish_sending_time(sum);
            //cout <<current_task->get_id() << " ready time sending: " << task_list1[current_task->get_id()]->get_ready_time_send() << "; finish time sending: " << task_list1[current_task->get_id()]->get_finish_time_send() << " 0" << endl;
        }
        else if (core_num1 > 0){
            double start_time = 0;
            double minn = 0;
            for (int i = 0; i < task_list[current_task->get_id()]->get_pre().size(); i++){
                minn = max(task_list1[task_list[current_task->get_id()]->get_pre()[i]->get_id()]->get_finish_time_local(), task_list1[task_list[current_task->get_id()]->get_pre()[i]->get_id()]->get_finish_time_receive());  //取前置任务结束的最早时间
                start_time = max(start_time, minn);
            }
            //test
            
            
            //
            
            
            start_time = max(start_time, core1[core_num1]->get_finish_time());
            task_list1[current_task->get_id()]->set_exe_on_core_number(core_num1);
            task_list1[current_task->get_id()]->set_start_time_local(start_time);
            task_list1[current_task->get_id()]->set_ready_time_local(start_time);
            task_list1[current_task->get_id()]->set_finish_time_local(start_time + task_list[current_task->get_id()]->get_time_local(core_num1));
            task_list1[current_task->get_id()]->set_cloud_or_local(1);
            core1[core_num1]->set_finish_time(start_time + task_list[current_task->get_id()]->get_time_local(core_num1));
        }
        temp_current_task_list.push_back(task_list[current_task->get_id()]);
        for (int i = 0; i < task_list[current_task->get_id()]->get_succ().size(); i++){
            ready1[task_list[current_task->get_id()]->get_succ()[i]->get_id()] -= 1;
        }
        if (core_num1 > 0){
            for (int i = 0; i < core1[core_num1]->get_current_local_task_list().size()-1; i++){
                if (current_task->get_id() == core1[core_num1]->get_current_local_task_list()[i]->get_id()){
                    ready2[core1[core_num1]->get_current_local_task_list()[i+1]->get_id()] = 0;
                    break;
                }
            }
        }
        else{
            for (int i = 0; i < cloud1.get_current_cloud_task_list().size()-1; i++){
                if (current_task->get_id() == cloud1.get_current_cloud_task_list()[i]->get_id()){
                    ready2[cloud1.get_current_cloud_task_list()[i+1]->get_id()] = 0;
                    break;
                }
            }
        }
        for (int i = 1; i < task_list.size(); i++){
            if (ready1[i] == 0 && ready2[i] == 0){
                s.push(i);
                ready1[i] = -1;
                ready2[i] = -1;
            }
        }
    }
}
