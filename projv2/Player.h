#ifndef PLAYER_H
#define PLAYER_H

class Player{
public:
	Player();
	void update();
	void addHp(int amount);
	void damage(int amount, bool isMagic);
	void move(int x, int y, int z);
	void printHp();
	void empty();
	void longTask();
	void taskInput();
	~Player();
private:
	int hp=0;
};

#endif