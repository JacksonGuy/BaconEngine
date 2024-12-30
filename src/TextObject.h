#pragma once

#include "json.hpp"

#include "GameObject.h"

class TextObject : public GameObject {
    public:
        std::string text;
        Font font;
        std::string fontPath;
        i32 fontSize;
        i32 charSpacing;
        Color color;

        TextObject();
        ~TextObject();
        void SetFont(std::string path);
        void CalculateSize();
        void SaveTextObjectJson(nlohmann::json& data);
        void LoadTextObjectJson(nlohmann::json& data);
        void DrawPropertiesUI();
};
