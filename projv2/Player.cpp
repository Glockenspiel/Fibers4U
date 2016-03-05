#include "Player.h"
#include <iostream>
#include "con_iostream.h"
#include "Timer.h"
#include "Scheduler.h"
#include "con_soa_vector.h"

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
	Timer *t = new Timer();
	t->wait(2);
}

void Player::taskInput(){
	con_soa_vector<int, double> soalist;
	soalist.add(1, 3.5);



	std::string s;
	fbr::cin(s, "Enter some text:");
	fbr::con_cout << s << fbr::endl;
	Scheduler::wakeUpMain();
}
