#include "include/BaseTask.h"

namespace fbr{
	bool BaseTask::isReusable(){
		return reusable;
	}

	void BaseTask::setReuseable(bool reuse){
		reusable = reuse;
	}
}