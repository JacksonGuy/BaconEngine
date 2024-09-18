#include "Input.h"

namespace Input {
    std::unordered_map<std::string, KeyboardKey> key_map;
    std::unordered_map<std::string, MouseButton> mouse_map;

    /**
     * @brief Creates maps of Strings to Raylib inputs
     * 
     */
    void InitInputMaps() {
        for (int i = 0; i < 26; i++) {
            // Yes I know this looks dumb but the obvious way wouldn't work so
            char ch = 'A' + i;
            std::string key;
            key.push_back(ch);
            key_map[key] = KeyboardKey(65 + i);
        }

        key_map["ESC"] = KEY_ESCAPE;
        key_map["SPACE"] = KEY_SPACE;
        key_map["TAB"] = KEY_TAB;

        key_map["LCONTROL"] = KEY_LEFT_CONTROL;
        key_map["LSHIFT"] = KEY_LEFT_SHIFT;
        key_map["LALT"] = KEY_LEFT_ALT;
        
        key_map["RCONTROL"] = KEY_RIGHT_CONTROL;
        key_map["RSHIFT"] = KEY_RIGHT_SHIFT;
        key_map["RALT"] = KEY_RIGHT_ALT;

        key_map["UP"] = KEY_UP;
        key_map["DOWN"] = KEY_DOWN;
        key_map["LEFT"] = KEY_LEFT;
        key_map["RIGHT"] = KEY_RIGHT;

        key_map["LBRACKET"] = KEY_LEFT_BRACKET;
        key_map["RBRACKET"] = KEY_RIGHT_BRACKET;

        mouse_map["MOUSE_LEFT"] = MOUSE_LEFT_BUTTON;
        mouse_map["MOUSE_RIGHT"] = MOUSE_RIGHT_BUTTON;
        mouse_map["MOUSE_MIDDLE"] = MOUSE_MIDDLE_BUTTON;
        mouse_map["MOUSE_BUTTON_4"] = MOUSE_BUTTON_FORWARD;
        mouse_map["MOUSE_BUTTON_5"] = MOUSE_BUTTON_BACK;
    }
}