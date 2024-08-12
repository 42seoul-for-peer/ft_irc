/*
 * GameLogic.h
 *
 *  Created on: 2011. 12. 7.
 *      Author: malloc
 */

#include "Block.h"

#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_

static const int WIDTH = 10;
static const int HEIGHT = 22;

class GameLogic {
public:
	int score;
	int myTurn;
	bool isRun;
	int interval;
	int clearLines;
	// block Array
	int blockArray[1000];
	int blockTurn;
	/*board[y][x]*/
	char *board[HEIGHT];
	/*게임시작*/
	void start(char *, char *);
	/*컨트롤 하는 블럭을 이동*/
	void moveLeft();
	/*컨트롤 하는 블럭을 이동*/
	void moveRight();
	/*컨트롤 하는 블럭을 이동*/
	void moveDown();
	/*컨트롤 하는 블럭을 회전*/
	void transforse();
	/*컨트롤 하는 블럭을 가장 아래로 이동*/
	void drop();
	/*제어하는 블럭을 표시*/
	void printBlock();
	/*Viewing Shape of Dropped Block*/
	void printDropBlock();
	GameLogic();
	virtual ~GameLogic();

private:
	Block block;
	Block block_dropped;
	Block next;
	bool isNotGameOver;
	/*다찬 줄이 있는지 체크*/
	void clearLine(int line);
	void check(Block block);
	bool isCollision();
	void refresh();
	void refreshNextBlock();
	void refreshMyTurn();
	void refreshPoint();
	void gameOver();
	void gameClear();
	void nextTurn();
	void nextLevel();
	int level;
};

#endif /* GAMELOGIC_H_ */
