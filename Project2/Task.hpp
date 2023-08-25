//
//  Task.hpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/27.
//

#ifndef Task_h
#define Task_h

#include <iostream>
#include <vector>
using namespace std;
class Task{
private:
    int id;
    int exe_on_core_number = 0;
    vector <Task*> pre;
    vector <Task*> succ;
    vector <double> time_local;
    double time_cloud, time_send, time_receive;
    double finish_time_local = 0, finish_time_send = 0, finish_time_cloud = 0, finish_time_receive = 0; // 用到了
    double ready_time_local, ready_time_send, ready_time_cloud, ready_time_receive;
    double start_time_local = 0;
    bool cloud_or_local; // 0 = cloud 1 = local
    double wi; //consumption cost
    double priority;
    bool entrance_task = 0;  //没有前置任务的task
    bool exit_task = 0; //没有后继任务的task
    bool flag = 0; //该任务有没有被schedule   0->no schedule   1->have been scheduled
public:
    Task(){}
    Task(int id){
        this->id = id;
    }
    int get_id(){
        return id;
    }
    void set_pre(Task* &t){
        pre.push_back(t);
    }
    vector <Task*> get_pre(){
        return pre;
    }
    void set_succ(Task* &t){
        succ.push_back(t);
    }
    vector <Task*> get_succ(){
        return succ;
    }
    void set_time_local(double t){
        time_local.push_back(t);
    }
    double get_time_local(int i){
        return time_local[i];
    }
    void set_time_cloud(double t){
        time_cloud = t;
    }
    double get_time_cloud(){
        return time_cloud;
    }
    void set_time_send(double t){
        time_send = t;
    }
    double get_time_send(){
        return time_send;
    }
    void set_time_receive(double t){
        time_receive = t;
    }
    double get_time_receive(){
        return time_receive;
    }
    void set_finish_time_local(double t){
        finish_time_local = t;
    }
    double get_finish_time_local(){
        return finish_time_local;
    }
    void set_finish_time_send(double t){
        finish_time_send = t;
    }
    double get_finish_time_send(){
        return finish_time_send;
    }
    void set_finish_time_cloud(double t){
        finish_time_cloud = t;
    }
    double get_finish_time_cloud(){
        return finish_time_cloud;
    }
    void set_finish_time_receive(double t){
        finish_time_receive = t;
    }
    double get_finish_time_receive(){
        return finish_time_receive;
    }
    void set_ready_time_local(double t){
        ready_time_local = t;
    }
    double get_ready_time_local(){
        return ready_time_local;
    }
    void set_ready_time_send(double t){
        ready_time_send = t;
    }
    double get_ready_time_send(){
        return ready_time_send;
    }
    void set_ready_time_cloud(double t){
        ready_time_cloud = t;
    }
    double get_ready_time_cloud(){
        return ready_time_cloud;
    }
    void set_ready_time_receive(double t){
        ready_time_receive = t;
    }
    double get_ready_time_receive(){
        return ready_time_receive;
    }
    bool get_cloud_or_local(){
        return cloud_or_local;
    }
    void set_priority(double xx){
        this->priority = xx;
    }
    double get_priority(){
        return priority;
    }
    double get_wi(){
        return wi;
    }
    void Set_primary_local_or_cloud(); //初始任务是在cloud上还是local
    void set_wi();
    void set_entrance_task(bool flag){
        entrance_task = flag;
    }
    bool get_entrance_task(){
        return entrance_task;
    }
    void set_exit_task(bool flag){
        exit_task = flag;
    }
    bool get_exit_task(){
        return exit_task;
    }
    void set_cloud_or_local(bool x){
        cloud_or_local = x;
    }
    void set_start_time_local(double start_time_local){
        this->start_time_local = start_time_local;
    }
    double get_start_time_local(){
        return start_time_local;
    }
    
    void set_exe_on_core_number(int i){
        exe_on_core_number = i;
    }
    
    int get_exe_on_core_number(){
        return exe_on_core_number;
    }
};

#endif /* Task_h */
