#pragma once

#include <cstdint>
#include <stack>

#include "core/game_object.h"
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
			UUID object_uuid;
			GameObject* before;
			GameObject* after;

			ObjectEvent(GameObject* before, GameObject* after);
			~ObjectEvent();
			void apply(EventAction action) override;
		} ObjectEvent;

		typedef struct TreeEvent : EventBase
		{
			UUID object_uuid;
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

			EditorEvent() = default;
			~EditorEvent();
			void apply(EventAction action) override;
		} EditorEvent;

		typedef struct ObjectCreateEvent : EventBase
		{
			GameObject* object_copy;

			ObjectCreateEvent(const GameObject& object);
			~ObjectCreateEvent();
			void apply(EventAction action) override;
		} ObjectCreateEvent;

		typedef struct ObjectDeleteEvent : EventBase
		{
			GameObject* object_copy;

			ObjectDeleteEvent(const GameObject& object);
			~ObjectDeleteEvent();
			void apply(EventAction action) override;
		} ObjectDeleteEvent;

		extern std::stack<EventBase*>
			undo_stack;
		extern std::stack<EventBase*> redo_stack;

		void push_event(EventBase* event);
		void undo_event();
		void redo_event();

		void event_cleanup();
	} // namespace event
} // namespace bacon
