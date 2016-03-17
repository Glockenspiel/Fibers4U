#ifndef WORKER_H
#define WORKER_H

#include "Fiber.h"
#include <atomic>
#include <chrono>
#include "thread_sleeper.h"


using std::atomic;
using namespace std::chrono;

namespace fbr{
	class Worker{
	public:
		/*
		Possible states of a worker:
		free = ready to be allocated a new fiber
		acquired = the fiber is being allocated
		prepared = the fiber is allowed to run
		*/
		enum State { free, aquired, prepared };
		Worker(unsigned int id);
		~Worker();

		//starts the worker thread running in a spinlock,
		//it runs the fiber if the fiber is prepared and then frees the fiber and itself
		void run();

		//ends the spinlock when the current fiber completes
		void close();

		//switches the fiber and sets the state to prepared
		void set(Fiber& fiber);

		//try move from state from free to acquire, ruturns false if failed
		bool tryAcquire();

		//returns the workers id
		unsigned int getID();

		//spinlocks until free, then changes state to acquired
		void forceAcquire();

		//returns the current state
		State getState();
	private:
		//switch the given fiber with the current fiber
		void switchFiber(Fiber& fiber);

		//sets the state
		void setState(State s);

		//wakes up the worker thread if it is asleep
		void wakeUp();

		atomic<bool> running = false;
		Fiber* currentFiber;
		atomic<State> state = free;

		//values for sleeping
		unsigned int id;
		high_resolution_clock::time_point lastRun, timeNow;

		duration<double, std::milli>
			timeSinceLastRun,
			timeUntilSleep = duration<double, std::milli>(1000);

		ThreadSleeper threadSleeper;
	};
}

#endif