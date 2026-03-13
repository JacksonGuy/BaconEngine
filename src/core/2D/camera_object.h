#pragma once

#include "core/2D/object_2d.h"
#include "lib/pool_allocator.h"

namespace bacon
{
	class CameraObject : public Object2D
	{
	public:
		static PoolAllocator<CameraObject> _allocator;
		static void* operator new(size_t size);
		static void* operator new(size_t size, void* ptr);
		static void operator delete(void* ptr);
		static void operator delete(void* ptr, size_t size);

		static bool classof(const GameObject* obj)
		{
			return obj->get_type_id() == TypeID::CAMERA_2D;
		}
		static constexpr TypeID static_type_id = TypeID::CAMERA_2D;

		Camera2D camera;
		bool is_active;
		float zoom;

		CameraObject();
		CameraObject(ByteStream& bytes);
		CameraObject(const CameraObject& camera);
		CameraObject& operator=(const CameraObject& camera);
		CameraObject(CameraObject&& camera) = delete;
		CameraObject& operator=(CameraObject&& camera) = delete;
		~CameraObject();

		// void destroy() override;
		void copy(const GameObject& object) override;
		CameraObject* clone() const override;
		CameraObject* clone_unique() const override;

		void add_to_scene() override;
		void remove_from_scene() override;

		void move_camera(Vector2 delta);
		void set_camera_position(Vector2 position);

		void draw_outline() const override;

		void update_ui_buffer() const override;
		void update_from_ui_buffer() override;

		void draw() const override;
		void draw_properties_editor() override;
		void save_to_json(nlohmann::json& data) const override;
		void load_from_json(const nlohmann::json& data) override;
		ByteStream serialize() const override;

	protected:
		void deserialize(ByteStream& bytes) override;
	};
} // namespace bacon
