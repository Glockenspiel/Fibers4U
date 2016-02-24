#include "Task.h"
#include <iostream>
#include "global.h"

Task::Task(function<void()>& func){
	fn = func;
}

Task::~Task(){

}

void Task::run(){
	//std::cout << "executing task\n";
	global::writeLock();
	std::cout << "Executing task" << std::endl;
	global::writeUnlock();
	fn();
	global::writeLock();
	std::cout << "Completed task" << std::endl;
	global::writeUnlock();
}