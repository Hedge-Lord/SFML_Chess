#include "ChessWindow.h"
#include "board.h"
#include <iostream>
#include <cmath>

ChessWindow::ChessWindow(bool whitePlayer) : board(whitePlayer) {
	window.create(sf::VideoMode(480, 480), "CHESS", sf::Style::Close | sf::Style::Titlebar);
	buffs[0].loadFromFile("Assets/move-self.wav");
	buffs[1].loadFromFile("Assets/capture.wav");
	buffs[2].loadFromFile("Assets/move-check.wav");
	buffs[3].loadFromFile("Assets/illegal.wav");
	sounds[0].setBuffer(buffs[0]);
	sounds[1].setBuffer(buffs[1]);
	sounds[2].setBuffer(buffs[2]);
	sounds[3].setBuffer(buffs[3]);
}

bool ChessWindow::update() {
	if (!window.isOpen()) return false;
	sf::Event event;
	window.pollEvent(event);
	if (event.type == sf::Event::Closed) {
		window.close();
		return false;
	}

	if ((board.whitePlaying && !board.whiteToMove) || (!board.whitePlaying && board.whiteToMove)) {
		std::vector<Move> moves = board.getLegalMoves();
		if (moves.size()) {
			Move move = board.findMove(board.depth);
			Piece* piece = board.getPieceAt(move.start);

			if (board.isLegalMove(piece, move.end)) {
				sounds[board.soundToPlay].play();
				piece->setPiecePosition(move.end);
				dragged = piece;
			}
		}
	}

	sf::Vector2i mpos = sf::Mouse::getPosition(window);
	int boardPos = mpos.x / 60 + (mpos.y / 60 * 8);
	if (!board.whitePlaying) boardPos = -1 * (boardPos - 63);

	if (dragging) {
		dragged->sprite.setPosition(sf::Vector2f(mpos.x - 30, mpos.y - 30));
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				for (int i = 7; i >= 0; i--) {
					for (int j = 7; j >= 0; j--)
						std::cout << board.board[i * 8 + j];
					std::cout << std::endl;
				}
				dragging = false;
				dragged->setPiecePosition(dragged->pos);
				board.undisplayFrame();
				if (mpos.x > 0 && mpos.x < 480 && mpos.y > 0 && mpos.y < 480)
				{
					if (board.isLegalMove(dragged, boardPos)) {
						sounds[board.soundToPlay].play();
						dragged->setPiecePosition(boardPos);
					}
					else {
						sounds[3].play();
						dragged->setPiecePosition(dragged->pos);
					}
				}
			}
			if (mpos.x > 0 && mpos.x < 480 && mpos.y > 0 && mpos.y < 480)
			board.displayFrame(boardPos);
	}
	else {
		Piece* pp = board.getPieceAt(boardPos);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && pp) {
			dragging = true;
			dragged = pp;
		}
	}


	window.clear();
	board.display(&window, dragged);
	window.display();
	if (board.end) {
		if (board.end == 1) {
			if (!board.whiteToMove)
				std::cout << "White Wins!" << std::endl;
			else
				std::cout << "Black Wins!" << std::endl;
		}
		if (board.end == 2) {
			std::cout << "The game is a draw." << std::endl;
		}
	}
	return true;
}



