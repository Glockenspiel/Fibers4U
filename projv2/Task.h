#ifndef TASK_H
#define TASK_H

#include <functional>
using std::function;
#include "BaseTask.h"

class Task : public BaseTask{
public:
	Task(function<void()> &func);

	template <class Func, class Obj>
	Task(Func func, Obj obj){
		fn = std::bind(func, obj);
	}
	~Task();

	void run();

private:
	function<void()> fn;
};

#endif