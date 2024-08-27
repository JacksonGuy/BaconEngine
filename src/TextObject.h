#pragma once

#include "GameObject.h"

class TextObject : public GameObject {
    public:
        std::string text;
        Font font;
        i8 fontSize;
        i8 charSpacing;
        Color color;
};
