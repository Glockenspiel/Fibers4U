#include "fbr.h"
#include "Player.h"

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
	move->setArgs(54, c, std::thread::hardware_concurrency());

	Task *inputtask = new Task(&Player::taskInput, p);
	inputtask->setReuseable(true);

	Scheduler *scheduler = new Scheduler(0,4, taskArg,true,true);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}

	fbr::con_cout << "All workers ready! " << fbr::endl;

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
	//Scheduler::waitForCounter(0, inputtask);
	//Scheduler::waitForCounter(0, inputtask);


	//puts main thread to wait and doesn't cunsume cpu time
	//wakes up when endTask is run
	Scheduler::waitMain();

	scheduler->close();
	
	
	//system("pause");
	//delete scheduler and display msg
	delete scheduler;
	
	//fbr::log << "here" << Log::endl;
	fbr::con_cout << "Scheduler deleted" << fbr::endl;
	SpinUntil *timer = new SpinUntil();
	timer->wait(2);
	delete timer;
}