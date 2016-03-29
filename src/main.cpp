#include "include/fbr.h"
#include "demo/Player.h"

using namespace std;
using namespace fbr;

Counter ctr("test");
Counter ctr2("test2");

int main(){
	//ctr.add(1);
	Scheduler::addCounter(ctr);
	con_cout << "count:" << Scheduler::getCounterValByName("test") << fbr::endl;
	


	Player *p = new Player();
	DelTask<> *delTask = new DelTask<>(std::bind(&Player::printHp, p));
	



	DelTask<int> *delTask2 = new DelTask<int>(std::bind(&Player::addHp, p,_1));
	int amount = 100;
	delTask2->set(amount);


	vector<BaseTask*> testing;
	testing.push_back(delTask2);
	testing.push_back(delTask);
	//for (BaseTask* bt : testing)
		//bt->run();
	
	

	

	BaseTask *printHP = new DelTask<>(std::bind(&Player::printHp, p));
	printHP->setReuseable(true);

	DelTask<int> *taskArg = new DelTask<int>(std::bind(&Player::addHp, p,_1));
	int a=20;
	taskArg->set(a);

	DelTask<int, bool> *test = new DelTask<int, bool>(std::bind(&Player::damage, p, _1, _2));
	int b = 5;
	bool isMagic=false;
	test->set(b, isMagic);

	DelTask<int, int, int> *move = new DelTask<int, int, int>(std::bind(&Player::move, p, _1, _2, _3));
	int c = 0;
	move->set(54, c, std::thread::hardware_concurrency());

	DelTask<> *inputtask = new DelTask<>(std::bind(&Player::taskInput, p));
	inputtask->setReuseable(true);

	Scheduler *scheduler = new Scheduler(0,4, taskArg,true,true);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}

	fbr::con_cout << "All workers ready! " << fbr::endl;

	DelTask<> *update = new DelTask<>(std::bind(&Player::update, p));
	

	//wake up main thread
	DelTask<> *endTask = new DelTask<>(&Scheduler::wakeUpMain);

	DelTask<> *longTask = new DelTask<>(std::bind(&Player::longTask, p));
	//run all task unsyncronized
	//example with vector
	//vector<BaseTask*> allTasks = { printHP, move, update,longTask };
	//scheduler->runTasks(allTasks, priority::low);
	//example with variadic function
	Scheduler::setTaskNaming("my tasks");
	Scheduler::runTasks(priority::low, &ctr, 4, delTask2, printHP, move, update);
	//Scheduler::runTasks(priority::low, 1, inputtask);
	//Scheduler::runTasks(priority::low, 1, inputtask);
	//scheduler->waitAllFibersFree(); 
	//Scheduler::waitForCounter(0, printHP);
	//Scheduler::waitForCounter(0, printHP);
	

	Scheduler::waitForCounter(&ctr, 0, longTask, &ctr2);

	Scheduler::waitForCounter(&ctr2, 0, inputtask, &ctr);

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