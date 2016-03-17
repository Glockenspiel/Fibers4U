#include "demo/Player.h"
#include <iostream>
#include "include/con_iostream.h"
#include "include/SpinUntil.h"
#include "include/Scheduler.h"
#include "include/con_data_struct/concurrent.h"
#include "include/Task.h"
#include "include/con_data_struct/con_queue.h"
#include "include/con_data_struct/con_stack.h"

using namespace fbr;

Player::Player(){

}

Player::~Player(){

}

void Player::update(){
	fbr::con_cout << "updating player" << fbr::endl;
}

void Player::addHp(int amount){
	hp += amount;
}

void Player::printHp(){
	fbr::con_cout << "player hp: " << hp << fbr::endl;
}

void Player::empty(){}

void Player::damage(int amount, bool isMagic){
	if (isMagic)
		hp -= amount * 2;
	else 
		hp -= amount;
}

void Player::move(int x, int y, int z){
	fbr::con_cout << "x:" << x << " y:" << y << " z:" << z << fbr::endl;
}

void Player::longTask(){
	SpinUntil *t = new SpinUntil();
	t->wait(2);
	Task *inputtask = new Task(&Player::taskInput, this);
	Scheduler::waitForCounter(0, inputtask);
}

void Player::taskInput(){

	concurrent<int> aa = 10;
	concurrent<double> dd = 5.5;
	++dd;
	aa = (int)dd.get() + 10;
	concurrent<int>* ptr = &aa;

	con_vector<int> mylist;

	//access with only 1 lock
	mylist.getLock();
		for (unsigned int i = 0; mylist.size_unsync() < 10; i++)
			mylist.push_back_unsync(aa.get());
	mylist.unlock();

	fbr::con_cout << "myList:" << mylist.at(0) << fbr::endl;

	
	fbr::con_cout << "CONCURRENT VALUE:" << ptr->get() << fbr::endl;

	//push 10 values and pop all off, using a single lock
	con_queue<int> myqueue;

	//snycronized
	myqueue.push(aa.get());

	myqueue.getLock();
	for (unsigned int i = 1; i <= 10; i++)
		myqueue.push_unsync(i);

	while (myqueue.empty_unsync() == false)
		con_cout << "queue popped:" << myqueue.getPop_unsync() << fbr::endl;
	myqueue.unlock();


	con_stack<int> mystack;
	mystack.getLock();
	for (unsigned int i = 1; i <= 10; i++)
		mystack.push_unsync(i);

	while (mystack.empty_unsync() == false)
		con_cout << "stack popped:" << mystack.getPop_unsync() << fbr::endl;
	mystack.unlock();

	std::string s;
	fbr::con_input(s, "Enter some text:");
	fbr::con_cout << s << fbr::endl;
	Scheduler::wakeUpMain();
}
