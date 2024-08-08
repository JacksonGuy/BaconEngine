#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace Input {
    extern std::unordered_map<std::string, sf::Keyboard::Key> key_map;
    extern std::unordered_map<std::string, sf::Mouse::Button> mouse_map;

    void InitInputMaps();
}

#endif