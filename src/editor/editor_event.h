#pragma once

#include <cstdint>
#include <stack>

#include "core/2D/game_object.h"
#include "core/2D/entity.h"
#include "core/2D/text_object.h"
#include "core/2D/camera_object.h"

namespace bacon
{
	namespace event
	{
		enum class EventAction : uint8_t
		{
		    NONE = 0,
			UNDO,
			REDO
		};

		typedef struct EditorEvent
		{
		    EditorEvent() = default;
			virtual ~EditorEvent() = default;
			virtual void apply(EventAction action) = 0;
		} EditorEvent;

		typedef struct ObjectEvent : EditorEvent
		{
		    GameObject* before;
			GameObject* after;

			ObjectEvent(GameObject* before, GameObject* after);
			~ObjectEvent();
			void apply(EventAction action) override;
		} ObjectEvent;

		typedef struct TreeEvent : EditorEvent
		{
		    GameObject* object;
			GameObject* old_parent;
			GameObject* new_parent;

			TreeEvent();
			~TreeEvent();
			void apply(EventAction action) override;
		} TreeEvent;

		extern std::stack<EditorEvent*> undo_stack;
		extern std::stack<EditorEvent*> redo_stack;

		void push_event(EditorEvent* event);
		void undo_event();
		void redo_event();

		void event_cleanup();
	} // namespace event
} // namespace bacon
