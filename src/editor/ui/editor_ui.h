#pragma once

#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "core/2D/game_manager.h"

namespace bacon {
    namespace ui {
        const size_t _BUF_SIZE = 64;

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

        inline GameObject* view_properties_object = nullptr;

        void init(uint32_t width, uint32_t height);

        void draw_top_bar();
        void draw_object_properties();
        void draw_object_tree(GameManager* manager);
        void draw_scene_display(Renderer* renderer);
        void draw_engine_console();
        void draw_settings();
        void draw_general_info_display();

        void game_object_tree_recurse(GameObject* object);

        void SetImGuiStyle();
    }
}
