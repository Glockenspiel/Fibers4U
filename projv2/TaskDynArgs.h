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

	TaskDynArgs(){
		if (isShared == false){
			delete aPtr;
			delete bPtr;
			delete cPtr;
		}
	}

	void setArgs(A& a, B& b, C& c){
		isShared = false;
		aPtr = &a;
		bPtr = &b;
		cPtr = &c;
	}

	void setArgsCopy(A& a, B& b, C& c){
		isShared = true;
		aPtr = &a;
		bPtr = &b;
		cPtr = &c;
	}

	void run(){
		fn(*aPtr, *bPtr, *cPtr);
	}
private:
	std::function<void(A, B, C)> fn;
	bool isShared = false;
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

	TaskDynArgs(){
		if (isShared == false){
			delete aPtr;
			delete bPtr;
		}
	}

	void setArgs(A& a, B& b){
		isShared = false;
		aPtr = &a;
		bPtr = &b;
	}

	void setArgsCopy(A& a, B& b){
		isShare = true;
		aPtr = &a;
		bPtr = &b;
	}

	void run(){
		fn(*aPtr, *bPtr);
	}
private:
	std::function<void(A,B)> fn;
	bool isShared = true;
	A* aPtr;
	B* bPtr;
};

template<class A>
class TaskDynArgs<A, nothing, nothing> : public BaseTask{
public:
	template <class Func, class Obj>
	TaskDynArgs(Func func, Obj obj){
		fn = std::bind(func, obj, _1);
	}

	TaskDynArgs(){
		if (isShared == false){
			delete aPtr;
		}
	}

	void setArgs(A& a){
		isShared = false;
		aPtr = &a;
	}

	void setArgsCopy(A& a){
		isShare = true;
		aPtr = &a;
	}

	void run(){
		fn(*aPtr, *bPtr);
	}
private:
	std::function<void(A)> fn;
	bool isShared = true;
	A* aPtr;
};




#endif