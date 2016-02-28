#ifndef TASK_ARG_H
#define TASK_ARH_H

#include "BaseTask.h"

using namespace std::placeholders;

template <class Arg>
class TaskArg : public BaseTask{
public:

	template <class Func, class Obj>
	TaskArg(Func f, Obj o) {
		fn = std::bind(f, o, _1);
	}
	~TaskArg(){
		delete argPtr;
	}

	void run(){
		fn(*argPtr);
	}

	void setArg(Arg& arg){
		argPtr = &arg;
	}

private:
	function<void(Arg)> fn;
	Arg* argPtr;
};

#endif