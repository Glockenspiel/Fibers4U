#ifndef TASK_H
#define TASK_H

#include <functional>
using std::function;
#include "BaseTask.h"

//lightweight task
class Task : public BaseTask{
public:
	//constructor with std::function
	Task(function<void()> &func);

	//constructor with a function and an object
	template <class Func, class Obj>
	Task(Func func, Obj obj){
		fn = std::bind(func, obj);
	}

	//constructor for static function
	template <class Func>
	Task(Func staticFunc){
		fn = staticFunc;
	}

	~Task();

	//runs the task, overrided by BaseTask
	void run();

private:
	function<void()> fn;
};

#endif