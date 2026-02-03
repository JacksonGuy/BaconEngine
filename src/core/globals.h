#pragma once

#include <string>

namespace bacon {
    namespace globals {
        inline std::string engine_version;

        inline bool is_project_loaded;
        inline bool has_changes_made;
        inline std::string project_directory;
        inline std::string project_file;
    }
}
