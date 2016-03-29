#ifndef FIBER_H
#define FIBER_H

#include "BaseTask.h"

#include <atomic>
#include <string>
#include "con_data_struct\Counter.h"

namespace fbr{
	//namspace for prioity enum so it can be accessed more easily
	namespace priority{
		enum Priority { low, medium, high };
	}

	using std::atomic;
	using namespace priority;



	class Fiber{
	public:
		/*
		possible states for fibers:
		free = ready for allocation
		acquired = the fiber can be set or currently is being set
		prepared = fiber ready to be run or is running on a worker thread
		*/
		enum State { free, acquired, prepared };

		//constructor 
		Fiber(unsigned short id);

		~Fiber();

		//run the current task and then frees the fiber
		void runAndFree();

		//set the current task and the priority
		void setTask(BaseTask* task, priority::Priority p, std::string taskNaming,  Counter* ctr);

		//returns the id of this fiber
		unsigned int getID();

		//moves the state from acquired to prepared
		void setPrepared();

		//waits until the fiber reaches its free state
		void waitUntilFree();

		//returns true if the fiber is in the given state
		bool inState(State s);

		//trys to move the fiber from free to acquired state, returns false if failed
		bool tryAcquire();

		//returns the current priority of this fiber
		priority::Priority getPriority();

	private:
		//runs the current task
		void run();

		//frees the fiber
		void freeFiber();

		//waits until the fiber reaches the given state
		void waitForState(State);

		//sets the state of the fiber
		void setState(State);

		unsigned int id;
		atomic<State> state = free;
		BaseTask *currentTask;
		Priority currentPriority = Priority::low;
		std::string currentTaskNaming;
		Counter* m_ctr;
	};
}

#endif