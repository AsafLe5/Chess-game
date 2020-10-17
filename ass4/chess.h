#ifndef __ASS4_H
#define __ASS4_H
#define SIZE 8

void createBoard(char board[][SIZE], char fen[]);
void printBoard(char board[][SIZE]);
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn);
//void printBoardFromFEN(char fen[]);

#endif
