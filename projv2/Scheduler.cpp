#include "Scheduler.h"

Scheduler::Scheduler(unsigned const int fiberCount){
	
	for (unsigned int i = 0; i < fiberCount; i++)
		fibers.push_back(new Fiber(counter));

}

Scheduler::~Scheduler(){
	//desrtuct fibers
	for (std::vector< Fiber* >::iterator it = fibers.begin(); it != fibers.end(); ++it)
		delete (*it);
	fibers.clear();

	//destruct queued tasks
	for (std::vector< Task* >::iterator it = queuedTasks.begin(); it != queuedTasks.end(); ++it)
		delete (*it);
	queuedTasks.clear();
}

void Scheduler::runTask(Task &task){
	counter++;
	//try find an available fiber
	for (unsigned int i = 0; i<fibers.size(); i++)
		if (fibers[i]->isFiberFree()){
			fibers[i]->runAndFree(task);
			return;
		}

	queuedTasks.push_back(&task);
}