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
			this->object_uuid = before->uuid;
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
			assert(action != EventAction::NONE);

			GameObject* object = GameState::scene.find_object_by_uuid(object_uuid);
			if (object == nullptr)
			{
				debug_error("Failed to find object in scene!");
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

			globals::has_unsaved_changes = true;
		}

		TreeEvent::TreeEvent()
		{
			old_parent = nullptr;
			new_parent = nullptr;
		}

		void TreeEvent::apply(EventAction action)
		{
			assert(action != EventAction::NONE);

			GameObject* object = GameState::scene.find_object_by_uuid(object_uuid);
			if (object == nullptr)
			{
				debug_error("Failed to find object in scene!");
				return;
			}

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
			}

			globals::has_unsaved_changes = true;
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

			if (action == EventAction::UNDO)
			{
				before->apply();
			}
			else if (action == EventAction::REDO)
			{
				after->apply();
			}

			ui::set_input_buffers();
			globals::has_unsaved_changes = true;
		}

		ObjectCreateEvent::ObjectCreateEvent(const GameObject& object)
		{
			this->object_copy = object.clone();
			this->object_copy->clone_children(object, false);
		}

		ObjectCreateEvent::~ObjectCreateEvent()
		{
			// object_copy->delete_children();
			delete object_copy;
		}

		void ObjectCreateEvent::apply(EventAction action)
		{
			assert(object_copy != nullptr);
			assert(action != EventAction::NONE);

			if (action == EventAction::UNDO)
			{
				if (ui::view_properties_object != nullptr &&
					ui::view_properties_object->uuid == object_copy->uuid)
				{
					ui::view_properties_object = nullptr;
				}

				GameObject* scene_object = GameState::scene.find_object_by_uuid(object_copy->uuid);
				assert(scene_object != nullptr);

				// scene_object->remove_from_scene();
				// scene_object->delete_children();
				delete scene_object;
			}
			else if (action == EventAction::REDO)
			{
				GameObject* new_object = object_copy->clone();
				new_object->clone_children(*object_copy, true);
				new_object->add_to_scene();
			}

			globals::has_unsaved_changes = true;
		}

		ObjectDeleteEvent::ObjectDeleteEvent(const GameObject& object)
		{
			this->object_copy = object.clone();
			this->object_copy->clone_children(object, false);
		}

		ObjectDeleteEvent::~ObjectDeleteEvent()
		{
			// object_copy->delete_children();
			delete object_copy;
		}

		void ObjectDeleteEvent::apply(EventAction action)
		{
			assert(object_copy != nullptr);
			assert(action != EventAction::NONE);

			if (action == EventAction::UNDO)
			{
				GameObject* new_object = object_copy->clone();
				new_object->clone_children(*object_copy, true);
				new_object->add_to_scene();
			}
			else if (action == EventAction::REDO)
			{
				GameObject* scene_object = GameState::scene.find_object_by_uuid(object_copy->uuid);
				assert(scene_object != nullptr);

				// scene_object->remove_from_scene();
				// scene_object->delete_children();
				delete scene_object;
			}

			globals::has_unsaved_changes = true;
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
