#include <SFML/Graphics.hpp>
#include "ChessWindow.h"

bool PLAYINGASWHITE = true;

int main()
{
    ChessWindow chesswindow(PLAYINGASWHITE);
    while (chesswindow.update());
}