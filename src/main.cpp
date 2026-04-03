#include <chrono>
#include <iostream>

#include "editor/editor_event.h"
#include "file/file.h"
#include "imgui.h"
#include "lib/pool_allocator.h"
#include "nfd.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"

#include "core/2D/entity_2d.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor.h"
#include "editor/ui/editor_ui.h"
#include "core/2D/camera_object.h"
#include "core/game_state.h"
#include "lib/pool_allocator.h"

int main(int argc, char** argv)
{
	using namespace bacon;

	debug_log("Starting BaconEngine...");

	globals::engine_version = "v0.1";

	// Disable RayLib logging
	SetTraceLogLevel(LOG_WARNING);

	// NativeFileDialog
	NFD_Init();
	debug_log("NativeFileDialog initialized.");

	// Setup
	Editor editor;
	editor.set_framerate_limit(165);
	editor.is_playing = false;
	globals::editor_ref = &editor;

	// Debug for quick startup
	globals::project_directory = "/home/jackson/BaconEngine/projects/test2";
	globals::project_file =
		"/home/jackson/BaconEngine/projects/test2/game.json";
	file::load_project(false);
	GameState::game_type = GameState::GameType::GAME_2D; // TODO

	debug_log("Engine startup successful.");

	while (globals::program_running)
	{
		if (editor.is_playing)
		{
			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				GameState::state_2d->scene->simulation_step();
			}
		}
		else
		{
			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				editor.editor_input_2d();
			}
		}

		BeginDrawing();
		ClearBackground(LIGHTGRAY);

		if (editor.is_playing)
		{
			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				GameState::state_2d->scene->draw_entities();
			}
		}
		else
		{
			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				GameState::state_2d->scene->draw_entities(&editor.camera);
			}
		}

		editor.draw_ui();
		EndDrawing();
	}

	debug_log("Performing cleanup...");
	event::event_cleanup();
	GameState::cleanup();
	if (ui::inspect_object_copy)
	{
		ui::inspect_object_copy->delete_children();
		delete ui::inspect_object_copy;
	}

	rlImGuiShutdown();
	CloseWindow();

	return 0;
}
