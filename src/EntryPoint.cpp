#include "game/Game.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
try
{
    Game game;
    game.run();
    return EXIT_SUCCESS;
}
catch (const std::exception& exception)
{
    std::cout << "Fatal error:\n" << exception.what() << std::endl;
    return EXIT_FAILURE;
}