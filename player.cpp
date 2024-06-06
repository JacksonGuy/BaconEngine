#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "src/GameManager.hpp"
#include "src/File.hpp"
#include "src/PlayerInstance.hpp"

int main() {
    PlayerInstance player;
    bool result = player.LoadGame("Game.json");
    if (!result) return -1;

    while (player.window->isOpen()) {
        player.Update();
    }
}
