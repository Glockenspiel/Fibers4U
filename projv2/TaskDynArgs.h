#ifndef TASK_DYN_ARGS
#define TASK_DYN_ARGS

#include "BaseTask.h"
#include <functional>
#include "Nothing.h"

using namespace aaa;

template<class A, class B=nothing, class C=nothing>
class TaskDynArgs : public BaseTask{
public:
	template <class Func, class Obj>
	TaskDynArgs(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2, _3);
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
class TaskDynArgs<A, B, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskDynArgs(Func func, Obj obj){
		fn = std::bind(func, obj, _1, _2);
	}

	void setArgs(A& a, B& b){
		aPtr = &a;
		bPtr = &b;
	}

	void run(){
		fn(*aPtr, *bPtr);
	}
private:
	std::function<void(A,B)> fn;
	A* aPtr;
	B* bPtr;
};




#endif