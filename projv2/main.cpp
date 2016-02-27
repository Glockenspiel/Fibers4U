#include <iostream>
#include "Task.h"
#include "Player.h"
#include "Scheduler.h"
#include "Global.h"
//#include "WaitVars.h"
using namespace std;

//bool awake = false;
//condition_variable cv;
int main(){
	Player *p = new Player();
	std::function<void()> startfn = std::bind(&Player::printHp,p);
	Task *startingTask = new Task(startfn);

	

	Scheduler *scheduler = new Scheduler(3,2, *startingTask);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}

	scheduler->waitAllFibersFree();

	function<void()> fn = std::bind(&Player::update, p);
	Task *task1 = new Task(fn);
	scheduler->runTask(*task1);

	//wake up main thread
	function<void()> closeFn = std::bind(&Scheduler::wakeUpMain, scheduler);
	Task *endTask = new Task(closeFn);
	scheduler->runTask(*endTask);

	//puts main thread to wait and doesn't cunsume cpu time
	//wakes up when endTask is run
	scheduler->waitMain();

	scheduler->close();
	system("pause");

	//delete scheduler and display msg
	delete scheduler;
	std::cout << "Scheduler deleted" << std::endl;
	Timer *timer = new Timer();
	timer->wait(1);
	delete timer;
}