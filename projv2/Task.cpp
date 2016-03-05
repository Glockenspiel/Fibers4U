#include "Task.h"
#include "con_iostream.h"

Task::Task(function<void()>& func){
	fn = func;
}

Task::~Task(){}

void Task::run(){
	fn();
}
