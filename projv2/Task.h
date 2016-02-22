#ifndef TASK_H
#define TASK_H

#include <functional>
using std::function;

class Task{
public:
	Task(function<void()> &func);
	void run();
	~Task();
private:
	function<void()> fn;
};

#endif