#include <chrono>
#include <iostream>

#include "editor/editor_event.h"
#include "file/file.h"
#include "imgui.h"
#include "lib/pool_allocator.h"
#include "nfd.h"
#include "raylib.h"
#include "rlImGui.h"

#include "core/2D/entity.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor.h"
#include "editor/ui/editor_ui.h"
#include "core/2D/camera_object.h"
#include "core/game_state.h"
#include "lib/arena.h"

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
	editor.framerate_limit = 165;
	editor.is_playing = false;

	// Debug for quick startup
	globals::project_directory = "/home/jackson/BaconEngine/projects/test2";
	globals::project_file =
		"/home/jackson/BaconEngine/projects/test2/game.json";
	file::load_project(false);

	debug_log("Engine startup successful.");

	while (globals::program_running)
	{
		if (editor.is_playing)
		{
			// Do physics step
			GameState::scene.simulation_step();
		}
		else
		{
			editor.editor_input();
		}

		BeginDrawing();
		ClearBackground(LIGHTGRAY);

		Camera2D* drawing_camera = nullptr;
		if (editor.is_playing)
		{
			drawing_camera = &GameState::scene.get_active_camera()->camera;
		}
		else
		{
			drawing_camera = &editor.camera;
		}
		GameState::scene.draw_entities(drawing_camera);

		editor.draw_ui();
		EndDrawing();
	}

	debug_log("Performing cleanup...");
	event::event_cleanup();

	rlImGuiShutdown();
	CloseWindow();

	return 0;
}
