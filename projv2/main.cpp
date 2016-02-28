#include <iostream>
#include "Task.h"
#include "Player.h"
#include "Scheduler.h"
#include "Global.h"
#include "TaskArg.h"
//#include "WaitVars.h"
using namespace std;



//bool awake = false;
//condition_variable cv;
int main(){
	Player *p = new Player();
	Task *startingTask = new Task(&Player::printHp, p);

	TaskArg<int> *taskArg = new TaskArg<int>(&Player::addHp, p);
	int a = 10;
	taskArg->setArg(a);


	Scheduler *scheduler = new Scheduler(5,2, taskArg);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}

	scheduler->waitAllFibersFree();

	scheduler->runTask(startingTask);

	Task *task1 = new Task(&Player::update, p);
	scheduler->runTask(task1);
	

	//wake up main thread
	Task *endTask = new Task(&Scheduler::wakeUpMain, scheduler);
	scheduler->runTask(endTask);

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