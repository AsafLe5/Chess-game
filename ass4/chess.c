#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "chess.h"
#define MAX_PGN_LEN 7

/***********
* Asaf Levi
* 205543317
* 01
* ass4
***********/

const int INVALID = -1;
const int FALSE = 0;
// Move logical representation
typedef struct {
	char srcPiece, srcRow, srcCol, destRow, destCol, destPiece, promotionPiece;
	int iSrc, jSrc, iDest, jDest;
	int isWhite, isCapture, isPromotion, isCheck, isMate, isLegal;
} Move;

//initialization of the struct.
void initializeMove(Move* move) {
	move->srcPiece = INVALID;
	move->srcRow = INVALID;
	move->srcCol = INVALID;
	move->destPiece = INVALID;
	move->destRow = INVALID;
	move->destCol = INVALID;
	move->promotionPiece = INVALID;
	move->iSrc = INVALID;
	move->jSrc = INVALID;
	move->iDest = INVALID;
	move->jDest = INVALID;
	move->isWhite = FALSE;
	move->isCapture = FALSE;
	move->isPromotion = FALSE;
	move->isCheck = FALSE;
	move->isMate = FALSE;
	move->isLegal = FALSE;
}

// PGN characters
const char PAWN = 'P';
const char ROOK = 'R';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char QUEEN = 'Q';
const char KING = 'K';
const char CAPTURE = 'x';
const char PROMOTION = '=';
const char CHECK = '+';
const char MATE = '#';
const char FIRST_COL = 'a';

// FEN & Board characters
const char WHITE_PAWN = 'P';
const char WHITE_ROOK = 'R';
const char WHITE_KNIGHT = 'N';
const char WHITE_BISHOP = 'B';
const char WHITE_QUEEN = 'Q';
const char WHITE_KING = 'K';
const char BLACK_PAWN = 'p';
const char BLACK_ROOK = 'r';
const char BLACK_KNIGHT = 'n';
const char BLACK_BISHOP = 'b';
const char BLACK_QUEEN = 'q';
const char BLACK_KING = 'k';

// FEN separator for strtok()
const char SEP[] = "/";

// Board characters
const char EMPTY = ' ';

/**************************
 * convert char to digit
 **************************/
int toDigit(char c) {
	assert('0' <= c && c <= '9');
	return c - '0';
}

/*
Function Name: printColumns
Input: none
Output: none
Function Operation: The function prints the columns.
*/
void printColumns() {
	char column = toupper(FIRST_COL);
	printf("* |");
	for (int i = 0; i < SIZE; i++) {
		if (i) 
			printf(" ");
		printf("%c", column);
		column++;
	}
	printf("| *\n");
}

/*
Function Name: printSpacers
Input: none
Output: none
Function Operation: The function prints the spacers.
*/
void printSpacers() {
	printf("* -");
	for (int i = 0; i < SIZE; i++)
		printf("--");
	printf(" *\n");
}
/*
Function Name: printRow
Input: char[] , int.
Output: none
Function Operation: The function prints the raws.
*/
void printRow(char row[], int rowIdx) {
	printf("%d ", rowIdx);
	int i = 0;
	while (row[i]) {
		if (isdigit(row[i])) {
			int spaces = toDigit(row[i++]);
			for (int j = 0; j < spaces; j++) 
				printf("|%c", EMPTY);
		}
		else 
			printf("|%c", row[i++]);
	}
	printf("| %d\n", rowIdx);
}
/*
Function Name: createBoard
Input: char[][], char[]
Output: none
Function Operation: The function create the chess board with the fen.
*/
void createBoard(char board[][SIZE], char fen[]) {
	int fenIdx = 0, needInc = 0;
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++) {
			if (fen[fenIdx] == '/')
				fenIdx++;
			if (isdigit(fen[fenIdx])) {
				for (int k = j; k < fen[fenIdx] - '0' + j; k++) {
					board[i][k] = ' ';
					needInc++;
				}
				fenIdx++;
			} else {
				board[i][j] = fen[fenIdx];
				fenIdx++;
			}
			if (needInc > 1)
				j += needInc - 1;
			needInc = 0;
		}
}
/*
Function Name: printBoard
Input: char[][]
Output: none
Function Operation: The function prints the chess board for a given board.
*/
void printBoard(char board[][SIZE]) {
	printColumns();
	printSpacers();
	for (int i = 0; i < SIZE; i++) {
		printf("%d ", SIZE - i);
		for (int j = 0; j < SIZE; j++)
			printf("|%c", board[i][j]);
		printf("| %d", SIZE - i);
		printf("\n");
	}
	printSpacers();
	printColumns();
}

/*
the next two funcions:
Function's Names: getSourceISpot/getSourceJSpot
Input: char[], Move* 
Output: int
Function Operation: The function return the I/J source index of a given pgn.
*/
int getSourceISpot(char pgn[], Move* move) {
	int i = 0, counter = 0;
	while (pgn[i] != '\0') {
		//checks if its two time getting y spot so it must be the first one.
		if (pgn[i] >= '0' && pgn[i] <= '9')
			counter++;
		i++;
	}
	i = 0;
	if (counter == 2) {
		while (pgn[i] != '\0') {
			if (pgn[i] >= '0' && pgn[i] <= '9') {
				move->iSrc = SIZE - (pgn[i] - '0');
				return 1;
			}
			i++;
		}
	}
	return 0;
}
int getSourceJSpot(char pgn[], Move* move) {
	int i = 0, counter = 0;
	while (pgn[i] != '\0') {
		//checks if its two time getting x spot so it must be the first one.
		if (pgn[i] >= 'a' && pgn[i] <= 'i')
			counter++;
		i++;
	}
	i = 0;
	if (counter == 2) {
		while (pgn[i] != '\0') {
			if (pgn[i] >= 'a' && pgn[i] <= 'i') {
				move->jSrc = pgn[i] - 'a';
				return 1;
			}
			i++;
		}
	}
	return 0;
}

/*
the next seven funcions:
Function's Names: isValidKingStep/isValidQueenStep/isValidRookStep/isValidBishopStepisValidKnightStep/
				  isValidKnightStep/isValidWhitePawnStep/isValidBlackPawnStep
Input: Move*
Output: int
Function Operation: The function return 1 if its a valid step per each piece or 0 if it isn't.
*/
int isValidKingStep(Move* move) {
	if (move->iSrc == move->iDest && (move->jSrc == move->jDest + 1 || move->jSrc == move->jDest - 1))
		return 1;
	else if (move->jSrc == move->jDest && (move->iSrc == move->iDest + 1 || move->iSrc == move->iDest - 1))
		return 1;
	else if ((move->jSrc == move->jDest + 1 || move->jSrc == move->jDest - 1) &&
		(move->iSrc == move->iDest + 1 || move->iSrc == move->iDest - 1))
		return 1;
	else
		return 0;
}
int isValidQueenStep(Move* move) {
	if (move->iSrc == move->iDest && (move->jSrc > move->jDest || move->jSrc < move->jDest))
		return 1;
	if (move->jSrc == move->jDest && (move->iSrc > move->iDest || move->iSrc < move->iDest))
		return 1;
	if (abs(move->jSrc - move->jDest) == abs(move->iSrc - move->iDest)) 
			return 1;
	return 0;
}
int isValidRookStep(Move* move) {
	if (move->iSrc == move->iDest && (move->jSrc > move->jDest || move->jSrc < move->jDest))
		return 1;
	else if (move->jSrc == move->jDest && (move->iSrc > move->iDest || move->iSrc < move->iDest))
		return 1;
	else
		return 0;
}
int isValidBishopStep(Move* move) {
	if (abs(move->jSrc - move->jDest) == (abs(move->iSrc - move->iDest)))
		return 1;
	else
		return 0;
}
int isValidKnightStep(Move* move) {
	if ((move->jSrc == move->jDest + 2 || move->jSrc == move->jDest - 2) &&
		(move->iSrc == move->iDest + 1 || move->iSrc == move->iDest - 1))
		return 1;
	if ((move->iSrc == move->iDest + 2 || move->iSrc == move->iDest - 2) &&
		(move->jSrc == move->jDest + 1 || move->jSrc == move->jDest - 1))
		return 1;
	else
		return 0;
}
int isValidWhitePawnStep(Move* move) {
	if (move->isCapture) {
		if (move->iSrc - 1 == move->iDest) {
			if ((move->jSrc + 1 == move->jDest) || (move->jSrc - 1 == move->jDest))
				return 1;
		}
		return 0;
	}
	//in case the pawn is in his first raw, he can jump two spots.
	if (move->iSrc == SIZE - 2) {
		if (move->jDest == move->jSrc && move->iSrc == move->iDest + 2)
			return 1;
	}
	if (move->jSrc == move->jDest && move->iSrc == move->iDest + 1)
		return 1;
	else
		return 0;
}
int isValidBlackPawnStep(Move* move) {
	if (move->isCapture) {
		if (move->iSrc + 1 == move->iDest) {
			if ((move->jSrc + 1 == move->jDest) || (move->jSrc - 1 == move->jDest))
				return 1;
		}
		return 0;
	}
	//in case the pawn is in his first raw, he can jump two spots.
	if (move->iSrc == 1)
		if (move->jDest == move->jSrc && move->iSrc == move->iDest - 2)
			return 1;
	if (move->jSrc == move->jDest && move->iSrc == move->iDest - 1)
		return 1;
	else
		return 0;
}


/*
Function Name: isDiagonalClear
Input: char[][], Move*
Output: int
Function Operation: The function checks for a given source and destination if the path is diagonally clear 
*/
int isDiagonalClear(char board[][SIZE], Move* move) {
	if (move->srcPiece == WHITE_KNIGHT || move->srcPiece == BLACK_KNIGHT)
		return 1;
	//check whether its a diagonal or not
	if (abs(move->iSrc - move->iDest) != abs(move->jSrc - move->jDest))
		return -1;
	int iSign, jSign, length;
	if (move->iSrc > move->iDest) {
		iSign = -1;
		length = move->iSrc - move->iDest;
	} else {
		iSign = 1;
		length = move->iDest - move->iSrc;
	}
	if (move->jSrc > move->jDest)
		jSign = -1;
	else
		jSign = 1;
	for (int i = 1; i < length; i++) {
		if (board[move->iSrc + i * iSign][move->jSrc + i * jSign] != EMPTY)
			return 0;
	}
	return 1;
}
/*
Function Name: isHorizontalOrVerticalClear
Input: char[][], Move*
Output: int
Function Operation: The function checks for a given source and destination
					if the path is horizontal or vertical clear.
*/
int isHorizontalOrVerticalClear(char board[][SIZE], Move* move) {
	if (move->srcPiece == WHITE_KNIGHT || move->srcPiece == BLACK_KNIGHT)
		return 1;
	int length;
	//checks whether the move is horizontal or vertical move.
	if (!((move->iSrc == move->iDest && (move->jSrc > move->jDest || move->jSrc < move->jDest)) ||
		(move->jSrc == move->jDest && (move->iSrc > move->iDest || move->iSrc < move->iDest))))
		return -1;
	int sign;
	if (move->iSrc > move->iDest || move->jSrc > move->jDest) {
		sign = -1;
	}
	else
		sign = 1;
	if (move->iSrc != move->iDest)
		length = abs(move->iSrc - move->iDest);
	else
		length = abs(move->jSrc - move->jDest);
	if (move->iSrc != move->iDest) {
		for (int i = 1; i < length; i++) {
			if (board[move->iSrc + i * sign][move->jSrc] != EMPTY)
				return 0;
		}
	}
	else
		for (int i = 1; i < length; i++) {
			if (board[move->iSrc][move->jSrc + i * sign] != EMPTY)
				return 0;
		}
	return 1;
}
/*
Function Name: isDestinationClear
Input: char[][], Move*
Output: int
Function Operation: The function checks if a given destination is clear.
*/
int isDestinationClear(char board[][SIZE], Move* move) {
	if (board[move->iDest][move->jDest] == EMPTY)
		return 1;
	return 0;
}
/*
Function Name: isValidCapture
Input: char[][], Move*
Output: int
Function Operation: The function checks if a specific capture if its the differnt color piece.
*/
int isValidCapture(char board[][SIZE], Move* move) {
	//for everyother piece its just have to be the other color piece. 
	if (move->isWhite && isupper(move->destPiece))
		return 0;
	if (!move->isWhite && islower(move->destPiece))
		return 0;
	if (board[move->iDest][move->jDest] == EMPTY)
		return 0;
	return 1;

}

int isPromotionValid(Move* move) {
	if ((move->isWhite && move->iDest == 0) || (!move->isWhite && move->iDest == SIZE - 1))
		if (move->isPromotion && move->promotionPiece)
				return 1;
	return 0;
}

/*
Function Name: isValidStep
Input: Move*
Output: int
Function Operation: The function organize all the isValidStep functions in to one,
					so it knows which function to call for a specific piece.
*/
int isValidStep(Move* move) {
	int isLegitMove = 0;
	if (move->srcPiece == BLACK_KING || move->srcPiece == WHITE_KING)
		isLegitMove = isValidKingStep(move);
	else if (move->srcPiece == BLACK_QUEEN || move->srcPiece == WHITE_QUEEN)
		isLegitMove = isValidQueenStep(move);
	else if (move->srcPiece == BLACK_ROOK || move->srcPiece == WHITE_ROOK)
		isLegitMove = isValidRookStep(move);
	else if (move->srcPiece == BLACK_BISHOP || move->srcPiece == WHITE_BISHOP)
		isLegitMove = isValidBishopStep(move);
	else if (move->srcPiece == BLACK_KNIGHT || move->srcPiece == WHITE_KNIGHT)
		isLegitMove = isValidKnightStep(move);
	else if (move->srcPiece == BLACK_PAWN)
		isLegitMove = isValidBlackPawnStep(move);
	else if (move->srcPiece == WHITE_PAWN)
		isLegitMove = isValidWhitePawnStep(move);

	return isLegitMove;
}
/*
Function Name: isTotalValid
Input: char[][], Move*
Output: int
Function Operation: The function organize all the isValid functions in to one,
					so it knows which function to call for a specific valid check.
*/
int isTotalValid(char board[][SIZE], Move* move) {
	if (!isValidStep(move)) {
		return 0;
	}
	if (isDiagonalClear(board, move) != -1) {
		if (!isDiagonalClear(board, move)) {
			return 0;
		}
	}
	if (isHorizontalOrVerticalClear(board, move) != -1) {
		if (!isHorizontalOrVerticalClear(board, move)) {
			return 0;
		}
	}
	if (move->isCapture)
		return isValidCapture(board, move);
	if (!move->isCapture) {
		if (board[move->iDest][move->jDest] != EMPTY)
			return 0;
	}
	if (move->isPromotion)
		if (!isPromotionValid(move))
			return 0;
	return 1;
}

/*
the next two funcions:
Function's Names: getDestinationISpot/getDestinationJSpot
Input: char[], Move*
Output: none
Function Operation: The function gets the I/J index and put it in the struct.
*/
void getDestinationISpot(char pgn[], Move* move) {
	int i = 0, counter = 0;
	//if there is only ones a digit in the pgn then it got to be the destination's i. 
	if (!getSourceISpot(pgn, move)) {
		while (pgn[i] != '\0') {
			if (pgn[i] >= '0' && pgn[i] <= '9')
				move->iDest = SIZE - (pgn[i] - '0');
			i++;
		}
	//if its two times, then its got to be the second time
	} else {
		while (pgn[i] != '\0') {
			if (pgn[i] >= '0' && pgn[i] <= '9') {
				if (counter == 1)
					move->iDest = SIZE - (pgn[i] - '0');
				counter++;
			}
			i++;
		}
	}
}
void getDestinationJSpot(char pgn[], Move* move) {
	int i = 0, counter = 0;
	//if there is only ones a lowercase letter in the pgn then it got to be the destination's j. 
	if (!getSourceJSpot(pgn, move)) {
		while (pgn[i] != '\0') {
			if (pgn[i] >= 'a' && pgn[i] <= 'i') {
				move->jDest = (pgn[i] - 'a');
			}
			i++;
		}
	//if its two times, then its got to be the second time
	} else {
		while (pgn[i] != '\0') {
			if (pgn[i] >= 'a' && pgn[i] <= 'i') {
				if (counter == 1) {
					move->jDest = pgn[i] - 'a';
				}
				counter++;
			}
			i++;
		}
	}
}

/*
the next four funcions:
Function's Names: isCapture/isPromotion/isCheck/isMate
Input: char[]
Output: int
Function Operation: The function checks from the pgn if a move is capture/promotion/check/mate.
*/
int isCapture(char pgn[]) {
	int i = 0;
	while (pgn[i] != '\0') {
		if (pgn[i] == CAPTURE) {
			return 1;
		}
		i++;
	}
	return 0;
}
int isPromotion(char pgn[]) {
	int i = 0;
	while (pgn[i] != '\0') {
		if (pgn[i] == PROMOTION)
			return 1;
		i++;
	}
	return 0;
}
int isCheck(char pgn[]) {
	int i = 0;
	while (pgn[i] != '\0') {
		if (pgn[i] == CHECK)
			return 1;
		i++;
	}
	return 0;
}
int isMate(char pgn[]) {
	int i = 0;
	while (pgn[i] != '\0') {
		if (pgn[i] == MATE)
			return 1;
		i++;
	}
	return 0;
}

/*
Function Name: promotionPiece
Input: char[], Move*
Output: char
Function Operation: The function return the piece type which is desire to be promote to.
*/
char promotionPiece(char pgn[], Move* move) {
	if (isPromotion(pgn)) {
		if (move->srcPiece != BLACK_PAWN && move->srcPiece != WHITE_PAWN)
			return 0;
		int i = 0;
		while (pgn[i] != '\0') {
			if (pgn[i] == PROMOTION)
				return pgn[i + 1];
			i++;
		}
	}
	return 0;
}

/*
Function Name: getPieceTypeInDestination
Input: char[][], Move*
Output: char
Function Operation: The function return the piece type which is in the destination spot.
*/
char getPieceTypeInDestination(char board[][SIZE], Move* move) {
	if (board[move->iDest][move->jDest] != EMPTY)
		return board[move->iDest][move->jDest];
	else
		return 0;
}

/*
Function Name: concludePiece
Input: char[][], Move*, int
Output: none
Function Operation: The function conclude the piece from a given pgn and puts it in the struct.
*/
void concludePiece(char pgn[], Move* move, int isWhiteTurn) {
	//checks which player is it making the move, it always the first char (if there is) of the pgn so its pgn[0].
	if (isupper(pgn[0])) {
		if (isWhiteTurn)
			move->srcPiece = pgn[0];
		else
			move->srcPiece = pgn[0] + ('a' - 'A');
	}
	else {
		if (isWhiteTurn)
			move->srcPiece = WHITE_PAWN;
		else
			move->srcPiece = BLACK_PAWN;
	}
}

/*
the next seven funcions:
Function's Names: concludeSource/concludeISourceWithJ/concludeJSourceWithI
Input: char[][], Move*
Output: none
Function Operation: The functions conclude the source I/J/both and puts in the struct.
*/
void concludeSource(char board[][SIZE], Move* move) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (board[i][j] == move->srcPiece) {
				move->iSrc = i;
				move->jSrc = j;
				if (isTotalValid(board, move)) {
					return;
				}
			}
		}
	}
}
void concludeISourceWithJ(char board[][SIZE], Move* move) {
	for (int i = 0; i < SIZE; i++) {
		if (board[i][move->jSrc] == move->srcPiece) {
			move->iSrc = i;
			if (isTotalValid(board, move)) {
				return;
			}
		}
	}
}
void concludeJSourceWithI(char board[][SIZE], Move* move) {
	for (int j = 0; j < SIZE; j++) {
		if (board[move->iSrc][j] == move->srcPiece) {
			move->jSrc = j;
			if (isTotalValid(board, move)) {
				return;
			}
		}
	}
}

/*
Function Name: pushToStruct
Input: char[][], char[], Move*, int
Output: none
Function Operation: The function call the right function for each value and puts it in the struct.
*/
void pushToStruct(char board[][SIZE], char pgn[], Move* move, int isWhiteTurn) {
	//Is the current turn white
	move->isWhite = isWhiteTurn;
	//Is it says that there is a capture
	move->isCapture = isCapture(pgn);
	//piece's destination i index
	getDestinationISpot(pgn, move);
	//piece's destination j index
	getDestinationJSpot(pgn, move);
	//the piece that's in this spot
	move->destPiece = getPieceTypeInDestination(board, move);
	//the piece that make the move.
	concludePiece(pgn, move, isWhiteTurn);
	//piece's source i and j index
	if (!getSourceISpot(pgn, move) && !getSourceJSpot(pgn, move))
		concludeSource(board, move);
	else if (!getSourceISpot(pgn, move))
		concludeISourceWithJ(board, move);
	else if (!getSourceJSpot(pgn, move))
		concludeJSourceWithI(board, move);
	//Is it says that there is a promotion
	move->isPromotion = isPromotion(pgn);
	//which piece to promote to
	move->promotionPiece = promotionPiece(pgn, move);
	//Is it says that there is a check
	move->isCheck = isCheck(pgn);
	//Is it says that there is a mate
	move->isMate = isMate(pgn);
	//is the current move legal
	move->isLegal = isTotalValid(board, move);
}

/*
Function Name: amICheckThreatened
Input: char[][], Move*, char[]
Output: int
Function Operation: The function checks whether in the currnent move some 
					piece of the other team can capture the king. 
*/
int amICheckThreatened(char board[][SIZE], Move* move, char pgn[]) {
	int kingILocation, kingJLocation;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if ((move->isWhite && board[i][j] == WHITE_KING) || (!move->isWhite && board[i][j] == BLACK_KING)) {
				kingILocation = i;
				kingJLocation = j;
			}
		}
	}
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if ((move->isWhite && islower(board[i][j])) || (!move->isWhite && isupper(board[i][j]))) {
				Move tempMove;
				initializeMove(&tempMove);
				/*
				creating a new pgn and temp move struct to send back as a regular pgn to check 
				from the opposite move perspective if its legal move to capure the king, using
				all the "is valid" functions written until now.
				*/
				char tempPgn[MAX_PGN_LEN];
				if (move->isWhite)
					tempPgn[0] = board[i][j] - ('a' - 'A');
				else
					tempPgn[0] = board[i][j];
				tempPgn[1] = (SIZE - i) + '0';
				tempPgn[2] = j + 'a';
				tempPgn[3] = CAPTURE;
				tempPgn[4] = (SIZE - kingILocation) + '0';
				tempPgn[5] = kingJLocation + 'a';
				tempPgn[6] = '\0';
				pushToStruct(board, tempPgn, &tempMove, !move->isWhite);
				if (tempMove.isLegal) {
					return 1;
				}
			}
		}
	}
	return 0;
}

int doICheckThreat(char board[][SIZE], Move* move, char pgn[]) {
	int kingILocation, kingJLocation;
	char tempBoard[SIZE][SIZE];
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			tempBoard[i][j] = board[i][j];
			if ((move->isWhite && board[i][j] == BLACK_KING) || (!move->isWhite && board[i][j] == WHITE_KING)) {
				kingILocation = i;
				kingJLocation = j;
			}
		}
	}
	tempBoard[move->iDest][move->jDest] = move->srcPiece;
	tempBoard[move->iSrc][move->jSrc] = EMPTY;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if ((!move->isWhite && islower(tempBoard[i][j])) || (move->isWhite && isupper(tempBoard[i][j]))) {
				Move tempMove;
				initializeMove(&tempMove);
				/*
				creating a new pgn and temp move struct to send back as a regular pgn to check
				from the opposite move perspective if its legal move to capure the king, using
				all the "is valid" functions written until now.
				*/
				char tempPgn[MAX_PGN_LEN];
				if (!move->isWhite)
					tempPgn[0] = tempBoard[i][j] - ('a' - 'A');
				else
					tempPgn[0] = tempBoard[i][j];
				tempPgn[1] = (SIZE - i) + '0';
				tempPgn[2] = j + 'a';
				tempPgn[3] = CAPTURE;
				tempPgn[4] = (SIZE - kingILocation) + '0';
				tempPgn[5] = kingJLocation + 'a';
				tempPgn[6] = '\0';
				pushToStruct(tempBoard, tempPgn, &tempMove, move->isWhite);
				if (tempMove.isLegal) {
					return 1;
				}
			}
		}
	}
	return 0;
}

/*
Function Name: updateBoard
Input: char[][], Move*
Output: none
Function Operation: The function update the board with the new move.
*/
void updateBoard(char board[][SIZE], Move* move) {
	if (!(move->isPromotion)) {
		board[move->iDest][move->jDest] = board[move->iSrc][move->jSrc];
		board[move->iSrc][move->jSrc] = EMPTY;
	}
	else {
		if (move->isWhite)
			board[move->iDest][move->jDest] = move->promotionPiece;
		else
			board[move->iDest][move->jDest] = tolower(move->promotionPiece);
		board[move->iSrc][move->jSrc] = EMPTY;
	}
}

/*
Function Name: makeMove
Input: char[][], char[],  int
Output: int
Function Operation: The function makes the move if its a valid move and return 1,
					if it isn't valid move, it doesn't make the move and return 0.
*/
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn) {
	Move move;
	char tempBoard[SIZE][SIZE] = { 0 };
	initializeMove(&move);
	pushToStruct(board, pgn, &move, isWhiteTurn);
	if (move.isLegal) {
		//creating a temporary board to check if the current move makes the king exposed. 
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				tempBoard[i][j] = board[i][j];
			}
		}
		updateBoard(tempBoard, &move);
		if (amICheckThreatened(tempBoard, &move, pgn))
			return 0;
		if (!move.isCheck && !move.isMate && doICheckThreat(board, &move, pgn))
			return 0;
		updateBoard(board, &move);
		return 1;
	}
	return 0;
}