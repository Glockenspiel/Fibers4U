#ifndef TASK_ARGS_COPY_H
#define TASK_ARGS_COPY_H

#include "BaseTask.h"
#include <functional>
#include "Nothing.h"

using namespace nothingSpace;
using namespace std::placeholders;

/*
This is an overloaded template class which allows for different
amounts of parameters (1,2 or 3).
When this setArgs() is called the task will duplicate (Copy) the argument(s).
The copys will not update the origional values nor themselves be updated by the origionals.
If you don't want to make copies look at TaskArgs
*/

template<class A, class B = nothing, class C = nothing>
class TaskArgsCopy : public BaseTask{
public:
	//constructor which creates the function with use of placeholders
	template <class Func, class Obj>
	TaskArgsCopy(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2, _3);
	}

	//Desructor
	~TaskArgsCopy(){}

	//make copied of the arguments
	void setArgs(A a, B b, C c){
		this->a = a;
		this->b = b;
		this->c = c;
	}

	//run function and substitute the placeholders
	void run(){
		fn(a, b, c);
	}
private:
	std::function<void(A, B, C)> fn;

	A a;
	B b;
	C c;
};

//2 parameters required for the function
template<class A, class B>
class TaskArgsCopy<A, B, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgsCopy(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2);
	}

	~TaskArgsCopy(){}

	void setArgs(A a, B b){
		this->a = a;
		this->b = b;
	}

	void run(){
		fn(a, b);
	}
private:
	std::function<void(A, B)> fn;
	A a;
	B b;
};

//1 parameter required for the function
template<class A>
class TaskArgsCopy<A, nothing, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgsCopy(Func func, Obj obj){
		fn = std::bind(func, obj, _1);
	}

	~TaskArgsCopy(){}

	void setArgs(A a){
		this->a = a;
	}

	void run(){
		fn(a);
	}
private:
	std::function<void(A)> fn;
	A a;
};

#endif