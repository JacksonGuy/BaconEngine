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
		EditorEvent undo_event();
		EditorEvent redo_event();
	} // namespace event
} // namespace bacon
