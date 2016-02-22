#include "Task.h"
#include <iostream>

Task::Task(function<void()>& func){
	fn = func;
}

Task::~Task(){

}

void Task::run(){
	std::cout << "executing task\n";
	fn();
	std::cout << " task completed\n";
}