#include "text_object.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"
#include "raymath.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
	Arena<TextObject> TextObject::_allocator(globals::allocator_block_size);

	void* TextObject::operator new(size_t size)
	{
		return TextObject::_allocator.allocate();
	}

	void TextObject::operator delete(void* ptr, size_t size)
	{
		TextObject::_allocator.deallocate((TextObject*)ptr);
	}

	void* TextObject::operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	TextObject::TextObject() : GameObject()
	{
		this->object_type = ObjectType::TEXT;
		this->name = "Text";
		this->m_text = "";
		this->m_font = {0};
		this->m_font_size = 12;
		this->m_char_spacing = 0;
		this->m_color = BLACK;

		this->update_buffers();
	}

	TextObject::TextObject(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	TextObject::TextObject(const TextObject& text_object)
		: GameObject(text_object)
	{
		this->object_type = ObjectType::TEXT;

		this->copy(text_object);
	}

	TextObject& TextObject::operator=(const TextObject& text_object)
	{
		this->copy(text_object);

		return *this;
	}

	void TextObject::copy(const GameObject& object)
	{
		GameObject::copy(object);

		const TextObject& text_object = static_cast<const TextObject&>(object);

		this->m_text = text_object.m_text;
		this->set_font(text_object.m_font_path);
		this->m_font_size = text_object.m_font_size;
		this->m_char_spacing = text_object.m_char_spacing;
		this->m_color = text_object.m_color;
	}

	TextObject* TextObject::clone() const
	{
		return new TextObject(*this);
	}

	void TextObject::add_to_state()
	{
		GameState::scene.add_text_object(this);
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
		this->update_render_text();
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

	bool TextObject::contains_point(Vector2 point)
	{
		Vector2 point_relative = Vector2Subtract(point, position);
		Vector2 p = Vector2Rotate(point_relative, -this->rotation * DEG2RAD);
		Rectangle rect = {
			0,
			0,
			size.x,
			size.y,
		};

		return CheckCollisionPointRec(p, rect);
	}

	void TextObject::update_buffers()
	{
		update_base_buffers();

		m_buffers.text = m_text;
		m_buffers.font_path = m_font_path;
		m_buffers.font_size = m_font_size;
		m_buffers.char_spacing = m_char_spacing;
		m_buffers.color = m_color;
	}

	void TextObject::update_from_buffers()
	{
		update_from_base_buffers();

		set_text(m_buffers.text);
		set_font(m_buffers.font_path);
		set_font_size(m_buffers.font_size);
		m_char_spacing = m_buffers.char_spacing;
		m_color = m_buffers.color;
	}

	void TextObject::draw() const
	{
		DrawTextPro(
			*this->m_font,
			this->m_render_text.c_str(),
			this->position,
			{0, 0}, // Origin
			this->rotation,
			this->m_font_size,
			this->m_char_spacing,
			this->m_color);
	}

	void TextObject::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		TextObject copy_text(*this);

		bool change_made = draw_base_properties();

		// Text
		ImGui::ItemLabel("Text", ItemLabelFlag::Left);
		ImGui::InputTextMultiline("##text", &m_text);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Font
		ImGui::ItemLabel("Font", ItemLabelFlag::Left);
		ImGui::InputText("##font", &m_buffers.font_path);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Font size
		ImGui::ItemLabel("Font Size", ItemLabelFlag::Left);
		ImGui::InputInt("##fontsize", &m_buffers.font_size);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Character spacing
		ImGui::ItemLabel("Character Spacing", ItemLabelFlag::Left);
		ImGui::InputInt("##charspacing", &m_buffers.char_spacing);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Color
		float color[] = {
			(float)m_buffers.color.r / 255.f,
			(float)m_buffers.color.g / 255.f,
			(float)m_buffers.color.b / 255.f,
			(float)m_buffers.color.a / 255.f};
		ImGui::ItemLabel("Color", ItemLabelFlag::Left);
		if (ImGui::ColorEdit4("##color", color))
		{
			color[0] = std::clamp(color[0], 0.f, 1.f);
			color[1] = std::clamp(color[1], 0.f, 1.f);
			color[2] = std::clamp(color[2], 0.f, 1.f);
			color[3] = std::clamp(color[3], 0.f, 1.f);

			m_buffers.color = (Color){
				.r = (unsigned char)(color[0] * 255.f),
				.g = (unsigned char)(color[1] * 255.f),
				.b = (unsigned char)(color[2] * 255.f),
				.a = (unsigned char)(color[3] * 255.f)};
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
			update_from_buffers();

			ObjectEvent* event = new ObjectEvent(&copy_text, this);
			event->object = this;
			push_event(event);
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
		this->update_buffers();
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
