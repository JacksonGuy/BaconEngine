#pragma once

#include "core/2D/game_object.h"
#include "lib/pool_allocator.h"
#include "lib/arena.h"

namespace bacon
{
    typedef struct
    {
        bool is_active;
        float zoom;
    } CameraBuffers;

	class CameraObject : public GameObject
	{
	public:
	    static Arena<CameraObject> _allocator;
		static void* operator new(size_t size);
		static void* operator new(size_t size, void* ptr);
		static void operator delete(void* ptr, size_t size);

		Camera2D camera;
		bool is_active;
		float zoom;

		CameraObject();
		CameraObject(uint8_t* bytes);
		CameraObject(const CameraObject& camera);
		CameraObject& operator=(const CameraObject& camera);
		CameraObject(CameraObject&& camera) = delete;
		CameraObject& operator=(CameraObject&& camera) = delete;
		~CameraObject() = default;

		void copy(const GameObject& object) override;
		CameraObject* clone() const override;

		void add_to_state() override;

		void move_camera(Vector2 delta);
		void set_position(Vector2 position);
		void calculate_size(Vector2 window_size);

		void update_buffers() override;
		void update_from_buffers() override;

		void draw() const override;
		void draw_properties_editor() override;
		void save_to_json(nlohmann::json& data) const override;
		void load_from_json(nlohmann::json& data) override;
		size_t calculate_size() const override;
		uint8_t* serialize() const override;
		void deserialize(uint8_t* bytes) override;

	private:
	    CameraBuffers m_buffers;
	};
} // namespace bacon
