#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player{
public:
	Player();

	/*
	empty task for intializing scheduler
	*/
	void empty();

	/*
	This task takes a long time (i.e. it just spinlocks for a period of time)
	This will allow worker threads to sleep if sleeping is enabled in the scheduler
	*/
	static void longCall();

	//testing funcionality of various features
	static void testing();

	//adds health to the player
	void addHealth(int amount);

	//prints the health of the player
	void printHealth();

	//move the player position and prints the new position
	void movePos(int x, int y, int z);

	//displays the information passed as arguments
	static void showIdentity(std::string name, int age);

	//wakes up the main thread
	void endScheduler();

	//take input from the command line
	void takeInput();


	~Player();
private:
	int health=0, x=0, y=0, z=0;
};

#endif