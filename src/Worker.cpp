#include "include/Worker.h"
#include <iostream>
#include "include/con_iostream.h"
#include "include/Scheduler.h"
#include <ctime>

namespace fbr{
	Worker::Worker(unsigned int id){
		this->id = id;
		lastRun == high_resolution_clock::now();
	}

	Worker::~Worker(){}

	/*
	Starts the spin lock for this worker thread
	if the state is prepared, the spin will call
	runAndFree() on the currentFiber
	the state of the worker will return to free
	the task queue will be notified of the free
	*/
	void Worker::run(){
		running = true;
		while (running.load(std::memory_order_relaxed)){
			if (state.load(std::memory_order_relaxed) == State::prepared){
				currentFiber->runAndFree();
				setState(State::free);
				Scheduler::notifyTaskFinished();
				Scheduler::notifyWorkerBeenFreed(this);
				lastRun = steady_clock::now();
			}

			//don't sleep if sleeping is disabled
			if (Scheduler::isSleepingEnabled()){
				timeNow = high_resolution_clock::now();
				timeSinceLastRun = timeNow - lastRun;
				if (timeSinceLastRun > timeUntilSleep){
					fbr::con_cout << "WORKER ASLEEP: " << getID() << fbr::endl;

					threadSleeper.sleep();

					fbr::con_cout << "WORKER AWAKE:" << id << fbr::endl;
				}
			}
		}
	}

	//switches fiber pointer
	void Worker::switchFiber(Fiber& fiber){
		//if nullptr assign it
		if (currentFiber == nullptr){
			currentFiber = &fiber;
			return;
		}
		//no change in fiber pointer
		else if (currentFiber->getID() == fiber.getID()){
			return;
		}
		//assign fiber pointer
		else{
			currentFiber = &fiber;
		}
	}

	//ends worker
	void Worker::close(){
		//wait for current fiber to be set
		while (currentFiber == nullptr){}
		currentFiber->waitUntilFree();
		running.store(false, std::memory_order_release);
		wakeUp();
	}

	//switch in the fiber, use fibers current task
	//moves state to prepared
	void Worker::set(Fiber& fiber){
		switchFiber(fiber);
		setState(State::prepared);
	}

	//Returns true if this worker has been acquired
	//The worker state must be free to acquire it
	bool Worker::tryAcquire(){
		if (state.load(std::memory_order_acquire) == State::free){
			setState(State::aquired);
			return true;
		}
		return false;
	}

	//spin locks until the workers goes to free state and 
	//then the fiber changed to acquired state
	void Worker::forceAcquire(){
		while (state.load(std::memory_order_relaxed) != free){}
		setState(State::aquired);
		wakeUp();
	}

	//wake up worker thread if it is asleep
	void Worker::wakeUp(){
		if (Scheduler::isSleepingEnabled())
			threadSleeper.wakeUp();
	}

	//sets the state of the worker
	void Worker::setState(State s){
		state.store(s, std::memory_order_seq_cst);
	}

	//returns the id
	unsigned int Worker::getID(){
		return id;
	}

	//returns the current state
	Worker::State Worker::getState(){
		return state;
	}
}