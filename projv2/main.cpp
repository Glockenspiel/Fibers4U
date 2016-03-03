#include <iostream>
#include "Task.h"
#include "Player.h"
#include "Scheduler.h"
#include "Global.h"
#include "TaskArgs.h"
#include "TaskArgsCopy.h"
#include "Timer.h"

using namespace std;


int main(){
	Player *p = new Player();
	BaseTask *printHP = new TaskArgs<>(&Player::printHp, p);

	TaskArgs<int> *taskArg = new TaskArgs<int>(&Player::addHp, p);
	int a = 20;
	taskArg->setArgs(a);

	TaskArgs<int, bool> *test = new TaskArgs<int, bool>(&Player::damage, p);
	int b = 5;
	bool isMagic=false;
	test->setArgs(b, isMagic);

	TaskArgsCopy<int, int, int> *move = new TaskArgsCopy<int, int, int>(&Player::move, p);
	int c = 0;
	move->setArgs(54, c, global::getThreadCount());

	Scheduler *scheduler = new Scheduler(9,2, taskArg);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}

	fbr::cout << "All workers ready! " << fbr::endl;

	Task *update = new Task(&Player::update, p);
	

	//wake up main thread
	Task *endTask = new Task(&Scheduler::wakeUpMain);

	Task *longTask = new Task(&Player::longTask, p);
	//run all task unsyncronized
	//example with vector
	//vector<BaseTask*> allTasks = { printHP, move, update,longTask };
	//scheduler->runTasks(allTasks, priority::low);
	//example with variadic function
	Scheduler::runTasks(priority::low, 4, printHP, move, update, longTask);

	//scheduler->waitAllFibersFree();  
	//scheduler->runTask(endTask);
	Scheduler::waitForCounter(0, endTask);


	//puts main thread to wait and doesn't cunsume cpu time
	//wakes up when endTask is run
	Scheduler::waitMain();

	scheduler->close();
	
	
	system("pause");
	//delete scheduler and display msg
	delete scheduler;
	
	//fbr::log << "here" << Log::endl;
	fbr::cout << "Scheduler deleted" << fbr::endl;
	Timer *timer = new Timer();
	timer->wait(2);
	delete timer;
}