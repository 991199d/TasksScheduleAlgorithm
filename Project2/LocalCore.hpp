//
//  LocalCore.hpp
//  Project2
//
//  Created by 刘天睿 on 2021/11/29.
//

#ifndef LocalCore_hpp
#define LocalCore_hpp

#include <stdio.h>
#include "Task.hpp"
class LocalCore{
private:
    int id;
    double finish_time = 0;
    double ready_time;
    vector <Task*> current_local_task_list;
public:
    LocalCore(int id){
        this->id = id;
    };
    void set_finish_time(double finish_time){
        this->finish_time = finish_time;
    }
    double get_finish_time(){
        return finish_time;
    }
    void set_current_local_task_list(Task* &a){
        current_local_task_list.push_back(a);
    }
    void set_current_local_task_list(vector<Task*> current_local_task_list){
        this->current_local_task_list = current_local_task_list;
    }
    vector <Task*> get_current_local_task_list(){
        return current_local_task_list;
    }
    void Erase(int i){
        current_local_task_list.erase(current_local_task_list.begin()+i);
    }
    void Insert_task(int i, Task* &k){
        current_local_task_list.insert(current_local_task_list.begin()+i, k);
    }
};
#endif /* LocalCore_hpp */
