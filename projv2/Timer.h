#ifndef TIMER_H
#define TIMER_H

#include <ctime>


class Timer{
public :
	Timer();
	~Timer();
	void wait(int secs);
};

#endif