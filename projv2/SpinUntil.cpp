#include "SpinUntil.h"
#include <ctime>

namespace fbr{
	SpinUntil::SpinUntil(){}

	SpinUntil::~SpinUntil(){}

	//wait until time is up using a spin lock
	void SpinUntil::wait(int secs){
		std::time_t endTime = std::time(nullptr) + secs;
		while (endTime > std::time(nullptr)){}
	}
}