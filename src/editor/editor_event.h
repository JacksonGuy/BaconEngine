#pragma once

#include <cstdint>
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

		enum class EventAction : uint8_t
		{
		    NONE = 0,
			UNDO,
			REDO
		};

		typedef struct EditorEvent
		{
			EventType type;
			GameObject* before;
			GameObject* after;
		} EditorEvent;

		extern std::stack<EditorEvent> undo_stack;
		extern std::stack<EditorEvent> redo_stack;

		void push_event(EditorEvent event);
		void undo_event();
		void redo_event();
		void apply_event(EditorEvent event, EventAction action);

		EditorEvent make_object_event(const GameObject* before, const GameObject* after);
		void event_cleanup();
	} // namespace event
} // namespace bacon
