#pragma once

#include <string>

namespace File {
    bool SaveProject(std::string filename);
    bool LoadProject(std::string filename);
    bool CreateNewProject(std::string& path, bool reset);
};