#ifndef PLAYER_H
#define PLAYER_H

class Player{
public:
	Player();
	void update();
	void addHp(int amount);
	void printHp();
	void empty();
	~Player();
private:
	int hp=0;
};

#endif