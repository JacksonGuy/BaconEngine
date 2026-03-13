#include "text_object.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include "core/2D/object_2d.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"
#include "raymath.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "file/file.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
	PoolAllocator<TextObject> TextObject::_allocator(globals::allocator_block_size);

	void* TextObject::operator new(size_t size)
	{
		return TextObject::_allocator.allocate();
	}

	void* TextObject::operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	void TextObject::operator delete(void* ptr)
	{
		TextObject::_allocator.deallocate((TextObject*)ptr);
	}

	void TextObject::operator delete(void* ptr, size_t size)
	{
		TextObject::_allocator.deallocate((TextObject*)ptr);
	}

	TextObject::TextObject() : Object2D()
	{
		m_type_id = TypeID::TEXT_2D;
		set_name("Text");

		m_text = "";
		m_font = {0};
		m_font_size = 12;
		m_char_spacing = 0;
		m_color = BLACK;
	}

	TextObject::TextObject(ByteStream& bytes) : Object2D()
	{
		m_type_id = TypeID::TEXT_2D;
		deserialize(bytes);
	}

	TextObject::TextObject(const TextObject& text_object) : Object2D()
	{
		m_type_id = TypeID::TEXT_2D;
		copy(text_object);
	}

	TextObject& TextObject::operator=(const TextObject& text_object)
	{
		m_type_id = TypeID::TEXT_2D;
		copy(text_object);

		return *this;
	}

	TextObject::~TextObject()
	{
		destroy();
	}

	void TextObject::copy(const GameObject& object)
	{
		Object2D::copy(object);

		const TextObject& text_object = static_cast<const TextObject&>(object);

		this->set_font(text_object.m_font_path);
		this->m_font_size = text_object.m_font_size;
		this->m_char_spacing = text_object.m_char_spacing;
		this->m_color = text_object.m_color;
		set_text(text_object.m_text); // Call last
	}

	TextObject* TextObject::clone() const
	{
		TextObject* new_text = new TextObject(*this);
		return new_text;
	}

	TextObject* TextObject::clone_unique() const
	{
		TextObject* new_text = new TextObject(*this);
		new_text->set_uuid(UUID());
		return new_text;
	}

	void TextObject::add_to_scene()
	{
		if (get_in_scene()) return;

		GameState::state_2d->scene->add_text_object(this);
		GameState::state_2d->renderer->add_to_layer(this, get_layer());
		set_in_scene(true);

		add_children_to_scene();
	}

	void TextObject::remove_from_scene()
	{
		if (!get_in_scene()) return;

		GameState::state_2d->scene->remove_text_object(this);
		GameState::state_2d->renderer->remove_from_layer(this);
		set_in_scene(false);

		remove_children_from_scene();
	}

	void TextObject::set_text(const std::string& text)
	{
		m_text = text;
		update_render_text();
	}

	void TextObject::set_font(const std::string& font_path)
	{
		if (font_path.length() <= 0)
		{
			m_font = nullptr;
			m_font_path = "";
			update_render_text();

			return;
		}

		m_font = GameState::state_2d->assets->load_font(font_path.c_str());
		if (m_font == nullptr)
		{
			m_font_path = "";
		}
		else
		{
			m_font_path = font_path;
		}

		update_render_text();
	}

	void TextObject::set_font_size(int32_t size)
	{
		m_font_size = size;
		update_render_text();
	}

	void TextObject::update_render_text()
	{
		float text_width = calculate_text_width(m_text);

		if (text_width > get_size().x)
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
		Vector2 size_vector = {0, 0};
		if (m_font != nullptr)
		{
			size_vector = MeasureTextEx(
				*m_font,
				text.data(),
				m_font_size,
				m_char_spacing);
		}
		else
		{
			size_vector = MeasureTextEx(
				GetFontDefault(),
				text.data(),
				m_font_size,
				m_char_spacing);
		}

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

				if (current_width <= get_size().x)
				{
					line = current_line;
				}
				else
				{
					if (!wrapped_paragraph.empty())
					{
						wrapped_paragraph += '\n';
					}

					if (calculate_text_width(word) > get_size().x)
					{
						std::string broken;
						for (char c : word)
						{
							std::string test = broken + c;
							if (calculate_text_width(test) > get_size().x)
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

	void TextObject::draw_outline() const
	{
		Vector2 position = get_position();
		Vector2 size = get_size();
		float rotation = get_rotation();

		DrawRectangleLinesPro(
			{
				position.x + (size.x / 2.f),
				position.y + (size.y / 2.f),
				size.x,
				size.y,
			},
			rotation,
			3.f,
			Color{0, 255, 0, 255}
		);
	}

	bool TextObject::contains_point(Vector2 point)
	{
		Vector2 point_relative = Vector2Subtract(point, get_position());
		Vector2 p = Vector2Rotate(point_relative, -get_rotation() * DEG2RAD);
		Rectangle rect = {
			0,
			0,
			get_size().x,
			get_size().y,
		};

		return CheckCollisionPointRec(p, rect);
	}

	void TextObject::update_ui_buffer() const
	{
		Object2D::update_ui_buffer();

		ui::obj_properties.text = m_text;
		ui::obj_properties.font_path = m_font_path;
		ui::obj_properties.font_size = m_font_size;
		ui::obj_properties.char_spacing = m_char_spacing;
		ui::obj_properties.color = m_color;
	}

	void TextObject::update_from_ui_buffer()
	{
		Object2D::update_from_ui_buffer();

		set_text(ui::obj_properties.text);
		set_font(ui::obj_properties.font_path);
		set_font_size(ui::obj_properties.font_size);
		m_char_spacing = ui::obj_properties.char_spacing;
		m_color = ui::obj_properties.color;
	}

	void TextObject::draw() const
	{
		if (m_font == nullptr)
		{
			DrawTextPro(
				GetFontDefault(),
				m_render_text.c_str(),
				get_position(),
				{0, 0},
				get_rotation(),
				m_font_size,
				m_char_spacing,
				m_color);
		}
		else
		{
			DrawTextPro(
				*m_font,
				m_render_text.c_str(),
				get_position(),
				{0, 0},
				get_rotation(),
				m_font_size,
				m_char_spacing,
				m_color);
		}
	}

	void TextObject::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		if (ui::inspect_object_copy == nullptr ||
			ui::inspect_object_copy->get_uuid() != get_uuid())
		{
			delete ui::inspect_object_copy;
			ui::inspect_object_copy = this->clone();
		}

		Object2D::draw_properties_editor();

		// Text
		ImGui::ItemLabel("Text", ItemLabelFlag::Left);
		ImGui::InputTextMultiline("##text", &ui::obj_properties.text);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Font
		ImGui::ItemLabel("Font", ItemLabelFlag::Left);
		ImGui::InputText("##font", &ui::obj_properties.font_path);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}
		if (ImGui::Button("Select"))
		{
			file::asset_t result = file::load_asset_nfd(file::AssetType::FONT);
			if (result.type != file::AssetType::NONE)
			{
				ui::obj_properties.font_path = result.path;

				globals::has_unsaved_changes = true;
				ui::properties_changes_made = true;
			}
		}

		// Font size
		ImGui::ItemLabel("Font Size", ItemLabelFlag::Left);
		ImGui::InputInt("##font_size", &ui::obj_properties.font_size);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Character spacing
		ImGui::ItemLabel("Character Spacing", ItemLabelFlag::Left);
		ImGui::InputInt("##char_spacing", &ui::obj_properties.char_spacing);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Color
		float color[] = {
			(float)ui::obj_properties.color.r / 255.f,
			(float)ui::obj_properties.color.g / 255.f,
			(float)ui::obj_properties.color.b / 255.f,
			(float)ui::obj_properties.color.a / 255.f};
		ImGui::ItemLabel("Color", ItemLabelFlag::Left);
		if (ImGui::ColorEdit4("##color", color))
		{
			color[0] = std::clamp(color[0], 0.f, 1.f);
			color[1] = std::clamp(color[1], 0.f, 1.f);
			color[2] = std::clamp(color[2], 0.f, 1.f);
			color[3] = std::clamp(color[3], 0.f, 1.f);

			ui::obj_properties.color = (Color){
				.r = (unsigned char)(color[0] * 255.f),
				.g = (unsigned char)(color[1] * 255.f),
				.b = (unsigned char)(color[2] * 255.f),
				.a = (unsigned char)(color[3] * 255.f)};
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		if (ui::properties_changes_made)
		{
			update_from_ui_buffer();

			ObjectEvent* event = new ObjectEvent(ui::inspect_object_copy, this);
			push_event(event);

			ui::inspect_object_copy->copy(*this);
			ui::properties_changes_made = false;
		}
	}

	void TextObject::save_to_json(nlohmann::json& data) const
	{
		Object2D::save_to_json(data);

		data["text"] = m_text;
		data["font_path"] = m_font_path;
		data["font_size"] = m_font_size;
		data["char_spacing"] = m_char_spacing;
		data["color"] = {m_color.r, m_color.g, m_color.b, m_color.a};
	}

	void TextObject::load_from_json(const nlohmann::json& data)
	{
		Object2D::load_from_json(data);

		m_text = json_read_string(data, "text");
		m_font_path = json_read_string(data, "font_path");
		m_font_size = json_read_int32(data, "font_size");
		m_char_spacing = json_read_int32(data, "char_spacing");
		m_color = json_read_color(data, "color");

		this->set_font(m_font_path);
		this->set_text(m_text);
	}

	ByteStream TextObject::serialize() const
	{
		ByteStream bytes = Object2D::serialize();

		bytes << m_text;
		bytes << m_font_path;
		bytes << m_font_size;
		bytes << m_char_spacing;
		bytes << m_color.r << m_color.g << m_color.b << m_color.a;

		return bytes;
	}

	void TextObject::deserialize(ByteStream& bytes)
	{
		Object2D::deserialize(bytes);

		bytes >> m_text;
		bytes >> m_font_path;
		bytes >> m_font_size;
		bytes >> m_char_spacing;
		bytes >> m_color.r >> m_color.g >> m_color.b >> m_color.a;
	}
} // namespace bacon
