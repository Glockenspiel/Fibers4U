#ifndef TASK_ARGS_H
#define TASK_ARGS_H

#include "BaseTask.h"
#include <functional>
#include "Nothing.h"

using namespace nothingSpace;
using namespace std::placeholders;

namespace fbr{
	/*
	This is an overloaded template class which allows for different
	amounts of parameters (0,1,2 or 3).
	The arguments must be set using setArgs() before calling run()
	When this task is destructed it will delete the arguments.
	meaining the values passed to it shouldn't be used in another task.
	If you wish to pass a copy of a variable try using TaskArgCopy instead
	*/

	template<class A = nothing, class B = nothing, class C = nothing>
	class TaskArgs : public BaseTask{
	public:
		//constructor which creates the function with use of placeholders
		template <class Func, class Obj>
		TaskArgs(Func func, Obj obj){
			fn = std::bind(func, obj, _1, _2, _3);
		}

		//Desructor, delete all the arguments
		~TaskArgs(){
			delete aPtr;
			delete bPtr;
			delete cPtr;
		}

		//assign the pointers to the arguments
		void setArgs(A& a, B& b, C& c){
			aPtr = &a;
			bPtr = &b;
			cPtr = &c;
		}

		//run function and substitute the placeholders
		void run(){
			fn(*aPtr, *bPtr, *cPtr);
		}
	private:
		std::function<void(A, B, C)> fn;

		A* aPtr;
		B* bPtr;
		C* cPtr;
	};

	//2 parameters in the function
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

	//1 parameter in the function
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

	//no parameters in the function
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
}

#endif