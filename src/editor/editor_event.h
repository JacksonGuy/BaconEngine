#pragma once

#include <cstdint>
#include <stack>

#include "core/2D/game_object.h"
#include "core/2D/entity.h"
#include "core/2D/text_object.h"
#include "core/2D/camera_object.h"
#include "editor/editor.h"

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

		typedef struct EventBase
		{
		    EventBase() = default;
			virtual ~EventBase() = default;
			virtual void apply(EventAction action) = 0;
		} EventBase;

		typedef struct ObjectEvent : EventBase
		{
		    GameObject* object;
		    GameObject* before;
			GameObject* after;

			ObjectEvent(GameObject* before, GameObject* after);
			~ObjectEvent();
			void apply(EventAction action) override;
		} ObjectEvent;

		typedef struct TreeEvent : EventBase
		{
		    GameObject* object;
			GameObject* old_parent;
			GameObject* new_parent;

			TreeEvent();
			~TreeEvent() = default;
			void apply(EventAction action) override;
		} TreeEvent;

		typedef struct EditorEvent : EventBase
		{
		    EditorSnapshot* before;
			EditorSnapshot* after;

			EditorEvent();
			~EditorEvent();
			void apply(EventAction action) override;
		} EditorEvent;

		extern std::stack<EventBase*> undo_stack;
		extern std::stack<EventBase*> redo_stack;

		void push_event(EventBase* event);
		void undo_event();
		void redo_event();

		void event_cleanup();
	} // namespace event
} // namespace bacon
