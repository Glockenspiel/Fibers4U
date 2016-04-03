#include "include/fbr.h"
#include "demo/Player.h"

using namespace std;
using namespace fbr;

Counter ctr("player status");
Counter ctr2("input value example");

Counter sampleCtr("sample");
Counter sampleEndCtr("endOfSample");

Counter inputCtr("input");
Counter endCounter("ending");

static Player staticPlayer;

//task entry point
static void task_entry_point_sample(){
	con_cout << "SAMPLE" << fbr::endl;

	//task with no paramaters
	Task<> *printHealthTask = new Task<>(std::bind(&Player::printHealth, staticPlayer));

	//task with paramaters
	Task<int, int, int> *movePosTask = new Task<int, int, int>(std::bind(&Player::movePos, staticPlayer, _1, _2, _3));
	int y = 15;
	movePosTask->set(-10, y, 1);

	//task with parameters of different types
	Task<std::string, int> *showIdentityTask = new Task<std::string, int>(std::bind(&Player::showIdentity, _1, _2));
	showIdentityTask->set("john doe", 32);

	//full syntax example
	std::function<void()> testingFunc = &Player::testing;
	Task<> *testingTask = new Task<>(testingFunc);
	
	//short hand example
	Task<> *longTask = new Task<>(&Player::longCall);

	//run these tasks asynchronously
	Scheduler::runTasks({ showIdentityTask, movePosTask, testingTask, longTask}, Priority::low, &sampleCtr);
	//same call but with the overloaded function making use of variadic functions
	//Scheduler::runTasks(Priority::low, &sampleCtr, 2, showIdentityTask, movePosTask);

	//wait for these tasks to end when call printHealthTask
	Scheduler::waitForCounter(&sampleCtr, 0, printHealthTask, &sampleEndCtr);
}


int main()
	{
	staticPlayer.addHealth(100);
	Player *player_ptr = new Player();

	//declaring tasks
	Task<> *emptyTask = new Task<>(std::bind(&Player::empty, player_ptr));
	Task<> *endTask = new Task<>(std::bind(&Player::endScheduler, player_ptr));
	Task<> *inputTask = new Task<>(std::bind(&Player::takeInput, player_ptr));

	//constructing scheduler
	Scheduler *scheduler = new Scheduler(0,4, emptyTask,true,true);

	//check if scheduler was constructed correctly
	if (scheduler->getIsConstructed() == false){ 
		return 0;
	}
	
	fbr::con_cout << "All workers ready! " << fbr::endl;

	//task entry point example
	task_entry_point_sample();
	
	//running tasks synchronously
	Scheduler::waitForCounter(&sampleEndCtr, 0, inputTask, &inputCtr); //this task takes input
	Scheduler::waitForCounter(&inputCtr, 0, endTask, &endCounter); //this task end the scheduler
	
	//puts main thread to wait and doesn't consume cpu time
	//wakes up when endTask is run
	Scheduler::waitMain();

	scheduler->close();

	//delete scheduler and display msg for 2 seconds
	delete scheduler;
	
	fbr::con_cout << "Scheduler deleted" << fbr::endl;
	SpinUntil *timer = new SpinUntil();
	timer->wait(2);
	delete timer;
}