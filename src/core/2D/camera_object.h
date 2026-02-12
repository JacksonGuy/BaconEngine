#pragma once

#include "core/2D/game_object.h"
#include "lib/pool_allocator.h"

namespace bacon
{
	class CameraObject : public GameObject
	{
	public:
		Camera2D camera;
		bool is_active;
		float zoom;

		CameraObject();
		CameraObject(uint8_t* bytes);
		CameraObject(const CameraObject& camera);
		CameraObject& operator=(const CameraObject& camera) = delete;
		CameraObject(CameraObject&& camera) = delete;
		CameraObject& operator=(CameraObject&& camera) = delete;
		~CameraObject() = default;

		void* operator new(size_t ptr);
		void operator delete(void* ptr, size_t size);

		void move_camera(Vector2 delta);
		void set_position(Vector2 position);
		void calculate_size(Vector2 window_size);

		void draw() const override;
		void draw_properties_editor() override;
		void save_to_json(nlohmann::json& data) const override;
		void load_from_json(nlohmann::json& data) override;
		size_t calculate_size() const override;
		uint8_t* serialize() const override;
		void deserialize(uint8_t* bytes) override;

	private:
		static PoolAllocator _allocator;
	};
} // namespace bacon
