#include "rendering/2D/renderer_2d.h"

#include <vector>

#include "raylib.h"

#include "editor/ui/editor_ui.h"
#include "core/util.h"

namespace bacon
{
	Renderer2D::Renderer2D(uint32_t width, uint32_t height)
	{
		this->create_frame(width, height);

		for (size_t i = 0; i < _MAX_LAYERS; i++)
		{
			this->m_layers[i] =
				(RenderLayer){i, true, std::vector<Object2D*>()};
		}
	}

	void Renderer2D::create_frame(uint32_t width, uint32_t height)
	{
		this->m_frame_width = width;
		this->m_frame_height = height;

		if (IsRenderTextureValid(this->frame))
		{
			UnloadRenderTexture(this->frame);
		}
		this->frame = LoadRenderTexture(m_frame_width, m_frame_height);
	}

	void Renderer2D::add_to_layer(Object2D* object, size_t layer)
	{
		if (layer < 0 || layer >= _MAX_LAYERS)
		{
			debug_error("Layer %ul is out of bounds", layer);
		}

		size_t layer_val = std::clamp(layer, (size_t)0, _MAX_LAYERS);
		this->m_layers[layer_val].objects.push_back(object);
	}

	void Renderer2D::remove_from_layer(Object2D* object)
	{
		size_t current_layer = object->get_layer();

		bool found = false;
		RenderLayer* layer = &this->m_layers[current_layer];
		for (auto it = layer->objects.begin(); it != layer->objects.end(); it++)
		{
			Object2D* obj = *it;
			if (obj->get_uuid() == object->get_uuid())
			{
				layer->objects.erase(it);
				found = true;
				break;
			}
		}

		if (!found)
		{
			debug_warn("Object not found at expected RenderLayer (This is "
					   "probably fine.)");
		}
	}

	void Renderer2D::draw(Camera2D* camera) const
	{
		BeginTextureMode(this->frame);
		ClearBackground(DARKGRAY);

		BeginMode2D(*camera);
		for (const RenderLayer& layer : this->m_layers)
		{
			for (Object2D* object : layer.objects)
			{
				object->draw();

				if (ui::view_properties_object == object)
				{
					object->draw_outline();
				}
			}
		}
		EndMode2D();

		EndTextureMode();
	}

	void Renderer2D::reset()
	{
		for (RenderLayer& layer : this->m_layers)
		{
			layer.objects.clear();
		}
	}

	uint32_t Renderer2D::get_width() const
	{
		return this->m_frame_width;
	}

	uint32_t Renderer2D::get_height() const
	{
		return this->m_frame_height;
	}
} // namespace bacon
