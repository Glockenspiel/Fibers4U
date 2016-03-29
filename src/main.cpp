#include "include/fbr.h"
#include "demo/Player.h"
#include "DelTask.h"

using namespace std;
using namespace fbr;

Counter ctr("test");

int main(){
	ctr.add(1);
	Scheduler::addCounter(ctr);
	con_cout << "count:" << Scheduler::getCounterValByName("test") << fbr::endl;
	


	Player *p = new Player();

	DelTask<int> *delTask2 = new DelTask<int>(std::bind(&Player::addHp, p,_1));
	delTask2->set(100);
	//delTask2->run();


	DelTask<> *delTask = new DelTask<>(std::bind(&Player::printHp, p));
	delTask->set();
	//delTask->run();

	vector<BaseTask*> testing;
	testing.push_back(delTask2);
	testing.push_back(delTask);
	//for (BaseTask* bt : testing)
		//bt->run();
	
	

	

	BaseTask *printHP = new TaskArgs<>(&Player::printHp, p);
	printHP->setReuseable(true);

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
	Scheduler::setTaskNaming("my tasks");
	Scheduler::runTasks(priority::low, 4, delTask2, printHP, move, update);
	//Scheduler::runTasks(priority::low, 1, inputtask);
	//Scheduler::runTasks(priority::low, 1, inputtask);
	//scheduler->waitAllFibersFree(); 
	//Scheduler::waitForCounter(0, printHP);
	//Scheduler::waitForCounter(0, printHP);
	Scheduler::waitForCounter(0, inputtask);


	Scheduler::waitForCounter(0, longTask);

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