#ifndef TASK_ARGS_H
#define TASK_ARGS_H

#include "BaseTask.h"
#include <functional>
#include "Nothing.h"

using namespace aaa;
using namespace std::placeholders;

template<class A = nothing, class B = nothing, class C = nothing>
class TaskArgs : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgs(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2, _3);
	}

	~TaskArgs(){
		delete aPtr;
		delete bPtr;
		delete cPtr;
	}

	void setArgs(A& a, B& b, C& c){
		aPtr = &a;
		bPtr = &b;
		cPtr = &c;
	}

	void run(){
		fn(*aPtr, *bPtr, *cPtr);
	}
private:
	std::function<void(A, B, C)> fn;

	A* aPtr;
	B* bPtr;
	C* cPtr;
};

template<class A, class B>
class TaskArgs<A, B, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgs(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2);
	}

	~TaskArgs(){
		delete aPtr;
		delete bPtr;
	}

	void setArgs(A& a, B& b){
		aPtr = &a;
		bPtr = &b;
	}

	void run(){
		fn(*aPtr, *bPtr);
	}
private:
	std::function<void(A, B)> fn;
	A *aPtr;
	B *bPtr;
};

template<class A>
class TaskArgs<A, nothing, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgs(Func func, Obj obj){
		fn = std::bind(func, obj, _1);
	}

	~TaskArgs(){
		delete aPtr;
	}

	void setArgs(A& a){
		aPtr = &a;
	}

	void run(){
		fn(*aPtr);
	}
private:
	std::function<void(A)> fn;
	A* aPtr;
};

template<>
class TaskArgs<nothing, nothing, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskArgs(Func func, Obj obj){
		fn = std::bind(func, obj);
	}

	~TaskArgs(){}

	void run(){
		fn();
	}
private:
	std::function<void()> fn;
};




#endif