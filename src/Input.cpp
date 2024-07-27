#include "Input.hpp"

namespace Input {
    std::unordered_map<std::string, sf::Keyboard::Key> key_map;
    std::unordered_map<std::string, sf::Mouse::Button> mouse_map;

    /**
     * @brief Creates a map of strings to SFML inputs
     */
    void InitInputMaps() {
        for (int i = 0; i < 26; i++) {
            // I'm sorry Dennis Ritchie...
            char ch = 'A' + i;
            std::string index;
            index.push_back(ch);
            key_map[index] = sf::Keyboard::Key(i);
        }
        key_map["ESC"] = sf::Keyboard::Escape;
        key_map["LCONTROL"] = sf::Keyboard::LControl;
        key_map["LSHIFT"] = sf::Keyboard::LShift;
        key_map["LALT"] = sf::Keyboard::LAlt;
        key_map["RCONTROL"] = sf::Keyboard::RControl;
        key_map["RSHIFT"] = sf::Keyboard::RShift;
        key_map["RALT"] = sf::Keyboard::RAlt;
        key_map["SPACE"] = sf::Keyboard::Space;
        key_map["TAB"] = sf::Keyboard::Tab;
        key_map["UP"] = sf::Keyboard::Up;
        key_map["DOWN"] = sf::Keyboard::Down;
        key_map["LEFT"] = sf::Keyboard::Left;
        key_map["RIGHT"] = sf::Keyboard::Right;
        key_map["LBRACKET"] = sf::Keyboard::LBracket;
        key_map["RBRACKET"] = sf::Keyboard::RBracket;

        mouse_map["MOUSE_LEFT"] = sf::Mouse::Button::Left;
        mouse_map["MOUSE_RIGHT"] = sf::Mouse::Button::Right;
        mouse_map["MOUSE_MIDDLE"] = sf::Mouse::Button::Middle;
        mouse_map["MOUSE_BUTTON_4"] = sf::Mouse::Button::XButton1;
        mouse_map["MOUSE_BUTTON_5"] = sf::Mouse::Button::XButton2;
    }
}