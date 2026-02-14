#include "editor_event.h"

#include "core/2D/camera_object.h"
#include "core/2D/entity.h"
#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "core/util.h"
#include "core/game_state.h"

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
					Entity* obj_cast = (Entity*)object;
					Entity* new_entity = new Entity(*obj_cast);
					event.object = new_entity;
					break;
				}

				case ObjectType::TEXT:
				{
					TextObject* obj_cast = (TextObject*)object;
					TextObject* new_text = new TextObject(*obj_cast);
					event.object = new_text;
					break;
				}

				case ObjectType::CAMERA:
				{
					CameraObject* obj_cast = (CameraObject*)object;
					CameraObject* new_camera = new CameraObject(*obj_cast);
					event.object = new_camera;
					break;
				}

				default:
				{
					break;
				}
			}

			event_stack.push(event);

			debug_log("Event pushed to stack");
		}

		void undo_event()
		{
			if (event_stack.empty())
			{
			    return;
			}

			EditorEvent event = event_stack.top();
			undo_stack.push(event);
			event_stack.pop();

			if (event.type == EventType::OBJECT_EDIT)
			{
                GameObject* object = GameState::scene.find_object_by_uuid(event.object->uuid);

                if (object == nullptr)
                {
                    debug_error("Could not find object!");
                    return;
                }

                switch(object->object_type)
                {
                    case ENTITY:
                    {
                        Entity* entity = (Entity*)object;
                        Entity* event_entity = (Entity*)event.object;

                        entity = event_entity;

                        delete event_entity;

                        break;
                    }

                    case TEXT:
                    {
                        TextObject* text = (TextObject*)object;
                        TextObject* event_text = (TextObject*)event.object;

                        text = event_text;

                        delete event_text;

                        break;
                    }

                    case CAMERA:
                    {
                        CameraObject* camera = (CameraObject*)object;
                        CameraObject* event_camera = (CameraObject*)event.object;

                        camera = event_camera;

                        delete event_camera;

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
			}
		}

		void redo_event()
		{
			EditorEvent event = undo_stack.top();
			undo_stack.pop();
			event_stack.push(event);

			debug_error("This function has not been implemented yet.");
		}

		void event_cleanup()
		{
		    while (!event_stack.empty())
			{
			    EditorEvent event = event_stack.top();
				event_stack.pop();

				delete event.object;
			}
		}
	} // namespace event
} // namespace bacon
