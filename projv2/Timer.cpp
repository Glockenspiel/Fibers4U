#include "Timer.h"

Timer::Timer(){}

Timer::~Timer(){}

void Timer::wait(int secs){
	std::time_t endTime = std::time(nullptr) + secs;
	while (endTime > std::time(nullptr)){}
}