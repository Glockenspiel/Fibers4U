#ifndef TASK_ARGS_COPY_H
#define TASK_ARGS_COPY_H

#include "BaseTask.h"
#include <functional>
#include "Nothing.h"

using namespace aaa;
using namespace std::placeholders;

template<class A, class B = nothing, class C = nothing>
class TaskArgsCopy : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgsCopy(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2, _3);
	}

	~TaskArgsCopy(){}

	void setArgs(A a, B b, C c){
		this->a = a;
		this->b = b;
		this->c = c;
	}

	void run(){
		fn(a, b, c);
	}
private:
	std::function<void(A, B, C)> fn;

	A a;
	B b;
	C c;
};

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