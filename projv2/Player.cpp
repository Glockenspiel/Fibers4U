#include "Player.h"
#include <iostream>
#include "con_iostream.h"
#include "SpinUntil.h"
#include "Scheduler.h"
#include "con_soa_vector.h"
#include "concurrent.h"
#include "Task.h"

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
	con_soa_vector<int, double> soalist;
	soalist.add(1, 3.5);

	concurrent<int> aa = 10;
	concurrent<double> dd = 5.5;
	++dd;
	aa = dd.get() + 10;
	concurrent<int>* ptr = &aa;

	con_vector<int> mylist;

	mylist.get_lock_extern();
		for (unsigned int i = 0; mylist.size_unsync() < 10; i++)
			mylist.push_back_unsync(aa.get());
	mylist.unlock_extern();

	fbr::con_cout << "myList:" << mylist.at(0) << fbr::endl;

	
	fbr::con_cout << "CONCURRENT VALUE:" << ptr->get() << fbr::endl;

	std::string s;
	fbr::con_input(s, "Enter some text:");
	fbr::con_cout << s << fbr::endl;
	Scheduler::wakeUpMain();
}
