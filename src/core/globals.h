#pragma once

#include <string>

#include "raylib.h"

namespace bacon
{
    namespace globals
    {
        inline std::string engine_version;
        inline std::string editor_default_font_path;

        inline bool is_project_loaded;
        inline bool has_unsaved_changes;
        inline std::string project_title = "Untitled Project";
        inline std::string project_directory;
        inline std::string project_file;

        inline void update_window_title()
        {
            std::string window_title = "Bacon - ";
            if (globals::has_unsaved_changes)
            {
                window_title += globals::project_title + " (*)";
            }
            else
            {
                window_title += globals::project_title;
            }

            SetWindowTitle(window_title.c_str());
        }
    } // namespace globals
} // namespace bacon
