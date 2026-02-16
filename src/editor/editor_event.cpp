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

		void push_event(EditorEvent event)
		{
			event_stack.push(event);

			debug_log("Event pushed to stack");
		}

		void undo_event()
		{
			if (event_stack.empty())
			{
			    return;
			}

			debug_log("Undoing event...");

			EditorEvent event = event_stack.top();
			event_stack.pop();
			undo_stack.push(event);

			apply_event(event, EventAction::UNDO);
		}

		void redo_event()
		{
		    if (undo_stack.empty())
			{
                return;
			}

			debug_log("Redoing event...");

			EditorEvent event = undo_stack.top();
			undo_stack.pop();
			event_stack.push(event);

			apply_event(event, EventAction::REDO);
		}

		void apply_event(EditorEvent event, EventAction action)
		{
		    assert(event.before->object_type == event.after->object_type);
			assert(event.after->uuid == event.after->uuid);
			assert(action != EventAction::NONE);

            if (event.type == EventType::OBJECT_EDIT)
            {
                GameObject* object = GameState::scene.find_object_by_uuid(event.before->uuid);

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

                        if (action == EventAction::UNDO)
                        {
                            Entity* event_entity = (Entity*)event.before;
                            entity->copy(*event_entity);
                        }
                        else if (action == EventAction::REDO)
                        {
                            Entity* event_entity = (Entity*)event.after;
                            entity->copy(*event_entity);
                        }

                        break;
                    }

                    case TEXT:
                    {
                        TextObject* text = (TextObject*)object;

                        if (action == EventAction::UNDO)
                        {
                            TextObject* event_text = (TextObject*)event.before;
                            text->copy(*event_text);
                        }
                        else if (action == EventAction::REDO)
                        {
                            TextObject* event_text = (TextObject*)event.after;
                            text->copy(*event_text);
                        }

                        break;
                    }

                    case CAMERA:
                    {
                        CameraObject* camera = (CameraObject*)object;

                        if (action == EventAction::UNDO)
                        {
                            CameraObject* event_camera = (CameraObject*)event.before;
                            camera->copy(*event_camera);
                        }
                        else if (action == EventAction::REDO)
                        {
                            CameraObject* event_camera = (CameraObject*)event.after;
                            camera->copy(*event_camera);
                        }

                        break;
                    }

                    default:
                    {
                        debug_error("Default Case!");
                        break;
                    }
                }
            }
		}

		EditorEvent make_object_event(
            const GameObject* before,
			const GameObject* after
		) {
		    assert(before->object_type == after->object_type);
			assert(before->uuid == after->uuid);

		    EditorEvent event = {
				.type = EventType::OBJECT_EDIT,
				.before = nullptr,
				.after = nullptr
			};

			switch (before->object_type)
			{
			    case ENTITY:
				{
				    Entity* before_copy = new Entity(*(Entity*)before);
					Entity* after_copy = new Entity(*(Entity*)after);
					before_copy->uuid = before->uuid;
					after_copy->uuid = after->uuid;
					event.before = before_copy;
					event.after = after_copy;
					break;
				}

				case TEXT:
			    {
                    TextObject* before_copy = new TextObject(*(TextObject*)before);
                    TextObject* after_copy = new TextObject(*(TextObject*)after);
                    before_copy->uuid = before->uuid;
                    after_copy->uuid = after->uuid;
                    event.before = before_copy;
                    event.after = after_copy;
                    break;
				}

                case CAMERA:
                {
                    CameraObject* before_copy = new CameraObject(*(CameraObject*)before);
                    CameraObject* after_copy = new CameraObject(*(CameraObject*)after);
                    before_copy->uuid = before->uuid;
                    after_copy->uuid = after->uuid;
                    event.before = before_copy;
                    event.after = after_copy;
                    break;
                }

                default:
                {
                    debug_error("Default Case!");
                    break;
                }
			}

            return event;
		}

		void event_cleanup()
		{
		    while (!event_stack.empty())
			{
			    EditorEvent event = event_stack.top();
				event_stack.pop();

				delete event.before;
                delete event.after;
			}

			while (!undo_stack.empty())
			{
			    EditorEvent event = undo_stack.top();
				undo_stack.pop();

				delete event.before;
                delete event.after;
			}
		}
	} // namespace event
} // namespace bacon
