#include "text_object.h"

#include "raylib.h"
#include "imgui.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon {
    TextObject::TextObject() : GameObject() {
        this->object_type = ObjectType::TEXT;
        this->name = "Text";
        this->m_text = "";
        this->m_font = {0};
        this->m_font_size = 0;
        this->m_char_spacing = 0;
        this->m_max_text_width = 0;
        this->m_color = BLACK;
    }

    void TextObject::set_text(const std::string& text) {
        this->update_text(text);
    }

    void TextObject::set_font(const std::string& font_path) {
        debug_error("This function has not been implemented yet.");
        return;

        this->m_font_path = font_path;
    }

    void TextObject::set_font_size(uint32_t size) {
        this->m_font_size = size;
        this->update_text(this->m_text);
    }

    // TODO
    void TextObject::update_text(const std::string& text) {
        float text_width = this->calculate_text_width(text);

        if (text_width > this->m_max_text_width) {
            std::string new_text = this->get_wrapped_text(text);
            this->m_text = new_text;
        }
        else {
            this->m_text = text;
        }
    }

    float TextObject::calculate_text_width(const std::string& text) {
        Vector2 size_vector = MeasureTextEx(
            this->m_font,
            text.data(),
            this->m_font_size,
            this->m_char_spacing
        );

        return size_vector.x;
    }

    std::string TextObject::get_wrapped_text(const std::string& text) {
        debug_error("This function has not been implemented yet.");
        return "";
    }

    void TextObject::draw() const {
        DrawTextPro(
            this->m_font,
            this->m_text.c_str(),
            this->position,
            {0,0},
            this->rotation,
            this->m_font_size,
            this->m_char_spacing,
            this->m_color
        );
    }

    void TextObject::draw_properties_editor() {
        GameObject::draw_properties_editor();
    }

    void TextObject::save_to_json(nlohmann::json& data) const {
        GameObject::save_to_json(data);

        data["text"] = m_text;
        data["font_path"] = m_font_path;
        data["font_size"] = m_font_size;
        data["char_spacing"] = m_char_spacing;
        data["max_text_width"] = m_max_text_width;
        data["color"] = {
            m_color.r,
            m_color.g,
            m_color.b,
            m_color.a
        };
    }

    void TextObject::load_from_json(nlohmann::json& data) {
        GameObject::load_from_json(data);

        for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
        {
            std::string key = it.key();
            auto value = *it;

            if (key == "text")
            {
                // set_text(value);
                this->m_text = value;
            }
            else if (key == "font_path")
            {
                set_font(value);
            }
            else if (key == "font_size")
            {
                set_font_size(value);
            }
            else if (key == "char_spacing")
            {
                this->m_char_spacing = value;
            }
            else if (key == "max_text_width")
            {
                this->m_max_text_width = value;
            }
            else if (key == "color")
            {
                this->m_color = (Color){
                    .r = value[0],
                    .g = value[1],
                    .b = value[2],
                    .a = value[3]
                };
            }
        }

        this->set_text(m_text);
    }

    size_t TextObject::calculate_size() const {
        debug_error("This function has not been implemented yet.");
        return 0;
    }

    uint8_t* TextObject::serialize() const {
        debug_error("This function has not been implemented yet.");
        return nullptr;
    }

    void TextObject::deserialize(uint8_t* bytes) {
        debug_error("This function has not been implemented yet.");
    }
}
