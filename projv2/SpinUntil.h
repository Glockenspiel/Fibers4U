#ifndef SPIN_UNTIL_H
#define SPIN_UNTIL_H



//simple timer for waiting a thread ina spin lock
class SpinUntil{
public :
	SpinUntil();
	~SpinUntil();
	//spin waits until the time is up
	void wait(int secs);
};

#endif