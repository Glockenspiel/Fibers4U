#include "stdafx.h"
#include "Player.h"
#include <iostream>
namespace fbr
{
	Player::Player(){
		std::cout << "Conructor" << std::endl;
	}

	void Player::printNum(int a){
		std::cout << "num" << a << std::endl;
	}
};