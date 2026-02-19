#include "editor_event.h"

#include "core/2D/camera_object.h"
#include "core/2D/entity.h"
#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "core/util.h"
#include "core/game_state.h"
#include "core/globals.h"
#include "editor/editor.h"
#include "editor/ui/editor_ui.h"

namespace bacon
{
	namespace event
	{
		std::stack<EventBase*> undo_stack;
		std::stack<EventBase*> redo_stack;

		ObjectEvent::ObjectEvent(GameObject* before, GameObject* after)
		{
		    assert(before != nullptr && after != nullptr);
			assert(before->uuid == after->uuid);

			// Allocate new objects
			this->object = nullptr;
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

			if (object == nullptr)
			{
			    object = GameState::scene.find_object_by_uuid(before->uuid);

				if (object == nullptr)
				{
				    debug_error("Failed to find object!");
					return;
				}
			}

			if (action == EventAction::UNDO)
			{
			    object->copy(*this->before);

				globals::has_unsaved_changes = true;
			}
			else if (action == EventAction::REDO)
			{
			    object->copy(*this->after);

				globals::has_unsaved_changes = true;
			}

			object->update_buffers();
		}

		TreeEvent::TreeEvent()
		{
		    object = nullptr;
			old_parent = nullptr;
			new_parent = nullptr;
		}

		void TreeEvent::apply(EventAction action)
		{
		    assert(object != nullptr);
			assert(action != EventAction::NONE);

			if (action == EventAction::UNDO)
			{
			    object->set_parent(old_parent);
				if (old_parent != nullptr)
				{
				    old_parent->add_child(object);
				}
				if (new_parent != nullptr)
				{
				    new_parent->remove_child(object);
				}

				globals::has_unsaved_changes = true;
			}
			else if (action == EventAction::REDO)
			{
			    object->set_parent(new_parent);
				if (new_parent != nullptr)
				{
				    new_parent->add_child(object);
				}
				if (old_parent != nullptr)
				{
				    old_parent->remove_child(object);
				}

				globals::has_unsaved_changes = true;
			}
		}

		EditorEvent::EditorEvent()
		{
		    before = nullptr;
			after = nullptr;
		}

		EditorEvent::~EditorEvent()
		{
		    delete before;
			delete after;
		}

		void EditorEvent::apply(EventAction action)
		{
		    assert(before != nullptr);
			assert(after != nullptr);
			assert(action != EventAction::NONE);

			Editor* editor = globals::editor_ref;

			if (action == EventAction::UNDO)
			{
			    before->apply();

				globals::has_unsaved_changes = true;
			}
			else if (action == EventAction::REDO)
			{
			    after->apply();

				globals::has_unsaved_changes = true;
			}

			ui::set_input_buffers();
		}

		void push_event(EventBase* event)
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

			EventBase* event = undo_stack.top();
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

			EventBase* event = redo_stack.top();
			redo_stack.pop();
			undo_stack.push(event);

			event->apply(EventAction::REDO);
		}

		void event_cleanup()
		{
		    while (!undo_stack.empty())
			{
			    EventBase* event = undo_stack.top();
				undo_stack.pop();

				delete event;
			}

			while (!redo_stack.empty())
			{
			    EventBase* event = redo_stack.top();
				redo_stack.pop();

				delete event;
			}
		}
	} // namespace event
} // namespace bacon
