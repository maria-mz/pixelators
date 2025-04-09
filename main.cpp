#include "Game.h"

int main(int argc, char* args[])
{
    if (argc != 2)
    {
        printf("Usage: main <0|1>\n");
        return 1;
    }

    bool isHost = false;

    if (std::stoi(args[1]) == 1)
    {
        isHost = true;
    }

    Game game = Game();

    if (game.init(isHost))
    {
        game.run();
    }
}
