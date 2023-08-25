//
//  Operation.hpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/29.
//

#ifndef Operation_hpp
#define Operation_hpp

#include <stdio.h>
#include <vector>
#include "Task.hpp"
#include "Cloud.hpp"
#include "LocalCore.hpp"
class Operation{
private:
    int core_number;
    int sending_time, process_time, receive_time;
    Cloud cloud;
    vector<LocalCore*> core;
    vector<Task*> task_list;
    vector<double> power;
    vector<Task*> current_task_list;
    vector<int> exit_task_list;
    double energy;
    double time;
    double max_time;
public:
    Operation(int n, vector<LocalCore*>& core, vector<Task*> &task_list, vector<double>& power, int sending_time, int process_time, int receive_time);
    void set_each_task_wi(); //consumption cost for each task i
    void task_prioritizing();
    void calculate_priority(int i); //递归计算
    bool static cmp(Task* &a, Task* &b);
    vector<LocalCore*> get_core(){
        return core;
    }
    Cloud get_cloud(){
        return cloud;
    }
    vector<Task*> get_current_task_list(){
        return current_task_list;
    }
    void task_migration();
    void kernal_algorithm(int i, int j);
    void set_energy(double energy){
        this->energy = energy;
    }
    double get_enerty(){
        return energy;
    }
    void set_time(double time){
        this->time = time;
    }
    double get_time(){
        return time;
    }
    void set_total_time(double total_time){
        this->max_time = total_time;
    }
    double get_total_time(){
        return max_time;
    }
};
#endif /* Operation_hpp */
