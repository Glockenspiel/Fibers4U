#include <iostream>
#include "Task.h"
#include "Player.h"
#include "Scheduler.h"
#include "Global.h"

using namespace std;

int main(){
	Scheduler *scheduler = new Scheduler(1,1);
	if (scheduler->getIsConstructed() == false){
		system("pause");
		return 0;
	}

	Player *p = new Player();

	function<void()> fn = std::bind(&Player::update, p);
	Task *task1 = new Task(fn);
	scheduler->runTask(*task1);


	//wait until schduler ends (should use condiction variable here i think)
	while (scheduler->getEndProcess() == false){}
}