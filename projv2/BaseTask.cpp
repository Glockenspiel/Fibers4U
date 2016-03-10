#include "BaseTask.h"

bool BaseTask::isReusable(){
	return reusable;
}

void BaseTask::setReuseable(bool reuse){
	reusable = reuse;
}