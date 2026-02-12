#include "editor_event.h"

#include "core/2D/camera_object.h"
#include "core/2D/entity.h"
#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "core/util.h"

namespace bacon
{
	namespace event
	{
		std::stack<EditorEvent> event_stack;
		std::stack<EditorEvent> undo_stack;

		void push_event(EventType type, GameObject* object)
		{
			EditorEvent event = {.type = type, .object = nullptr};

			switch (object->object_type)
			{
				case ObjectType::ENTITY:
				{
					// Entity* obj_cast = (Entity*)object;
					// Entity* new_entity = object->manager->;
					break;
				}

				case ObjectType::TEXT:
				{
					// obj_cast = (TextObject*)object;
					break;
				}

				case ObjectType::CAMERA:
				{
					// obj_cast = (CameraObject*)object;
					break;
				}

				default:
				{
					break;
				}
			}

			// event_stack.push();

			debug_log("Event pushed to stack");
		}

		EditorEvent undo_event()
		{
			if (event_stack.empty())
			{
				return (EditorEvent){.type = EventType::NONE,
									 .object = nullptr};
			}

			EditorEvent event = event_stack.top();
			undo_stack.push(event);
			event_stack.pop();
			return event;
		}

		EditorEvent redo_event()
		{
			EditorEvent event = undo_stack.top();
			undo_stack.pop();
			event_stack.push(event);
			return event;
		}
	} // namespace event
} // namespace bacon
