#include "include/fbr.h"
#include "demo/Player.h"

using namespace std;
using namespace fbr;

Counter ctr("test");
Counter ctr2("test2");

int main()
	{

	Player *p = new Player();

	Task<> *printHpTask = new Task<>(std::bind(&Player::printHp, p));

	Task<int> *addHpTask = new Task<int>(std::bind(&Player::addHp, p,_1));

	int amount = 100;
	addHpTask->set(amount);


	vector<BaseTask*> myTaskList;
	myTaskList.push_back(printHpTask);
	myTaskList.push_back(addHpTask);
	myTaskList.push_back(printHpTask);

	for (unsigned int i = 0; i < myTaskList.size(); i++)
		myTaskList[i]->run();
	
	


	//ctr.add(1);
	Scheduler::addCounter(ctr);
	con_cout << "count:" << Scheduler::getCounterValByName("test") << fbr::endl;
	

	BaseTask *printHP = new Task<>(std::bind(&Player::printHp, p));
	printHP->setReuseable(true);

	Task<int> *taskArg = new Task<int>(std::bind(&Player::addHp, p,_1));
	int a=20;
	taskArg->set(a);

	Task<int, bool> *test = new Task<int, bool>(std::bind(&Player::damage, p, _1, _2));
	int b = 5;
	bool isMagic=false;
	test->set(b, isMagic);

	Task<int, int, int> *move = new Task<int, int, int>(std::bind(&Player::move, p, _1, _2, _3));
	int c = 0;
	move->set(54, c, std::thread::hardware_concurrency());

	Task<> *inputtask = new Task<>(std::bind(&Player::taskInput, p));
	inputtask->setReuseable(true);

	Scheduler *scheduler = new Scheduler(0,4, taskArg,true,true);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}

	fbr::con_cout << "All workers ready! " << fbr::endl;

	Task<> *update = new Task<>(std::bind(&Player::update, p));
	

	//wake up main thread
	Task<> *endTask = new Task<>(&Scheduler::wakeUpMain);

	Task<> *longTask = new Task<>(std::bind(&Player::longTask, p));
	//run all task unsyncronized
	//example with vector
	//vector<BaseTask*> allTasks = { printHP, move, update,longTask };
	//scheduler->runTasks(allTasks, priority::low);
	//example with variadic function
	Scheduler::setTaskNaming("my tasks");
	Scheduler::runTasks(priority::low, &ctr, 4, addHpTask, printHP, move, update);
	//Scheduler::runTasks(priority::low, 1, inputtask);
	//Scheduler::runTasks(priority::low, 1, inputtask);
	//scheduler->waitAllFibersFree(); 
	//Scheduler::waitForCounter(0, printHP);
	//Scheduler::waitForCounter(0, printHP);
	

	Scheduler::waitForCounter(&ctr, 0, longTask, &ctr2);

	Counter cc("test3");
	Scheduler::waitForCounter(&ctr2, 0, inputtask, &cc);

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