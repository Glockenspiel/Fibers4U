#include <iostream>
#include "Task.h"
#include "Player.h"
#include "Scheduler.h"
#include "Global.h"

using namespace std;

int main(){
	Scheduler *scheduler = new Scheduler(1,1);
	if (scheduler->getIsConstructed() == false){
	//	global::writeLock();
		system("pause");
	//	global::writeUnlock();
		return 0;
	}

	Player *p = new Player();
	//function<void(int)> addHp = std::bind(&Player::addHp, p, std::placeholders::_1);
	//addHp(10);
	//p->printHp();

	function<void()> fn = std::bind(&Player::update, p);
	Task *task1 = new Task(fn);
	scheduler->runTask(*task1);

	//Task *task2 = new Task(fn);
	//scheduler->runTask(*task2);

	//scheduler->close();

	//global::writeLock();
	//system("pause");
	//while (isCompleted.load(std::memory_order_relaxed) == false){}
	//global::writeUnlock();

	//std::this_thread::yield();
}