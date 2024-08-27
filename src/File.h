#pragma once

#include <string>

namespace File {
    bool SaveProject(std::string filename);
    bool LoadProject();
    bool CreateNewProject(std::string& path, bool reset);
};