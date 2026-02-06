#pragma once

#include "raylib.h"

#include "core/2D/game_object.h"

namespace bacon
{
class TextObject : public GameObject
{
     public:
    friend class GameManager;

    void set_text(const std::string& text);
    void set_font(const std::string& font_path);
    void set_font_size(int32_t size);

    void draw() const override;
    void draw_properties_editor() override;
    void save_to_json(nlohmann::json& data) const override;
    void load_from_json(nlohmann::json& data) override;
    size_t calculate_size() const override;
    uint8_t* serialize() const override;
    void deserialize(uint8_t* bytes) override;

     protected:
    TextObject();
    TextObject(const TextObject& text_object) = delete;
    TextObject& operator=(const TextObject& text_object) = delete;
    TextObject(TextObject&& text_object) = delete;
    TextObject& operator=(TextObject&& text_object) = delete;
    ~TextObject() = default;

     private:
    std::string m_text;
    std::shared_ptr<Font> m_font;
    std::string m_font_path;
    int32_t m_font_size;
    int32_t m_char_spacing;
    int32_t m_max_text_width;
    Color m_color;

    void update_text(const std::string& text);
    float calculate_text_width(const std::string& text);
    std::string get_wrapped_text(const std::string& text);
};
} // namespace bacon
