/*
 * Block.h
 *
 *  Created on: 2011. 12. 6.
 *      Author: malloc
 */

#ifndef BLOCK_H_
#define BLOCK_H_

struct Point {
	int x, y;
};

class Block {
private:
	static const int LEFT,RIGHT,TOP,BOTTOM;
	static const Point block_shape[][4][4];
	int stat;
	Point prev;
	void init(int type);
public:
	static const char * BLOCK;
	static const char * CLEAR;
	static Point zero;
	Point current;
	int left;
	int right;
	int bottom;
	int color;
	Point shape[4];
	//block moving
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void transpose();
	void reversTranspose();
	void print(int x, int y);
	void clear(int x, int y);
	void refresh();
	void print();
	void clear();
	Block();
	Block(int type);
	virtual ~Block();
private:
	void checkBound();
};

#endif /* BLOCK_H_ */
