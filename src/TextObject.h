#pragma once

#include "json.hpp"

#include "GameObject.h"

typedef struct {
    Font font;
    std::string path;
} b_Font;

class TextObject : public GameObject {
    public:
        std::string text;
        b_Font font;
        i32 fontSize;
        i32 charSpacing;
        Color color;

        TextObject();
        ~TextObject();
        void SetFont(std::string path);
        void SaveTextObjectJson(nlohmann::json& data);
        void LoadTextObjectJson(nlohmann::json& data);
        void DrawPropertiesUI();
};
