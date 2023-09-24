#ifndef CHESSWINDOW_H
#define CHESSWINDOW_H 1

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "board.h"

class ChessWindow {
public:
	ChessWindow();
	bool update();

private:
	sf::RenderWindow window;
	sf::SoundBuffer buffs[4];
	sf::Sound sounds[4];
	Board board;
	bool dragging = false;
	Piece* dragged;
};

#endif