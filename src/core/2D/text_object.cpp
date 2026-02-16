#include "text_object.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include "core/game_state.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"

#include "core/globals.h"
#include "core/util.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
	Arena<TextObject> TextObject::_allocator(globals::allocator_block_size);

	TextObject::TextObject() : GameObject()
	{
		this->object_type = ObjectType::TEXT;
		this->name = "Text";
		this->m_text = "";
		this->m_font = {0};
		this->m_font_size = 12;
		this->m_char_spacing = 0;
		this->m_color = BLACK;
	}

	TextObject::TextObject(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	TextObject::TextObject(const TextObject& text_object)
		: GameObject(text_object)
	{
		this->object_type = ObjectType::TEXT;
		this->m_text = text_object.m_text;
		this->set_font(text_object.m_font_path);
		this->m_font_size = text_object.m_font_size;
		this->m_char_spacing = text_object.m_char_spacing;
		this->m_color = text_object.m_color;
	}

	TextObject& TextObject::operator=(const TextObject& text_object)
	{
	    GameObject::operator=(text_object);

		this->object_type = ObjectType::TEXT;
		this->m_text = text_object.m_text;
		this->set_font(text_object.m_font_path);
		this->m_font_size = text_object.m_font_size;
		this->m_char_spacing = text_object.m_char_spacing;
		this->m_color = text_object.m_color;

	    return *this;
	}

	void TextObject::set_text(const std::string& text)
	{
	    m_text = text;
		this->update_render_text();
	}

	void TextObject::set_font(const std::string& font_path)
	{
		this->m_font = GameState::assets.load_font(font_path.c_str());
		this->m_font_path = font_path;
	}

	void TextObject::set_font_size(int32_t size)
	{
		this->m_font_size = size;
		this->update_render_text();
	}

	void TextObject::update_render_text()
	{
		float text_width = this->calculate_text_width(m_text);

		if (text_width > this->size.x)
		{
			m_render_text = this->get_wrapped_text(m_text);
		}
		else
		{
			m_render_text = m_text;
		}
	}

	float TextObject::calculate_text_width(const std::string& text)
	{
		Vector2 size_vector =
			MeasureTextEx(*this->m_font, text.data(), this->m_font_size,
						  this->m_char_spacing);

		return size_vector.x;
	}

	std::string TextObject::get_wrapped_text(const std::string& text)
	{
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

				if (current_width <= this->size.x)
				{
					line = current_line;
				}
				else
				{
					if (!wrapped_paragraph.empty())
					{
						wrapped_paragraph += '\n';
					}

					if (calculate_text_width(word) > this->size.x)
					{
						std::string broken;
						for (char c : word)
						{
							std::string test = broken + c;
							if (calculate_text_width(test) > this->size.x)
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

	void TextObject::draw() const
	{
		DrawTextPro(
		    *this->m_font,
			this->m_render_text.c_str(),
			this->position,
			{0, 0},  // Origin
			this->rotation,
			this->m_font_size,
			this->m_char_spacing,
			this->m_color
		);
	}

	void TextObject::draw_properties_editor()
	{
	    TextObject copy_text(*this);
		copy_text.uuid = this->uuid;

		bool change_made = false;

		// Name
		std::string name_buf = this->name;
		ImGui::ItemLabel("Name", ItemLabelFlag::Left);
		if (ImGui::InputText("##name", &name_buf,
							 ImGuiInputTextFlags_EnterReturnsTrue))
		{
			this->name = std::string(name_buf);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Tag
		std::string tag_buf = this->tag;
		ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
		ImGui::InputText("##tag", &tag_buf);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			this->tag = std::string(tag_buf);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rendering layer
		int render_layer = this->layer;
		ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
		// TODO this might be a better choice?
		// ImGui::InputScalar("##layer", ImGuiDataType_U64, &render_layer);
		if (ImGui::InputInt("##layer", &render_layer))
		{
			if (render_layer < 0)
			{
				render_layer = 0;
			}

			this->set_layer(render_layer);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Position
		float position[] = {this->position.x, this->position.y};
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		if (ImGui::InputFloat2("##position", position))
		{
			this->position = (Vector2){position[0], position[1]};

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Size
		float size[] = {this->size.x, this->size.y};
		ImGui::ItemLabel("Size", ItemLabelFlag::Left);
		if (ImGui::InputFloat2("##size", size))
		{
			this->size = (Vector2){size[0], size[1]};
			this->update_render_text();

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rotation
		ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
		if (ImGui::InputFloat("##rotation", &this->rotation))
		{
			this->rotation = b_fmod(this->rotation, 360);
			this->update_render_text();

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::Separator();

		// Text
		ImGui::ItemLabel("Text", ItemLabelFlag::Left);
		if (ImGui::InputTextMultiline("##text", &m_text))
		{
			this->set_text(m_text);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Font
		ImGui::ItemLabel("Font", ItemLabelFlag::Left);
		if (ImGui::BeginCombo("##font", m_font_path.c_str()))
		{
		    auto fonts = GameState::assets.get_fonts();
            for (auto it = fonts.begin(); it != fonts.end(); it++)
            {
                std::string key = it->first;
                std::shared_ptr<Font> font = it->second;
                bool is_selected = (m_font_path == key);

                if (ImGui::Selectable(key.c_str(), is_selected))
                {
                    this->set_font(key);
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
		}

		// Font size
		int32_t font_size = this->m_font_size;
		ImGui::ItemLabel("Font Size", ItemLabelFlag::Left);
		if (ImGui::InputInt("##fontsize", &font_size))
		{
			this->set_font_size(font_size);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Character spacing
		ImGui::ItemLabel("Character Spacing", ItemLabelFlag::Left);
		if (ImGui::InputInt("##charspacing", &m_char_spacing))
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Color
		float color[] = {(float)m_color.r / 255.f, (float)m_color.g / 255.f,
						 (float)m_color.b / 255.f, (float)m_color.a / 255.f};
		ImGui::ItemLabel("Color", ItemLabelFlag::Left);
		if (ImGui::ColorEdit4("##color", color))
		{
            color[0] = std::clamp(color[0], 0.f, 1.f);
            color[1] = std::clamp(color[1], 0.f, 1.f);
            color[2] = std::clamp(color[2], 0.f, 1.f);
            color[3] = std::clamp(color[3], 0.f, 1.f);

			m_color = (Color){.r = (unsigned char)(color[0] * 255.f),
							  .g = (unsigned char)(color[1] * 255.f),
							  .b = (unsigned char)(color[2] * 255.f),
							  .a = (unsigned char)(color[3] * 255.f)};

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
		    event::EditorEvent event = event::make_object_event(&copy_text, this);
			event::push_event(event);
		}
	}

	void TextObject::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["text"] = m_text;
		data["font_path"] = m_font_path;
		data["font_size"] = m_font_size;
		data["char_spacing"] = m_char_spacing;
		data["color"] = {m_color.r, m_color.g, m_color.b, m_color.a};
	}

	void TextObject::load_from_json(nlohmann::json& data)
	{
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
			else if (key == "color")
			{
				this->m_color = (Color){
					.r = value[0], .g = value[1], .b = value[2], .a = value[3]};
			}
		}

		this->set_text(m_text);
	}

	size_t TextObject::calculate_size() const
	{
		debug_error("This function has not been implemented yet.");
		return 0;
	}

	uint8_t* TextObject::serialize() const
	{
		debug_error("This function has not been implemented yet.");
		return nullptr;
	}

	void TextObject::deserialize(uint8_t* bytes)
	{
		debug_error("This function has not been implemented yet.");
	}
} // namespace bacon
