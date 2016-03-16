#include "Task.h"
#include "con_iostream.h"

namespace fbr{
	Task::Task(function<void()>& func){
		fn = func;
	}

	Task::~Task(){}

	void Task::run(){
		fn();
	}
}