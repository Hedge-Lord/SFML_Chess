#ifndef BOARD_H
#define BOARD_H 1
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <iostream>

class Board;

struct Piece {
	Piece(int id, int pos, Board* board);
	int id;
	int pos;
	sf::Sprite sprite;
	sf::Texture texture;
	bool moved = false;
	Board* board;
	void setPiecePosition(int newPos);
};

struct Move {
	sf::Vector2i startPos;
	sf::Vector2i endPos;
	int dx, dy, start, end;
	Move(int start, int end) : start(start), end(end), startPos(start % 8 + 1, start / 8 * -1 + 8), endPos(end % 8 + 1, start / 8 * -1 + 8) {
		dx = Move::end % 8 - Move::start % 8;
		dy = Move::start / 8 - Move::end / 8;
	}
};

class Board {
public:
	Board(bool whitePlayer);
	Piece* getPieceAt(int pos);
	bool isLegalMove(Piece* mover, int pos);
	void display(sf::RenderWindow* window, Piece* dragged);
	void displayFrame(int pos);
	void undisplayFrame() { frameDisplayed = false; }
	sf::Texture getTexture(int id);
	int soundToPlay = 0;
	std::vector<Move> getLegalMoves();
	sf::Vector2f getPixelPosition(int pos);
	int end = 0;
	bool checking = false;
	bool whitePlaying = true;
	bool whiteToMove = true;
	int depth = 3;

	int evaluate();
	int search(int depth, int alpha, int beta);
	void orderMoves(std::vector<Move> moves);
	Move findMove(int depth);
	Piece* makeMove(Move move, Piece* piece);
	void undoMove(Move move, Piece* piece, Piece* captured);

	int board[64] = { 10, 8, 9, 11, 12, 9, 8, 10,
		  7,  7, 7, 7,  7,  7, 7, 7,
		  0, 0, 0, 0, 0, 0, 0, 0,
		  0, 0, 0, 0, 0, 0, 0, 0,
		  0, 0, 0, 0, 0, 0, 0, 0,
		  0, 0, 0, 0, 0, 0, 0, 0,
		  1, 1, 1, 1, 1, 1, 1, 1,
		  4, 2, 3, 5, 6, 3, 2, 4 };
private:

	sf::Sprite boardSprite;
	sf::Texture boardTexture;
	sf::Sprite frameSprite;
	std::vector<Piece*> pieces;
	sf::Texture textures[14];
	bool frameDisplayed = false;

	// move logic
	Piece* whiteKing;
	Piece* blackKing;
	int enPassantable = -1;
	bool validPassant = false;
	bool pawnPromotion = false;
	bool whitePawnMove(Move move);
	bool blackPawnMove(Move move);
	bool rookMove(Move move);
	bool queenMove(Move move);
	bool bishopMove(Move move);
	bool knightMove(Move move);
	bool kingMove(Move move);
	bool isKingAttacked(int id, int start, int end);
	int searchDirection(int start, int dir, std::function<int(int)> endSearch);
};



#endif