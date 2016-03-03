#ifndef TIMER_H
#define TIMER_H

#include <ctime>

//simple timer for waiting a thread ina spin lock
class Timer{
public :
	Timer();
	~Timer();
	//spin waits until the time is up
	void wait(int secs);
};

#endif