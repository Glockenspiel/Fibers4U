#include "Player.h"
#include <iostream>

Player::Player(){

}

Player::~Player(){

}

void Player::update(){
	std::cout << "updating player" << std::endl;
}

void Player::addHp(int amount){
	hp += amount;
}

void Player::printHp(){
	std::cout << "player hp: " << hp << std::endl;
}