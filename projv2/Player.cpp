#include "Player.h"
#include <iostream>
#include "global.h"

Player::Player(){

}

Player::~Player(){

}

void Player::update(){
	global::writeLock();
	std::cout << "updating player" << std::endl;
	global::writeUnlock();
}

void Player::addHp(int amount){
	hp += amount;
}

void Player::printHp(){
	global::writeLock();
	std::cout << "player hp: " << hp << std::endl;
	global::writeUnlock();
}

void Player::empty(){}

void Player::damage(int amount, bool isMagic){
	if (isMagic)
		hp -= amount * 2;
	else 
		hp -= amount;
}

void Player::move(int x, int y, int z){
	global::writeLock();
	std::cout << "x:" << x << " y:" << y << " z:" << z << std::endl;
	global::writeUnlock();
}