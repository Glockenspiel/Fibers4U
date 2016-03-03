#include "Timer.h"

Timer::Timer(){}

Timer::~Timer(){}

//wait until time is up using a spin lock
void Timer::wait(int secs){
	std::time_t endTime = std::time(nullptr) + secs;
	while (endTime > std::time(nullptr)){}
}