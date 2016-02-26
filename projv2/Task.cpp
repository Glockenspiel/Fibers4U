#include "Task.h"
#include "global.h"

Task::Task(function<void()>& func){
	fn = func;
}

Task::~Task(){}

void Task::run(){
	fn();
}