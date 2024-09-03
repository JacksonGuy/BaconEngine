#pragma once

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
        void DrawPropertiesUI();
};
