#pragma once

#include "raylib.h"

#include "core/2D/game_object.h"
#include "lib/pool_allocator.h"
#include "lib/arena.h"

namespace bacon
{
	class TextObject : public GameObject
	{
	public:
	    static Arena<TextObject> _allocator;

		TextObject();
		TextObject(uint8_t* bytes);
		TextObject(const TextObject& text_object);
		TextObject& operator=(const TextObject& text_object);
		TextObject(TextObject&& text_object) = delete;
		TextObject& operator=(TextObject&& text_object) = delete;
		~TextObject() = default;

		void copy(const GameObject& object) override;
		TextObject* clone() const override;

		void add_to_state() override;

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

	private:
		std::string m_text;
		std::string m_render_text;
		std::shared_ptr<Font> m_font;
		std::string m_font_path;
		int32_t m_font_size;
		int32_t m_char_spacing;
		Color m_color;

		void update_render_text();
		float calculate_text_width(const std::string& text);
		std::string get_wrapped_text(const std::string& text);
	};
} // namespace bacon
