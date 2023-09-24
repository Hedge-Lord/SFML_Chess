#include "board.h"


Board::Board() {
	boardTexture.loadFromFile("Assets/chessboard.png");
	boardSprite.setTexture(boardTexture);
	textures[0].loadFromFile("Assets/chessboard.png");
	boardSprite.setTexture(textures[0]);
	textures[1].loadFromFile("Assets/Chess_plt60.png");
	textures[2].loadFromFile("Assets/Chess_Nlt60.png");
	textures[3].loadFromFile("Assets/Chess_blt60.png");
	textures[4].loadFromFile("Assets/Chess_rlt60.png");
	textures[5].loadFromFile("Assets/Chess_qlt60.png");
	textures[6].loadFromFile("Assets/Chess_klt60.png");
	textures[7].loadFromFile("Assets/Chess_pdt60.png");
	textures[8].loadFromFile("Assets/Chess_Ndt60.png");
	textures[9].loadFromFile("Assets/Chess_bdt60.png");
	textures[10].loadFromFile("Assets/Chess_rdt60.png");
	textures[11].loadFromFile("Assets/Chess_qdt60.png");
	textures[12].loadFromFile("Assets/Chess_kdt60.png");
	textures[13].loadFromFile("Assets/frame.png");

	for (int i = 0; i < 64; i++) {
		if (board[i]) {
			pieces.push_back(new Piece(board[i], i, this));
			if (board[i] == 6) whiteKing = pieces[pieces.size() - 1];
			if (board[i] == 12) blackKing = pieces[pieces.size() - 1];
		}
	}
	frameSprite.setTexture(textures[13]);
	sf::Color color(255, 255, 255, 128);
	frameSprite.setColor(color);
}

void Board::display(sf::RenderWindow* window, Piece* dragged) {
	window->draw(boardSprite);
	if (frameDisplayed) window->draw(frameSprite);
	if (dragged) {
		for (int i = 0; i < pieces.size(); i++) {
			if (pieces[i] == dragged) continue;
			window->draw(pieces[i]->sprite);
		}
		window->draw(dragged->sprite);
	}
	else {
		for (int i = 0; i < pieces.size(); i++) {
			window->draw(pieces[i]->sprite);
		}
	}
}

Piece* Board::getPieceAt(int pos) {
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i]->pos == pos) return pieces[i];
	}
	return 0;
}

sf::Texture Board::getTexture(int id) { return textures[id]; }

Piece::Piece(int id, int pos, Board* board) : id(id), pos(pos), board(board) {
	texture = board->getTexture(id);
	sprite.setTexture(texture);
	setPiecePosition(pos);
}

void Piece::setPiecePosition(int newPos) {
	pos = newPos;
	sprite.setPosition(board->getPixelPosition(pos));
}

int Board::searchDirection(int start, int dir, std::function<int(int)> endSearch) {
	start += dir;
	while (endSearch(start)) {
		if (board[start]) return board[start];
		start += dir;
	}
	return 0;
}

bool Board::isLegalMove(Piece* mover, int pos) {
	int mpos = mover->pos;
	int mid = mover->id;
	if (mid < 0 || mid > 12) exit(1);
	if (mpos == pos) return false;
	if (mid > 6 && whiteToMove) return false;
	if (mid <= 6 && !whiteToMove) return false;

	switch (mid) {
	case 1:
		if (whitePawnMove(Move(mpos, pos))) break;
		else return false;
	case 7:
		if (blackPawnMove(Move(mpos, pos))) break;
		else return false;
	case 2:
	case 8:
		if (knightMove(Move(mpos, pos))) break;
		else return false;
	case 3:
	case 9:
		if (bishopMove(Move(mpos, pos))) break;
		else return false;
	case 4:
	case 10:
		if (rookMove(Move(mpos, pos))) break;
		else return false;
	case 5:
	case 11:
		if (queenMove(Move(mpos, pos))) break;
		else return false;
	case 6:
	case 12:
		if (kingMove(Move(mpos, pos))) break;
		else return false;
	default:
		break;
	}

	if (isKingAttacked(mid, mpos, pos)) return false;
	soundToPlay = 0;

	if (board[pos]) {
		Piece* pp = getPieceAt(pos);
		if (pp) {
			if (whiteToMove) { if (pp->id <= 6) return false; }
			else if (pp->id > 6) return false;
			if (checking) return true;

			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i] == pp) pieces.erase(pieces.begin() + i);
			}
			delete pp;
		}
		soundToPlay = 1;
	}

	if (checking) return true;

	if (pawnPromotion) {
		if (whiteToMove) {
			mover->id = 5;
			mid = 5;
			mover->sprite.setTexture(textures[5]);
		}
		else {
			mover->id = 11;
			mid = 11;
			mover->sprite.setTexture(textures[11]);
		}
		pawnPromotion = false;
	}

	if (mid == 12) blackKing->moved = true;
	if (mid == 6) whiteKing->moved = true;
	if (mid == 4 || mid == 10) mover->moved = true;
	if (pos == enPassantable) {
		if (mid == 1) {
			Piece* pp = getPieceAt(pos + 8);
			board[pp->pos] = 0;
			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i] == pp) pieces.erase(pieces.begin() + i);
			}
			delete pp;
			soundToPlay = 1;
		}
		if (mid == 7) {
			Piece* pp = getPieceAt(pos - 8);
			board[pp->pos] = 0;
			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i] == pp) pieces.erase(pieces.begin() + i);
			}
			delete pp;
			soundToPlay = 1;
		}
	}

	if (validPassant) validPassant = false;
	else if (enPassantable >= 0) enPassantable = -1;
	whiteToMove = !whiteToMove;
	if (isKingAttacked(mid, mpos, pos)) soundToPlay = 2;
	board[pos] = mid;
	board[mpos] = 0;
	mover->pos = pos;


	if (!getLegalMoves().size()) {
		if (soundToPlay == 2) end = 1;
		else end = 2;
	}
	return true;
}

void Board::displayFrame(int pos) {
	frameSprite.setPosition(getPixelPosition(pos));
	frameDisplayed = true;
}

sf::Vector2f Board::getPixelPosition(int pos) {
	if (!whitePlaying) {
		pos = -1 * (pos - 63);
	}
	int x, y;
	x = pos % 8;
	y = pos / 8;
	return sf::Vector2f(x * 60, y * 60);
}

bool Board::whitePawnMove(Move move) {
	if (!move.dx) {
		if (board[move.end]) return false;
		if (move.startPos.y == 2 && move.dy == 2) {
			if (board[move.start - 8]) return false;
			enPassantable = move.start - 8;
			validPassant = true;
			return true;
		}
		if (move.dy == 1) {
			if (move.end <= 7) pawnPromotion = true;
			return true;
		}
		return false;
	}
	else if ((move.dx == 1 || move.dx == -1) && move.dy == 1) {
		if (board[move.end] || move.end == enPassantable) {
			if (move.end <= 7) pawnPromotion = true;
			return true;
		}
		return false;
	}
	return false;
}

bool Board::blackPawnMove(Move move) {
	if (!move.dx) {
		if (board[move.end]) return false;
		if (move.startPos.y == 7 && move.dy == -2) {
			if (board[move.start + 8]) return false;
			enPassantable = move.start + 8;
			validPassant = true;
			return true;
		}
		if (move.dy == -1) {
			if (move.end >= 56) pawnPromotion = true;
			return true;
		}
		return false;
	}
	else if ((move.dx == 1 || move.dx == -1) && move.dy == -1) {
		if (board[move.end] || move.end == enPassantable) {
			if (move.end >= 56) pawnPromotion = true;
			return true;
		}
		return false;
	}
	return false;
}

bool Board::rookMove(Move move) {
	int end = move.end;
	if (!move.dx) {
		if (move.dy < 0) {
			if (searchDirection(move.start, 8, [end](int x) -> int { return x < end; })) return false;
			return true;
		}
		else if (move.dy > 0) {
			if (searchDirection(move.start, -8, [end](int x) -> int { return x > end; })) return false;
			return true;
		}
	}
	else if (!move.dy) {
		if (move.dx < 0) {
			if (searchDirection(move.start, -1, [end](int x) -> int { return x > end; })) return false;
			return true;
		}
		else if (move.dx > 0) {
			if (searchDirection(move.start, 1, [end](int x) -> int { return x < end; })) return false;
			return true;
		}
	}
	else return false;
}

bool Board::queenMove(Move move) {
	if (rookMove(move) || bishopMove(move)) return true;
}

bool Board::bishopMove(Move move) {
	if (move.dx - move.dy && move.dx + move.dy) return false;
	int end = move.end;
	if (move.dx > 0) {
		if (move.dy < 0) {
			if (searchDirection(move.start, 9, [end](int x) -> int { return x < end; })) return false;
			return true;
		}
		else if (move.dy > 0) {
			if (searchDirection(move.start, -7, [end](int x) -> int { return x > end; })) return false;
			return true;
		}
	}
	else if (move.dx < 0) {
		if (move.dy < 0) {
			if (searchDirection(move.start, 7, [end](int x) -> int { return x < end; })) return false;
			return true;
		}
		else if (move.dy > 0) {
			if (searchDirection(move.start, -9, [end](int x) -> int { return x > end; })) return false;
			return true;
		}
	}
	else return false;
}

bool Board::knightMove(Move move) {
	int diff = move.end - move.start;
	if (move.dx > 2 || move.dy > 2 || move.dx < -2 || move.dy < -2) return false;
	if (diff == 6 || diff == -6 ||
		diff == 10 || diff == -10 ||
		diff == 15 || diff == -15 ||
		diff == 17 || diff == -17) return true;

	return false;
}	

bool Board::kingMove(Move move) {
	if (move.dx == 2) {
		if (whiteToMove && !whiteKing->moved) {
			if (board[62] || board[61]) return false;
			if (isKingAttacked(6, whiteKing->pos, whiteKing->pos + 1)) return false;
			Piece* rightRook = getPieceAt(63);
			if (rightRook && rightRook->id == 4 && !rightRook->moved) {
				if (checking) return true;
				board[63] = 0;
				rightRook->setPiecePosition(whiteKing->pos + 1);
				board[rightRook->pos] = 4;
				return true;
			}
		}
		else if (!blackKing->moved) {
			if (isKingAttacked(12, blackKing->pos, blackKing->pos)) return false;
			if (board[6] || board[5]) return false;
			if (isKingAttacked(12, blackKing->pos, blackKing->pos + 1)) return false;
			Piece* rightRook = getPieceAt(7);
			if (rightRook && rightRook->id == 10 && !rightRook->moved) {
				if (checking) return true;
				board[7] = 0;
				rightRook->setPiecePosition(blackKing->pos + 1);
				board[rightRook->pos] = 10;
				return true;
			}
		}
	}
	if (move.dx == -2) {
		if (isKingAttacked(6, whiteKing->pos, whiteKing->pos)) return false;
		if (whiteToMove && !whiteKing->moved) {
			if (board[57] || board[58] || board[59]) return false;
			if (isKingAttacked(6, whiteKing->pos, whiteKing->pos - 1)) return false;
			Piece* leftRook = getPieceAt(56);
			if (leftRook && leftRook->id == 4 && !leftRook->moved) {
				if (checking) return true;
				board[56] = 0;
				leftRook->setPiecePosition(whiteKing->pos - 1);
				board[leftRook->pos] = 4;
				return true;
			}
		}
		else if (!blackKing->moved) {
			if (board[1] || board[2] || board[3]) return false;
			if (isKingAttacked(12, blackKing->pos, blackKing->pos - 1)) return false;
			Piece* leftRook = getPieceAt(0);
			if (leftRook && leftRook->id == 10 && !leftRook->moved) {
				if (checking) return true;
				board[0] = 0;
				leftRook->setPiecePosition(blackKing->pos - 1);
				board[leftRook->pos] = 10;
				return true;
			}
		}
	}
	if (move.dx > 1 || move.dx < -1 || move.dy > 1 || move.dy < -1) return false;
	return true;
}

bool Board::isKingAttacked(int id, int start, int end) {
	int kingPos;
	int k;
	int initialStart = board[start];
	int initialEnd = board[end];
	board[start] = 0;
	board[end] = id;
	bool returnVal = false;

	if (whiteToMove) {
		if (id == 6) kingPos = end;
		else kingPos = whiteKing->pos;

		// checking for pawns and kings
		if ((kingPos - 7) % 8 && kingPos > 7 && (board[kingPos - 7] == 7 || board[kingPos - 7] == 12)) { returnVal = true; goto END; }
		if (kingPos % 8 && kingPos > 9 && (board[kingPos - 9] == 7 || board[kingPos - 9] == 12)) { returnVal = true; goto END; }
		if (kingPos + 7 <= 63 && kingPos % 8 && board[kingPos + 7] == 12) { returnVal = true; goto END; }
		if (kingPos + 9 <= 63 && kingPos % 8 != 7 && board[kingPos + 9] == 12) { returnVal = true; goto END; }
		if ((kingPos + 1) % 8 && board[kingPos + 1] == 12) { returnVal = true; goto END; }
		if (kingPos > 0 && (kingPos - 1) % 8 != 7 && board[kingPos - 1] == 12) { returnVal = true; goto END; }
		if (kingPos + 8 <= 63 && board[kingPos + 8] == 12) { returnVal = true; goto END; }
		if (kingPos - 8 >= 0 && board[kingPos - 8] == 12) { returnVal = true; goto END; }
		// bishops, queens, rooks
		k = searchDirection(kingPos, 8, [](int x) -> int { return x <= 63; }); // south
			if (k == 10 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -8, [](int x) -> int { return x >= 0; }); // north
			if (k == 10 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, 1, [](int x) -> int { return x % 8; }); // east
			if (k == 10 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -1, [](int x) -> int { return (x % 8 != 7); }); // west
			if (k == 10 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -7, [](int x) -> int { return (x >= 0 && (x % 8)); }); // northeast
			if (k == 9 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, 9, [](int x) -> int { return (x <= 63 && (x % 8)); }); // southeast
			if (k == 9 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -9, [](int x) -> int { return (x >= 0 && (x % 8 != 7)); }); // northwest
			if (k == 9 || k == 11) { returnVal = true; goto END; }
		k = searchDirection(kingPos, 7, [](int x) -> int { return (x <= 63 && (x % 8 != 7)); }); // southwest
			if (k == 9 || k == 11) { returnVal = true; goto END; }
		// knights
		int file = kingPos % 8 + 1;
		int rank = kingPos / 8 * -1 + 8;
		if (file > 1) {
			if (rank > 2) if (kingPos + 15 <= 63 && board[kingPos + 15] == 8) { returnVal = true; goto END; }
			if (rank < 7) if (kingPos - 17 >= 0 && board[kingPos - 17] == 8) { returnVal = true; goto END; }
			if (file > 2) {
				if (rank < 8) if (kingPos - 10 >= 0 && board[kingPos - 10] == 8) { returnVal = true; goto END; }
				if (rank > 1) if (kingPos + 6 <= 63 && board[kingPos + 6] == 8) { returnVal = true; goto END; }
			}
		}
		if (file < 8) {
			if (rank > 2) if (kingPos + 17 <= 63 && board[kingPos + 17] == 8) { returnVal = true; goto END; }
			if (rank < 7) if (kingPos - 15 >= 0 && board[kingPos - 15] == 8) { returnVal = true; goto END; }
			if (file < 7) {
				if (rank < 8) if (kingPos - 6 >= 0 && board[kingPos - 6] == 8) { returnVal = true; goto END; }
				if (rank > 1) if (kingPos + 10 <= 63 && board[kingPos + 10] == 8) { returnVal = true; goto END; }
			}
		}
		
	}
	else { // black to move
		if (id == 12) kingPos = end;
		else kingPos = blackKing->pos;

		// checking for pawns and kings
		if ((kingPos - 7) % 8 && kingPos > 7 && board[kingPos - 7] == 6) { returnVal = true; goto END; }
		if (kingPos % 8 && kingPos > 9 && board[kingPos - 9] == 6) { returnVal = true; goto END; }
		if (kingPos + 7 <= 63 && kingPos % 8 && (board[kingPos + 7] == 6 || board[kingPos + 7] == 1)) { returnVal = true; goto END; }
		if (kingPos + 9 <= 63 && kingPos % 8 != 7 && (board[kingPos + 9] == 6 || board[kingPos + 9] == 1)) { returnVal = true; goto END; }
		if ((kingPos + 1) % 8 && board[kingPos + 1] == 6) { returnVal = true; goto END; }
		if (kingPos > 0 && (kingPos - 1) % 8 != 7 && board[kingPos - 1] == 6) { returnVal = true; goto END; }
		if (kingPos + 8 <= 63 && board[kingPos + 8] == 6) { returnVal = true; goto END; }
		if (kingPos - 8 >= 0 && board[kingPos - 8] == 6) { returnVal = true; goto END; }
		// bishops, queens, rooks
		k = searchDirection(kingPos, 8, [](int x) -> int { return x <= 63; }); // south
		if (k == 4 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -8, [](int x) -> int { return x >= 0; }); // north
		if (k == 4 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, 1, [](int x) -> int { return x % 8; }); // east
		if (k == 4 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -1, [](int x) -> int { return (x % 8 != 7); }); // west
		if (k == 4 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -7, [](int x) -> int { return (x >= 0 && (x % 8)); }); // northeast
		if (k == 3 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, 9, [](int x) -> int { return (x <= 63 && (x % 8)); }); // southeast
		if (k == 3 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, -9, [](int x) -> int { return (x >= 0 && (x % 8 != 7)); }); // northwest
		if (k == 3 || k == 5) { returnVal = true; goto END; }
		k = searchDirection(kingPos, 7, [](int x) -> int { return (x <= 63 && (x % 8 != 7)); }); // southwest
		if (k == 3 || k == 5) { returnVal = true; goto END; }
		// knights
		int file = kingPos % 8 + 1;
		int rank = kingPos / 8 * -1 + 8;
		if (file > 1) {
			if (rank > 2) if (kingPos + 15 <= 63 && board[kingPos + 15] == 2) { returnVal = true; goto END; }
			if (rank < 7) if (kingPos - 17 >= 0 && board[kingPos - 17] == 2) { returnVal = true; goto END; }
			if (file > 2) {
				if (rank < 8) if (kingPos - 10 >= 0 && board[kingPos - 10] == 2) { returnVal = true; goto END; }
				if (rank > 1) if (kingPos + 6 <= 63 && board[kingPos + 6] == 2) { returnVal = true; goto END; }
			}
		}
		if (file < 8) {
			if (rank > 2) if (kingPos + 17 <= 63 && board[kingPos + 17] == 2) { returnVal = true; goto END; }
			if (rank < 7) if (kingPos - 15 >= 0 && board[kingPos - 15] == 2) { returnVal = true; goto END; }
			if (file < 7) {
				if (rank < 8) if (kingPos - 6 >= 0 && board[kingPos - 6] == 2) { returnVal = true; goto END; }
				if (rank > 1) if (kingPos + 10 <= 63 && board[kingPos + 10] == 2) { returnVal = true; goto END; }
			}
		}
	}

	END:
	board[start] = id;
	board[end] = initialEnd;
	return returnVal;
}

std::vector<Move> Board::getLegalMoves() {
	checking = true;
	bool thePawnPromotion = pawnPromotion;
	int theSoundToPlay = soundToPlay;
	int theEnPassantable = enPassantable;
	int theValidPassant = validPassant;
	std::vector<Move> moves;
	if (whiteToMove) {
		for (int i = 0; i < pieces.size(); i++) {
			if (pieces[i]->id <= 6) {
				for (int j = 0; j < 64; j++) {
					int a = enPassantable;
					int b = validPassant;
					int c = pawnPromotion;
					if (isLegalMove(pieces[i], j)) 
						moves.push_back(Move(pieces[i]->pos, j));
					enPassantable = a;
					validPassant = b;
					pawnPromotion = c;
				}
			}
		}
	}
	else {
		for (int i = 0; i < pieces.size(); i++) {
			if (pieces[i]->id > 6) {
				for (int j = 0; j < 64; j++) {
					int a = enPassantable;
					int b = validPassant;
					int c = pawnPromotion;
					if (isLegalMove(pieces[i], j)) 
						moves.push_back(Move(pieces[i]->pos, j));
					enPassantable = a;
					validPassant = b;
					pawnPromotion = c;
				}
			}
		}
	}
	soundToPlay = theSoundToPlay;
	enPassantable = theEnPassantable;
	validPassant = theValidPassant;
	pawnPromotion = thePawnPromotion;
	checking = false;
	return moves;
}