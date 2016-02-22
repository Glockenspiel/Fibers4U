#include "Task.h"
#include <iostream>

Task::Task(function<void()>& func){
	fn = func;
}

Task::~Task(){

}

void Task::run(){
	std::cout << "executing task" << std::endl;
	fn();
}