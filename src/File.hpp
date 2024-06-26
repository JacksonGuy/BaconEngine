#ifndef FILE_H
#define FILE_H

#include <string>
#include "Entity.hpp"

struct ConfigState {
    std::string version;
    std::string resolution;
};

void save(std::string filename);
bool load(std::string filename);

void saveConfig(ConfigState state);
ConfigState loadConfig();

void savePrefab(std::string filename, Entity* e);
int loadPrefab(std::string filename);

#endif
