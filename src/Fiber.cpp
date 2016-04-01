#include "include/Fiber.h"
#include "include/Scheduler.h"
#include <iostream>

namespace fbr{
	using namespace priority;

	Fiber::Fiber(unsigned short id){
		this->id = id;
	}

	Fiber::~Fiber(){
		delete currentTask;
	}

	//runs the current task
	void Fiber::run(){
		waitForState(prepared);

		fbr::con_cout << "Executing fiber: " << id << fbr::endl;
		currentTask->run();
		fbr::con_cout << "Completed fiber: " << id << fbr::endl;
	}

	//alert the scheduler that this task has completed and is ready for reallocation
	void Fiber::freeFiber(){
		//delete the current task if it is not reusable
		if (currentTask->isReusable() == false)
		{
			BaseTask *temp = currentTask;
			currentTask = nullptr;

			delete temp;
		}
		fbr::con_cout << "freed:" << id << fbr::endl;
		setState(free);
	}

	//sets the current task of the fiber and the fibers priority
	void Fiber::setTask(BaseTask* task, Priority p, Counter* ctr){
		currentPriority = p;
		currentTask = task;
		m_ctr = ctr;
	}

	//runs the current task and frees it
	void Fiber::runAndFree(){
		run();
		freeFiber();
	}

	//returns the current id
	unsigned int Fiber::getID(){
		return id;
	}

	//wait until fiber is freed and set state to prepared
	void Fiber::setPrepared(){
		waitForState(acquired);
		state = prepared;
	}

	//waits for the fibers state to reach the given state
	void Fiber::waitForState(State s){
		while (state.load(std::memory_order_relaxed) != s){}
	}

	//sets the state to the given state
	void Fiber::setState(State s){
		state.store(s, std::memory_order_release);
	}

	//waits until the fiber reaches its free state
	void Fiber::waitUntilFree(){
		while (state.load(std::memory_order_relaxed) != free){}
	}

	//returns true if the fiber is in the given state
	bool Fiber::inState(Fiber::State s){
		return state.load(std::memory_order_relaxed) == s;
	}

	//trys to change state from freed to aquire, returns true if state change was a success
	bool Fiber::tryAcquire(){
		if (state.load(std::memory_order_relaxed) == free){
			setState(acquired);
			//counter += 1;
			return true;
		}
		return false;
	}

	//returns the fibers current priority
	Priority Fiber::getPriority(){
		return currentPriority;
	}

	Counter* Fiber::getCounter(){
		return m_ctr;
	}
}