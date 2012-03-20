#include <vector>

#ifndef WORM_H
#define WORM_H

struct itemPos
{
	int x,y;
	itemPos();
	itemPos(int xPos, int yPos);
};

enum COLOR_PAIRS
{
	NULL_PAIR, BLANK_PAIR, WALL_PAIR, APPLE_PAIR, WORM_PAIR
};

void mSleep(int mSecs);
void addch_spec(char sym, int colorPair, bool bold);

class Worm
{
private:
	int delT, points, maxHeight, maxWidth, speedChange;
	char direction, wormSym, appSym, wallSym, beep;
	bool gotApple, gamePaused;
	itemPos applePos;
	std::vector<itemPos> wBody;
	
	void AddApple();
	bool CheckCollision();
	void MoveWorm();
	void FlashWorm();
public:
	Worm();
	~Worm();
	void StartGame();
};

#endif //SNAKE_H
