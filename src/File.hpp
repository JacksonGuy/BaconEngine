#ifndef FILE_H
#define FILE_H

#include <string>
#include "Entity.hpp"

namespace File {
    struct ConfigState {
        std::string version;
        std::string resolution;
    };

    void save(std::string filename);
    bool load(std::string filename);

    bool CreateNew(std::string& path);

    std::string toAbsolute(std::string path);

    void saveConfig(ConfigState state);
    ConfigState loadConfig();

    void savePrefab(std::string filename, Entity* e);
    Entity* loadPrefab(std::string filename);
}

#endif
