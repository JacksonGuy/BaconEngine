#pragma once

#include "core/2D/game_manager.h"
#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "editor/editor.h"

namespace bacon
{
namespace ui
{
enum class LastEditorAction : uint16_t
{
    NONE = 0,
    PROJECT_NEW,
    PROJECT_SAVE_AS,
    PROJECT_OPEN,
};

const size_t _BUF_SIZE = 64;

inline bool project_was_modified = false;

inline bool show_test = true;
inline bool show_object_properties = true;
inline bool show_object_tree = true;
inline bool show_scene_display = true;
inline bool show_console = true;
inline bool show_settings = false;
inline bool show_general_info = true;

inline bool show_entity_create = false;
inline bool show_text_create = false;
inline bool show_camera_create = false;

inline LastEditorAction last_action = LastEditorAction::NONE;
inline bool show_save_confirm_popup = false;

inline GameObject* view_properties_object = nullptr;

void init(uint32_t width, uint32_t height);

void draw_top_bar(GameManager& manager);
void draw_object_properties();
void draw_object_tree(GameManager& manager);
void draw_scene_display(Renderer* renderer);
void draw_engine_console(Editor* editor);
void draw_settings();
void draw_general_info_display(Editor* editor);

void draw_entity_create(GameManager& manager);
void draw_text_create(GameManager& manager);
void draw_camera_create(GameManager& manager);

void game_object_tree_recurse(GameObject* object);

void SetImGuiStyle();
} // namespace ui
} // namespace bacon
