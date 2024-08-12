/*
 * GameLogic.cpp
 *
 *  Created on: 2011. 12. 7.
 *      Author: malloc
 */

#include "GameLogic.h"
#include <stdio.h>
#include "myio.h"
#include <unistd.h>
#include <time.h>
#include "network.h"
#include <stdlib.h>

#define UP 65
#define DOWN 66
#define RIGHT 67
#define LEFT 68

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETEROR() (errno)

GameLogic::GameLogic() {
	for (int i = 0; i < HEIGHT + 1; i++) {
		board[i] = new char[WIDTH];
		for (int j = 0; j < WIDTH; j++) {
			board[i][j] = 0;
		}
	}
	for (int j = 0; j < WIDTH; j++) {
		board[HEIGHT][j] = 1;
	}

	isRun = true;
	Block::zero.x = 6;
	Block::zero.y = 2;
}

GameLogic::~GameLogic() {
	// TODO Auto-generated destructor stub
	for (int i = 0; i < HEIGHT + 1; i++) {
		delete board[i];
	}
}

void GameLogic::start(char IP[], char PORT[]) {
	// Basic Game Setting
	isNotGameOver=true;
	unsigned char key;
	
	// Block Array
	blockTurn = 0;
	//block = Block();
	//next = Block();
	int n;
	myTurn = 1;
	score = 0;
	interval = 300000;
	clearLines = 0;
	level = 1;

	// IP address get
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *peer_address;
	if (getaddrinfo(IP, PORT, &hints, &peer_address)){
		printf("getaddrinfo() failed\n");
		return;
	}

	// Creating Socket
	SOCKET socket_peer;
	socket_peer = socket(peer_address -> ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer)){
		printf("socket() failed\n");
		return;
	}

	// Trying to connect socket
	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen)){
		printf("connect() failed\n");
		return;
	}

	//receive myTurn value and init block from server
	char message[1001];
	read(socket_peer, message, 1001);
	message[1000] = '\0';
	myTurn = (int) message[0] - 48;
	for (int i = 0; i < 1000; i ++){
		blockArray[i] = message[i+1];
	}
	block = Block(blockArray[blockTurn++]);
	next = Block(blockArray[blockTurn++]);

	//initial screen
	nextLevel();
	refresh();
	clock_t refresh1 = clock();
	clock_t current = refresh1;
	clock_t blockDown = refresh1;

	//game start
	while(isNotGameOver){

		//auto block down
		current = clock();
		if (current - refresh1 > 16000){
			refresh1 = current;
			printBlock();
		}
		if (current - blockDown > interval){
			moveDown();
			blockDown = current;
		}

		//select setting
		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(socket_peer, &reads);
		FD_SET(0, &reads);

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		if (select(socket_peer+1, &reads, 0, 0, &timeout) < 0){
			printf("select() failed\n");
			return;
		}

		//empty buffer
		fflush(stdout);
		fflush(stdin);

		//input command
		read(0, &n, 1);

		// sending message to server, MyTurn
		if(FD_ISSET(0, &reads) && (myTurn == 1)){
			myTurn = 0; //change turn
			key = n;
			switch(key){
			case 'Q': //game exit
				n = EOF;
				send(socket_peer, &key, 1, 0);
				isNotGameOver=false;
				this->isRun = false;
				break;
			case DOWN: //down
				moveDown();
				refreshMyTurn();
				send(socket_peer, &key, 1,0);
				n = EOF;
				break;
			case UP: //rotate
				transforse();
				refreshMyTurn();
				send(socket_peer, &key, 1,0);
				n = EOF;
				break;
			case RIGHT: //move right
				moveRight();
				refreshMyTurn();
				send(socket_peer, &key, 1,0);
				n = EOF;
				break;
			case LEFT: //move left
				moveLeft();
				refreshMyTurn();
				send(socket_peer, &key, 1,0);
				n = EOF;
				break;
			case ' ': //drop, ' ' = space
				drop();
				refreshMyTurn();
				send(socket_peer, &key, 1,0);
				n = EOF;
				break;
			default: //invalid command
				myTurn = 1;
				n = EOF;
				break;
			}
		}
		// receiving message from serer, OpponentsTurn
		else if (FD_ISSET(socket_peer, &reads) && (myTurn == 0)){
			myTurn = 1;// change turn

			//recieve other client's command from server
			char control;
			int bytes_received = recv(socket_peer, &control, 1, 0);
			n = control;
			key = n;
			if (n != EOF){
				switch(key){
				case 'Q':
					n = EOF;
					isNotGameOver=false;
					this->isRun = false;
					break;
				case UP:
					transforse();
					refreshMyTurn();
					n = EOF;
					break;
				case DOWN:
					moveDown();
					refreshMyTurn();
					n = EOF;
					break;
				case RIGHT:
					moveRight();
					refreshMyTurn();
					n = EOF;
					break;
				case LEFT:
					moveLeft();
					refreshMyTurn();
					n = EOF;
					break;
				case ' ':
					drop();
					refreshMyTurn();
					n = EOF;
					break; 
				default:
					n = EOF;
					myTurn = 0;
					break;
				}
			}
			
		}
		else gotoxy(80, 24);
	}
}

void GameLogic::printBlock() {
	block.refresh();
}



void GameLogic::moveDown() {
	block.moveDown();
	if (isCollision()) {
		block.moveUp();
		//보드에 저장
		for (int i = 0; i < 4; i++) {
			board[block.current.y + block.shape[i].y][block.current.x + block.shape[i].x] = block.color;
		}
		nextTurn();
	}
}

void GameLogic::moveLeft() {
	block.moveLeft();
	if (isCollision())
		block.moveRight();
}

void GameLogic::moveRight() {
	block.moveRight();
	if (isCollision())
		block.moveLeft();
}

void GameLogic::transforse() {
	Block temp = block;
	block.transpose();
	if (isCollision()) {
		block.reversTranspose();
	} else {
		temp.clear();
		block.print();
	}
}

void GameLogic::drop() {

	while (!isCollision())
		block.moveDown();

	block.moveUp();

	nextTurn();
}

bool GameLogic::isCollision() {
	int x, y;
	for (int i = 0; i < 4; i++) {
		x = block.current.x + block.shape[i].x;
		y = block.current.y + block.shape[i].y;

		if (x >= WIDTH || y > HEIGHT || x < 0) {
			return true;
		}

		if (board[y][x]) {
			return true;
		}
	}
	return false;
}

void GameLogic::clearLine(int l) {
	char *line = board[l];
	for (int y = l; y > 0; y--) {
		board[y] = board[y - 1];
	}

	board[0] = line;
	for (int x = 0; x < WIDTH; x++) {
		board[0][x] = 0;
	}

}
void GameLogic::gameOver() {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			board[y][x] = 0;
		}
	}
	refresh();
	
	printColorString(Block::zero.x, Block::zero.y + 10, COLOR_BLACK, "gameover");
	isNotGameOver=false;

}
void GameLogic::gameClear() {

}
void GameLogic::nextTurn() {
	//보드에 저장
	for (int i = 0; i < 4; i++) {
		board[block.current.y + block.shape[i].y][block.current.x + block.shape[i].x] = block.color;
	}
	block.refresh();
	check(block);
	//새블럭 생성
	block = next;
	next = Block(blockArray[blockTurn++]);
	refreshNextBlock();
	refreshMyTurn();
	if (isCollision()) {
		gameOver();
	}
}

void GameLogic::check(Block block) {

	bool needClear = false;
	bool removeable;
	int lineCount = 0;
	int y = block.current.y + block.bottom;

	for (int i = 0; i < 4; i++) {
		removeable = true;
		for (int x = 0; x < WIDTH; x++) {
			if (!board[y][x]) {
				removeable = false;
				break;
			}
		}
		if (removeable) {
			clearLine(y);
			needClear = true;
			lineCount++;
		} else {
			y--;
		}
	}
	switch (lineCount) {
	case 1:
		score += 100;
		break;
	case 2:
		score += 300;
		break;
	case 3:
		score += 500;
		break;
	case 4:
		score += 800;
		break;
	default:
		break;
	}
	clearLines += lineCount;
	char l[4];
	sprintf(l, "%d", clearLines);
	printColorString(45, 15, 0, l);
	if (clearLines >= 10) {
		nextLevel();

	}
	if (needClear)
		refresh();

}

void GameLogic::refresh() {

	for (int i = 0; i < HEIGHT; i++) {
		gotoxy(Block::zero.x - 2, Block::zero.y + i);
		printf("■");

		for (int x = 0; x < WIDTH; x++) {
			if (board[i][x]) {
				printColorString(x * 2 + Block::zero.x, Block::zero.y + i, board[i][x], Block::BLOCK);
			} else {
				printColorString(x * 2 + Block::zero.x, Block::zero.y + i, board[i][x], Block::CLEAR);
			}
		}
		gotoxy(Block::zero.x + WIDTH * 2, Block::zero.y + i);
		printf("■");
	}
	gotoxy(Block::zero.x - 2, Block::zero.y + HEIGHT);
	printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■");
	refreshNextBlock();
	refreshMyTurn();
	refreshPoint();

}

void GameLogic::refreshNextBlock() {
	printColorString(45, 7, 0, "┌────────┐");
	for (int i = 0; i < 4; i++) {
		printColorString(45, 8 + i, 0, "│        │");
	}
	printColorString(45, 12, 0, "└────────┘");
	next.clear(46, 8);
	next.print(46, 8);
}

void GameLogic::refreshPoint() {
	printColorString(45, 5, 0, "SCORE");
	char score[10];
	sprintf(score, "%08d", this->score);
	printColorString(45, 6, 0, score);
}

void GameLogic::refreshMyTurn(){
	if (myTurn == 1) printColorString(45, 17, 0, "My Turn            ");
	else printColorString(45, 17, 0, "Opponent's Turn");
}

void GameLogic::nextLevel() {
	printColorString(45, 3, 0, "LEVEL");
	char score[10];
	sprintf(score, "%2d", level);
	printColorString(45, 4, 0, score);
	clearLines = 0;
	interval -= 5000;
	if (interval <= 0) {
		gameClear();
	}
}
