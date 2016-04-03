#include "demo/Player.h"
#include <iostream>
#include "include/con_iostream.h"
#include "include/SpinUntil.h"
#include "include/Scheduler.h"
#include "include/con_data_struct/concurrent.h"
#include "include/con_data_struct/con_queue.h"
#include "include/con_data_struct/con_stack.h"

using namespace fbr;

Player::Player(){}

Player::~Player(){}

void Player::empty(){}

//delaying task
void Player::longCall(){
	SpinUntil *t = new SpinUntil();
	t->wait(3);
}

void Player::testing(){
	//concurrent primitive types testing
	concurrent<int> aa = 10;
	concurrent<double> dd = 5.5;
	++dd;
	aa = (int)dd.get() + 10;
	concurrent<int>* ptr = &aa;

	con_vector<int> mylist;

	//access con_bvector with with only 1 lock
	const int size = 5;
	mylist.getLock();
		for (unsigned int i = 0; mylist.size_async() < size; i++)
			mylist.push_back_async(aa.get());
	mylist.unlock();

	fbr::con_cout << "myList:" << mylist.at(0) << fbr::endl;

	
	fbr::con_cout << "CONCURRENT VALUE:" << ptr->get() << fbr::endl;

	//push 5 values and pop all off, using a single lock
	con_queue<int> myqueue;

	//snycronized
	myqueue.push(aa.get());

	//testing concurrent queue
	myqueue.getLock();
	for (unsigned int i = 1; i <= size; i++)
		myqueue.push_async(i);

	while (myqueue.empty_async() == false)
		con_cout << "queue popped:" << myqueue.getPop_async() << fbr::endl;
	myqueue.unlock();


	//testing concurrent stack
	con_stack<int> mystack;
	mystack.getLock();
	for (unsigned int i = 1; i <= size; i++)
		mystack.push_async(i);

	while (mystack.empty_async() == false)
		con_cout << "stack popped:" << mystack.getPop_async() << fbr::endl;
	mystack.unlock();
}


void Player::addHealth(int amount){
	health += amount;
}

void Player::printHealth(){
	con_cout << "player health: " << health << fbr::endl;
}

void Player::movePos(int x, int y, int z){
	this->x += x;
	this->y += y;
	this->z += z;

	con_cout << "pos: " << this->x << ", " << this->y << ", " << this->z << fbr::endl;
}

void Player::showIdentity(std::string name, int age){
	con_cout << "Name: " << name << fbr::endl << "Age: " << age << fbr::endl;
}


void Player::endScheduler(){
	Scheduler::wakeUpMain();
}

void Player::takeInput(){
	std::string str;
	fbr::con_input(str, "Enter some text:");
	fbr::con_cout << str << fbr::endl;
}