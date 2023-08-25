//
//  Cloud.hpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/29.
//

#ifndef Cloud_hpp
#define Cloud_hpp

#include <stdio.h>
#include "Task.hpp"
class Cloud{
private:
    double sending_time, cloud_time, receive_time;
    double finish_time = 0;
    double finish_sending_time = 0;
    vector <Task*> current_cloud_task_list;
public:
    Cloud(){};
    Cloud(double sending_time, double cloud_time, double receive_time);
    void set_current_cloud_task_list(Task* &a){
        current_cloud_task_list.push_back(a);
    }
    void set_current_cloud_task_list(vector<Task*> current_cloud_task_list){
        this->current_cloud_task_list = current_cloud_task_list;
    }
    vector<Task*> get_current_cloud_task_list(){
        return current_cloud_task_list;
    }
    double get_finish_time(){
        return finish_time;
    }
    void set_finish_time(double finish_time){
        this->finish_time = finish_time;
    }
    double get_finish_sending_time(){
        return finish_sending_time;
    }
    void set_finish_sending_time(double finish_sending_time){
        this->finish_sending_time = finish_sending_time;
    }
    void Insert_task(int i, Task* &k){
        current_cloud_task_list.insert(current_cloud_task_list.begin()+i, k);
    }
};
#endif /* Cloud_hpp */
