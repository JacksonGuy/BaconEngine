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
		std::stack<EditorEvent*> undo_stack;
		std::stack<EditorEvent*> redo_stack;

		ObjectEvent::ObjectEvent(GameObject* before, GameObject* after)
		{
		    assert(before != nullptr && after != nullptr);
			assert(before->uuid == after->uuid);

			// Allocate new objects
			this->before = before->clone();
			this->after = after->clone();
		}

		ObjectEvent::~ObjectEvent()
		{
		    delete before;
			delete after;
		}

		void ObjectEvent::apply(EventAction action)
		{
		    assert(before != nullptr && after != nullptr);
			assert(before->uuid == after->uuid);
			assert(before->object_type == after->object_type);
			assert(action != EventAction::NONE);

			GameObject* object = GameState::scene.find_object_by_uuid(before->uuid);

			if (object == nullptr)
			{
			    debug_error("Could not find object in the scene!");
				return;
			}

			if (action == EventAction::UNDO)
			{
			    object->copy(*this->before);
			}
			else if (action == EventAction::REDO)
			{
			    object->copy(*this->after);
			}
		}

		void push_event(EditorEvent* event)
		{
		    undo_stack.push(event);

			debug_log("Event pushed to stack.");
		}

		void undo_event()
		{
			if (undo_stack.empty())
			{
			    return;
			}

			debug_log("Undoing event...");

			EditorEvent* event = undo_stack.top();
			undo_stack.pop();
			redo_stack.push(event);

			event->apply(EventAction::UNDO);
		}

		void redo_event()
		{
		    if (redo_stack.empty())
			{
                return;
			}

			debug_log("Redoing event...");

			EditorEvent* event = redo_stack.top();
			redo_stack.pop();
			undo_stack.push(event);

			event->apply(EventAction::REDO);
		}

		void event_cleanup()
		{
		    while (!undo_stack.empty())
			{
			    EditorEvent* event = undo_stack.top();
				undo_stack.pop();

				delete event;
			}

			while (!redo_stack.empty())
			{
			    EditorEvent* event = redo_stack.top();
				redo_stack.pop();

				delete event;
			}
		}
	} // namespace event
} // namespace bacon
