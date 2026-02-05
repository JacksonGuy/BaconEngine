#include "text_object.h"

#include <cstring>
#include <algorithm>
#include <sstream>

#include "raylib.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon {
    TextObject::TextObject() : GameObject() {
        this->object_type = ObjectType::TEXT;
        this->name = "Text";
        this->m_text = "";
        this->m_font = {0};
        this->m_font_size = 12;
        this->m_char_spacing = 0;
        this->m_max_text_width = 64;
        this->m_color = BLACK;
    }

    void TextObject::set_text(const std::string& text) {
        this->update_text(text);
    }

    void TextObject::set_font(const std::string& font_path) {
        this->m_font = this->manager->resources.load_font(font_path.c_str());
        this->m_font_path = font_path;
    }

    void TextObject::set_font_size(int32_t size) {
        this->m_font_size = size;
        this->update_text(this->m_text);
    }

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
            *this->m_font,
            text.data(),
            this->m_font_size,
            this->m_char_spacing
        );

        return size_vector.x;
    }

    std::string TextObject::get_wrapped_text(const std::string& text) {
        std::stringstream input(text);
        std::string paragraph = "";
        std::string final = "";
        bool first_paragraph = true;

        while (std::getline(input, paragraph))
        {
            std::istringstream iss(paragraph);
            std::string word;
            std::string line;
            std::string wrapped_paragraph;

            while (iss >> word)
            {
                std::string current_line;
                if (line.empty())
                {
                    current_line = word;
                }
                else
                {
                    current_line = line + " " + word;
                }

                float current_width = calculate_text_width(current_line);

                if (current_width <= m_max_text_width)
                {
                    line = current_line;
                }
                else
                {
                    if (!wrapped_paragraph.empty())
                    {
                        wrapped_paragraph += '\n';
                    }

                    if (calculate_text_width(word) > m_max_text_width)
                    {
                        std::string broken;
                        for (char c : word)
                        {
                            std::string test = broken + c;
                            if (calculate_text_width(test) > m_max_text_width)
                            {
                                wrapped_paragraph += broken + '\n';
                                broken.clear();
                            }
                            broken += c;
                        }
                        line = broken;
                    }
                    else
                    {
                        wrapped_paragraph += line;
                        line = word;
                    }
                }
            }

            if (!line.empty())
            {
                if (!wrapped_paragraph.empty())
                {
                    wrapped_paragraph += '\n';
                }
                wrapped_paragraph += line;
            }

            if (!first_paragraph)
            {
                final += '\n';
            }

            final += wrapped_paragraph;
            first_paragraph = false;
        }

        return final;
    }

    void TextObject::draw() const {
        DrawTextPro(
            *this->m_font,
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

        // Position
        float position[] = {this->position.x, this->position.y};
        ImGui::ItemLabel("Position", ItemLabelFlag::Left);
        if (ImGui::InputFloat2("##position", position)) {
            this->position = (Vector2){position[0], position[1]};
        }

        // Size
        // We aren't using this since we have max_text_width
        // float size[] = {this->size.x, this->size.y};
        // ImGui::ItemLabel("Size", ItemLabelFlag::Left);
        // if (ImGui::InputFloat2("##size", size)) {
        //     this->size = (Vector2){size[0], size[1]};
        // }

        // Rotation
        ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
        if (ImGui::InputFloat("##rotation", &this->rotation)) {
            this->rotation = b_fmod(this->rotation, 360);
        }

        ImGui::Separator();

        // Text
        ImGui::ItemLabel("Text", ItemLabelFlag::Left);
        if (ImGui::InputTextMultiline("##text", &m_text)) {
            this->set_text(m_text);
        }

        // Font
        // ImGui::ItemLabel("Font", ItemLabelFlag::Left);
        // if (ImGui::InputText("##font", &m_font_path)) {
        //     set_font(m_font_path);
        // }

        // Font size
        int32_t font_size = this->m_font_size;
        ImGui::ItemLabel("Font Size", ItemLabelFlag::Left);
        if (ImGui::InputInt("##fontsize", &font_size)) {
            this->set_font_size(font_size);
        }

        // Character spacing
        ImGui::ItemLabel("Character Spacing", ItemLabelFlag::Left);
        ImGui::InputInt("##charspacing", &m_char_spacing);

        // Text Width
        ImGui::ItemLabel("Max text width", ItemLabelFlag::Left);
        if (ImGui::InputInt("##textwidth", &m_max_text_width)) {
            m_text.erase(std::remove(m_text.begin(), m_text.end(), '\n'), m_text.end());
            this->update_text(m_text);
        }

        // Color
        float color[] = {
            (float)m_color.r / 255.f,
            (float)m_color.g / 255.f,
            (float)m_color.b / 255.f,
            (float)m_color.a / 255.f
        };
        ImGui::ItemLabel("Color", ItemLabelFlag::Left);
        if (ImGui::ColorEdit4("##color", color)) {
            m_color = (Color){
                .r = (unsigned char)(color[0] * 255.f),
                .g = (unsigned char)(color[1] * 255.f),
                .b = (unsigned char)(color[2] * 255.f),
                .a = (unsigned char)(color[3] * 255.f)
            };
        }
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
