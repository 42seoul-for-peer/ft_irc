/*
 * Block.cpp
 *
 *  Created on: 2011. 12. 6.
 *      Author: malloc
 */

#include "Block.h"
#include "myio.h"
#include "GameLogic.h"
#include <stdlib.h>
#include <time.h>

#define Z COLOR_RED	//Z
#define S COLOR_GREEN	//S
#define O COLOR_BROWN	//O
#define J COLOR_BLUE	//J
#define T COLOR_MAGENTA//T
#define I COLOR_CYAN	//I
#define L COLOR_GRAY	//L

const int Block::RIGHT=WIDTH-1;
const int Block::LEFT=0;
const int Block::BOTTOM=HEIGHT+1;
const int Block::TOP=0;

const char * Block::BLOCK = "[]";
const char * Block::CLEAR = "  ";

const Point Block::block_shape[][4][4] = {
{ 
		{ { 0, 2 }, { 1, 2 }, { 1, 3 }, { 2, 3 } }, 
		{ { 0, 3 }, { 0, 2 },{ 1, 2 }, { 1, 1 } }, 
		{ { 0, 2 }, { 1, 2 }, { 1, 3 }, { 2, 3 } }, 
		{ { 0, 3 }, { 0, 2 }, { 1, 2 }, { 1, 1 } } //Z
}, { 
		{ { 0, 3 }, { 1, 3 }, { 1, 2 }, { 2, 2 } }, 
		{ { 0, 1 }, { 0, 2 }, { 1, 2 }, { 1, 3 } }, 
		{ { 0, 3 }, { 1, 3 }, { 1, 2 }, { 2, 2 } }, 
		{ { 0, 1 }, { 0, 2 }, { 1, 2 }, { 1, 3 } } //S
}, { 
		{ { 1, 2 }, { 1, 3 }, { 0, 3 }, { 0, 2 } }, 
		{ { 1, 2 }, { 1, 3 }, { 0, 3 }, { 0, 2 } }, 
		{ { 1, 2 }, { 1, 3 }, { 0, 3 }, { 0, 2 } }, 
		{ { 1, 2 }, { 1, 3 }, { 0, 3 }, { 0, 2 } }//ㅁ
}, { 
		{ { 0, 2 }, { 0, 3 }, { 1, 3 }, { 2, 3 } }, 
		{ { 0, 3 }, { 1, 3 }, { 1, 2 }, { 1, 1 } }, 
		{ { 0, 2 }, { 1, 2 }, { 2, 2 }, { 2, 3 } }, 
		{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 } }//J
}, { 
		{ { 0, 3 }, { 1, 3 }, { 2, 3 }, { 1, 2 } },//ㅗ
		{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 0, 2 } },//ㅏ
		{ { 0, 2 }, { 1, 2 }, { 2, 2 }, { 1, 3 } },//ㅜ
		{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 2 } } //ㅓ
}, { 
		{ { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } }, 
		{ { 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 } }, 
		{ { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } }, 
		{ { 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 } }//I
}, { 
		{ { 0, 3 }, { 1, 3 }, { 2, 3 }, { 2, 2 } }, 
		{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 0, 1 } }, 
		{ { 0, 2 }, { 1, 2 }, { 2, 2 }, { 0, 3 } }, 
		{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 3 } } //L
} };
Block::Block() {
	init(rand() % 7);
}

Point Block::zero = Point();

Block::Block(int type) {
	init(type);
}

Block::~Block() {
	// TODO Auto-generated destructor stub
}
void Block::init(int type) {
	stat = 0;
	color = type % 7;
	shape[0] = Block::block_shape[color][stat][0];
	shape[1] = Block::block_shape[color][stat][1];
	shape[2] = Block::block_shape[color][stat][2];
	shape[3] = Block::block_shape[color][stat][3];
	color += 41;
	current.x = LEFT + 3;
	current.y = TOP;
	prev = current;
	checkBound();
}

void Block::transpose() {
	int s = color - 41;
	stat = (stat + 1) % 4;
	shape[0] = Block::block_shape[s][stat][0];
	shape[1] = Block::block_shape[s][stat][1];
	shape[2] = Block::block_shape[s][stat][2];
	shape[3] = Block::block_shape[s][stat][3];
	checkBound();
}
void Block::reversTranspose() {

	int s = color - 41;
	stat = (stat + 3) % 4;
	shape[0] = Block::block_shape[s][stat][0];
	shape[1] = Block::block_shape[s][stat][1];
	shape[2] = Block::block_shape[s][stat][2];
	shape[3] = Block::block_shape[s][stat][3];
	checkBound();
}

void Block::moveUp() {

	if (current.y > TOP)
		current.y--;
}

void Block::moveDown() {

	if (current.y + bottom < BOTTOM)
		current.y++;

}
void Block::moveRight() {
	if (current.x + right < RIGHT)
		current.x++;
}
void Block::moveLeft() {
	if (current.x + left > LEFT)
		current.x--;

}
void Block::print() {
	for (int i = 0; i < 4; i++) {
		printColorString(shape[i].x * 2 + current.x * 2 + zero.x, shape[i].y + current.y + zero.y, color, BLOCK);
	}
	prev = current;
}
void Block::clear() {
	for (int i = 0; i < 4; i++) {
		printColorString(shape[i].x * 2 + prev.x * 2 + zero.x, shape[i].y + prev.y + zero.y, COLOR_DEFAULT, CLEAR);
	}
}

void Block::refresh() {

	clear();
	print();
	prev = current;
}

void Block::print(int x, int y) {
	for (int i = 0; i < 4; i++) {
		printColorString(shape[i].x * 2  + x , shape[i].y  + y , color, BLOCK);
	}
}

void Block::clear(int x, int y) {
	for (int i = 0; i < 4; i++) {
		printColorString(shape[i].x * 2 + x , shape[i].y  + y , COLOR_DEFAULT, CLEAR);
	}
}

void Block::checkBound() {
	int maxX = -10, minX = 10, maxY = -10;

	for (int i = 0; i < 4; i++) {
		if (shape[i].x > maxX) {
			maxX = shape[i].x;
		}
		if (shape[i].x < minX) {
			minX = shape[i].x;
		}
		if (shape[i].y > maxY) {
			maxY = shape[i].y;
		}
	}
	left = minX;
	right = maxX;
	bottom = maxY;
}
