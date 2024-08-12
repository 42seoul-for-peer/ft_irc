#include <stdio.h>
#include "myio.h"

void printColorString(int x, int y, int color, char * str) {
	if ((color >= 40 && color <= 47) || color == 0) {
		gotoxy(x, y);
		printf("\033[%dm%s\033[0m", color, str);

	}
}
void printColorString(int x, int y, int color, const char * str) {
	if ((color >= 40 && color <= 47) || color == 0) {
		gotoxy(x, y);
		printf("\033[%dm%s\033[0m", color, str);

	}
}
void gotoxy(int x, int y) {
	printf("\033[%d;%df", y, x);
	fflush(stdout);
	fflush(stdin);
}
