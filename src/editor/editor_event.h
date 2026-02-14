#pragma once

#include <stdint.h>

#include <stack>

#include "core/2D/game_object.h"

namespace bacon
{
	namespace event
	{
		enum class EventType : uint32_t
		{
			NONE = 0,
			OBJECT_EDIT,
			STATE_EDIT
		};

		typedef struct EditorEvent
		{
			EventType type;
			GameObject* object;
		} EditorEvent;

		extern std::stack<EditorEvent> event_stack;
		extern std::stack<EditorEvent> undo_stack;

		void push_event(EventType type, GameObject* object);
		void undo_event();
		void redo_event();
		void event_cleanup();
	} // namespace event
} // namespace bacon
