#pragma once

#include <string_view>

enum ItemLabelFlag
{
    Left = 1u << 0u,
    Right = 1u << 1u,
    Default = Left,
};

namespace ImGui
{
    void ItemLabel(std::string_view title, ItemLabelFlag flags);
}
