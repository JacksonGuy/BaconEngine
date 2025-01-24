#include <iostream>
#include <fstream>

#include "imgui.h"
#include <nfd.h>

#include "Entity.h"
#include "Rendering.h"
#include "GameManager.h"
#include "Editor.h"

Entity::Entity() : GameObject() {
    type = ENTITY;
    
    texture = {0};
    texturePath = "";

    bodytype = STATIC;
    solid = false;
    physicsObject = false;
    grounded = false;
    velocity = {0,0};

    lua_scripts = {};
    variables = {};

    GameManager::Entities.push_back(this);
}

Entity::Entity(const Entity* entity) : GameObject(entity) {
    type = ENTITY;

    // SetTexture(entity->texturePath);
    std::string relativePath = std::filesystem::relative(
        entity->texturePath.c_str(), GameManager::projectEntryPath).generic_string();
    SetTexture(relativePath);
    UpdateRect();

    bodytype = entity->bodytype;
    solid = entity->solid;
    physicsObject = entity->physicsObject;
    
    grounded = false;
    velocity = {0,0};

    lua_scripts = entity->lua_scripts;
    variables = entity->variables;
    
    GameManager::Entities.push_back(this);
}

Entity::~Entity() {
    // Remove from Entities list
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        if (GameManager::Entities[i]->ID == ID) {
            GameManager::Entities.erase(GameManager::Entities.begin() + i);
            break;
        }
    }

    lua_scripts.clear();
    variables.clear();

    UnloadTexture(texture);
}

/**
 * @brief Changes the entity's texture
 * 
 * @param path 
 */
void Entity::SetTexture(std::string path) {
    std::string absPath = GameManager::projectEntryPath + "/" + std::string(path);   
    Image image = LoadImage(absPath.c_str());

    // Check if image was successfully loaded
    if (image.data == NULL) {
        GameManager::ConsoleError("Failed to load image: " + path);
        return;
    }

    UnloadTexture(this->texture);
    ImageResize(&image, size.x, size.y);
    this->texture = LoadTextureFromImage(image);
    this->texturePath = absPath;
    UnloadImage(image);
}

/**
 * @brief Updates the position and size of the entity's bounding rect
 * 
 */
void Entity::UpdateRect() {
    this->rect = {this->position.x, this->position.y, this->size.x, this->size.y};
}

/**
 * @brief Sets the value of a number variable. For Lua scripting.
 * 
 * @param name the name of the variable.
 * @param value the double value
 */
void Entity::SetVariable(std::string name, f64 value) {
    this->variables[name].numval = value;
}

/**
 * @brief Sets the value of a string variable. For Lua scripting.
 * 
 * @param name the name of the variable
 * @param value the string value
 */
void Entity::SetVariable(std::string name, std::string value) {
    this->variables[name].stringval = value;
}

/**
 * @brief Sets the value of a boolean variable. For Lua scripting.
 * 
 * @param name the name of the variable
 * @param value the boolean value
 */
void Entity::SetVariable(std::string name, bool value) {
    this->variables[name].boolval = value;
}

/**
 * @brief Return the value of a variable. For Lua scripting.
 * 
 * @param name the name of the variable
 * @return sol::lua_value the value 
 */
sol::lua_value Entity::GetVariable(std::string name) {
    // We don't want to create new variables when there isn't one
    if (variables.find(name) != variables.end()) {
        EntityVar var = variables[name];
        if (var.type == NUMBER) {
            return var.numval;
        }
        else if (var.type == STRING) {
            return var.stringval;
        }
        else if (var.type == BOOLEAN) {
            return var.boolval;
        }
    }

    GameManager::ConsoleError("Entity with ID=" + std::to_string(this->ID)
        + " has no variable with name " + name);
    return sol::make_object(GameManager::lua, sol::nil);
}

/**
 * @brief Writes entity data to JSON object
 * 
 * @param data JSON object to write to
 */
void Entity::SaveEntityJson(nlohmann::json& data) {
    SaveGameObjectJson(data);

    data["texture"] = std::filesystem::relative(texturePath, GameManager::projectEntryPath).generic_string();
    
    data["bodytype"] = bodytype;
    data["solid"] = solid;
    data["physicsObject"] = physicsObject;
    
    // Save Lua Scripts
    for (size_t i = 0; i < lua_scripts.size(); i++) {
        std::string script = lua_scripts[i];
        data["scripts"][i] = script;
    }

    // Save custom variables
    int i = 0;
    for (auto it = variables.begin(); it != variables.end(); it++) {
        data["variables"][i] = {
            {"name", it->first},
            {"type", it->second.type},
            {"numval", it->second.numval},
            {"stringval", it->second.stringval},
            {"boolval", it->second.boolval}
        };
        i++;
    }
}

/**
 * @brief Loads entity data from JSON object 
 * 
 * @param data JSON object to read from
 */
void Entity::LoadEntityJson(nlohmann::json& data) {
    LoadGameObjectJson(data);

    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        if (key == "texture") {
            this->SetTexture(std::string(value));
        }
        else if (key == "bodytype") {
            bodytype = PhysicsBody_t(value);
        }
        else if (key == "solid") {
            solid = value;
        }
        else if (key == "physicsObject") {
            physicsObject = value;
        }

        // Load scripts
        else if (key == "scripts") {
            for (size_t i = 0; i < data["scripts"].size(); i++) {
                lua_scripts.push_back(data["scripts"][i]);
            }
        }

        // Load variables
        else if (key == "variables") {
            for (size_t i = 0; i < data["variables"].size(); i++) {
                EntityVar var;
                var.type = EntityVar_t(data["variables"][i]["type"]);
                var.stringval = data["variables"][i]["stringval"];
                var.numval = data["variables"][i]["numval"];
                var.boolval = data["variables"][i]["boolval"];
                variables[data["variables"][i]["name"]] = var;
            }
        }
    }
}

/**
 * @brief Saves the Entity data to a JSON prefab file
 * 
 * @param data the file to save to
 */
void Entity::SaveEntityPrefab(nlohmann::json& data) {
    SaveGameObjectPrefab(data);

    data["texture"] = std::filesystem::relative(texturePath, GameManager::projectEntryPath).generic_string();
    
    data["bodytype"] = bodytype;
    data["solid"] = solid;
    data["physicsObject"] = physicsObject;
    
    // Save Lua Scripts
    for (size_t i = 0; i < lua_scripts.size(); i++) {
        std::string script = lua_scripts[i];
        data["scripts"][i] = script;
    }

    // Save custom variables
    int i = 0;
    for (auto it = variables.begin(); it != variables.end(); it++) {
        data["variables"][i] = {
            {"name", it->first},
            {"type", it->second.type},
            {"numval", it->second.numval},
            {"stringval", it->second.stringval},
            {"boolval", it->second.boolval}
        };
        i++;
    }
}

/**
 * @brief Loads Entity data from a JSON prefab file
 * 
 * @param data the file to load from
 */
void Entity::LoadEntityPrefab(nlohmann::json& data) {
    LoadGameObjectPrefab(data);

    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        if (key == "texture") {
            std::string absPath = GameManager::projectEntryPath + "/" + std::string(value);
            this->SetTexture(absPath);
        }
        else if (key == "bodytype") {
            bodytype = PhysicsBody_t(value);
        }
        else if (key == "solid") {
            solid = value;
        }
        else if (key == "physicsObject") {
            physicsObject = value;
        }

        // Load scripts
        else if (key == "scripts") {
            for (size_t i = 0; i < data["scripts"].size(); i++) {
                lua_scripts.push_back(data["scripts"][i]);
            }
        }

        // Load variables
        else if (key == "variables") {
            for (size_t i = 0; i < data["variables"].size(); i++) {
                EntityVar var;
                var.type = EntityVar_t(data["variables"][i]["type"]);
                var.stringval = data["variables"][i]["stringval"];
                var.numval = data["variables"][i]["numval"];
                var.boolval = data["variables"][i]["boolval"];
                variables[data["variables"][i]["name"]] = var;
            }
        }
    }
}

/**
 * @brief ImGui UI which shows details about the entity 
 * 
 */
void Entity::DrawPropertiesUI() {
    // ID
    std::string idText = "ID: " + std::to_string(ID);
    ImGui::Text(idText.c_str());

    // Name
    char nameBuff[Editor::BUFFSIZE];
    strcpy(nameBuff, name.c_str());
    if (ImGui::InputText("Name", nameBuff, Editor::BUFFSIZE)) {
        name = nameBuff;
    }

    // Tag
    char tagBuff[Editor::BUFFSIZE];
    strcpy(tagBuff, tag.c_str());
    if (ImGui::InputText("Tag", tagBuff, Editor::BUFFSIZE)) {
        tag = tagBuff;
    }

    // Rendering Layer
    int e_layer = this->layer;
    if (ImGui::InputInt("Layer", &e_layer)) {
        // Can't have non-negative layer number
        if (e_layer < 0) {
            e_layer = 0;
        }

        // Swap Layer
        Rendering::RemoveFromLayer(this);
        this->layer = e_layer;
        Rendering::AddToLayer(this);
    } 

    ImGui::Separator();

    ImGui::BeginTabBar("EntityDetails");
        // Physical Details (Position, size, etc)
        if (ImGui::BeginTabItem("Details")) {
            // Texture
            {
                namespace fs = std::filesystem;

                // Change texture with text input
                char texturePathBuff[Editor::BUFFSIZE];
                std::string relativePath = fs::relative(texturePath.c_str(), GameManager::projectEntryPath).generic_string();
                strcpy(texturePathBuff, relativePath.c_str());
                if (ImGui::InputText("Texture", texturePathBuff, Editor::BUFFSIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    SetTexture(texturePathBuff);
                }

                // Change texture with NFD
                if (ImGui::Button("Select Texture")) {
                    nfdu8char_t* outpath = NULL;
                    nfdopendialogu8args_t args = {0};
                    nfdresult_t result = NFD_OpenDialogU8_With(&outpath, &args);

                    if (result == NFD_OKAY) {
                        SetTexture(outpath);
                    }

                    free(outpath);
                }
            }

            // Change visibility
            ImGui::Checkbox("Visible", &isVisible);

            // Position
            float posBuff[] = {position.x, position.y};
            if (ImGui::InputFloat2("Position", posBuff)) {
                position = {posBuff[0], posBuff[1]};
                UpdateRect();
            }

            // Size
            float sizeBuff[] = {size.x, size.y};
            if (ImGui::InputFloat2("Size", sizeBuff)) {
                size = {sizeBuff[0], sizeBuff[1]};
                UpdateRect();
                this->SetTexture(texturePath);
            }

            // Rotation
            if (ImGui::InputFloat("Rotation", &rotation)) {
                if (rotation >= 360) {
                    rotation -= 360;
                }

                if (rotation <= -360) {
                    rotation += 360;
                }
            }

            // Bounding Box
            ImGui::Checkbox("Show Bounding Box", &showBoundingBox);

            ImGui::Separator();

            // Load from prefab
            if (ImGui::Button("Load from prefab")) {
                nfdu8char_t* path = NULL;
                nfdopendialogu8args_t args = {0};
                nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

                if (result == NFD_OKAY) {
                    std::ifstream infile(path);
                    nlohmann::json data = nlohmann::json::parse(infile);
                    LoadEntityPrefab(data);
                    GameManager::ConsoleMessage("Successfully Loaded Prefab");
                }
            }

            ImGui::SameLine();

            // Save to prefab
            if (ImGui::Button("Save as prefab")) {
                nfdu8char_t* path = NULL;
                nfdopendialogu8args_t args = {0};
                nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

                if (result == NFD_OKAY) {
                    std::ofstream outfile(path);
                    nlohmann::json json;

                    SaveEntityPrefab(json);
                    outfile << std::setw(4) << json;

                    GameManager::ConsoleMessage("Successfully Saved Prefab");
                }
            }

            // Delete
            if (ImGui::Button("Delete")) {
                delete(this);
            }

            ImGui::EndTabItem();
        }

        // Physics Information (Velocity, etc)
        if (ImGui::BeginTabItem("Physics")) {
            // Solid object
            ImGui::Checkbox("Solid", &solid);

            // If solid
            if (solid) {
                // Physics Object
                ImGui::SameLine();
                ImGui::Checkbox("Physics Object", &physicsObject);
            }

            if (physicsObject) {
                ImGui::Separator();
                
                // Velocity
                float veloBuff[] = {velocity.x, velocity.y};
                if (ImGui::InputFloat2("Velocity", veloBuff)) {
                    velocity = {veloBuff[0], veloBuff[1]};
                }

                // Grounded (Can't be set manually)
                ImGui::BeginDisabled();
                    ImGui::Checkbox("Grounded", &grounded);
                ImGui::EndDisabled();
            }
            ImGui::EndTabItem();
        }

        // Script information
        if (ImGui::BeginTabItem("Scripting")) { 
            namespace fs = std::filesystem;
            auto treeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            
            // Scripts
            ImGui::Text("Scripts");
            for (size_t i = 0; i < lua_scripts.size(); i++) {
                std::string script = lua_scripts[i];
                std::string relativePath = fs::relative(script, GameManager::projectEntryPath).generic_string();

                // Script Name
                ImGui::TreeNodeEx(relativePath.c_str(), treeFlags);
                if (ImGui::IsItemClicked()) {
                    // TODO show code 
                }

                // Delete script button
                ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                ImGui::PushID(script.c_str());
                if (ImGui::Button("X")) {
                    lua_scripts.erase(lua_scripts.begin() + i);
                }
                ImGui::PopID();
            }

            // Add script
            if (ImGui::Button("Add Lua Script")) {
                nfdu8char_t* scriptPath = NULL;
                nfdopendialogu8args_t args = {0};
                nfdresult_t result = NFD_OpenDialogU8_With(&scriptPath, &args);

                if (result == NFD_OKAY) {
                    lua_scripts.push_back(scriptPath);
                }

                free(scriptPath);
            }

            ImGui::Separator();

            // Display Entity Variables
            ImGui::Text("Variables");
            auto it = variables.begin();
            while(it != variables.end()) {
                EntityVar& var = it->second;
                const char* varName = it->first.c_str(); 

                if (var.type == NUMBER) {
                    ImGui::InputDouble(varName, &var.numval, 1.0, 1.0, "%.3f");

                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    ImGui::PushID(varName);
                    if (ImGui::Button("X")) {
                        it = variables.erase(it);
                        ImGui::PopID();
                        continue;
                    }
                    ImGui::PopID();
                }
                else if (var.type == STRING) {
                    char textBuff[512];
                    strcpy(textBuff, var.stringval.data());
                    if (ImGui::InputText(varName, textBuff, 512)) {
                        var.stringval = textBuff;
                    }

                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    ImGui::PushID(varName);
                    if (ImGui::Button("X")) {
                        it = variables.erase(it);
                        ImGui::PopID();
                        continue;
                    }
                    ImGui::PopID();
                }
                else if (var.type == BOOLEAN) {
                    ImGui::Checkbox(varName, &var.boolval);

                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    ImGui::PushID(varName);
                    if (ImGui::Button("X")) {
                        it = variables.erase(it);
                        ImGui::PopID();
                        continue;
                    }
                    ImGui::PopID();
                }

                ++it;
            }

            // Create Entity Variable
            if (ImGui::Button("Add Variable")) {
                ImGui::OpenPopup("Add Variable");
            }

            auto popupFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
            if (ImGui::BeginPopupModal("Add Variable", NULL, popupFlags)) {
                // Window Settings
                ImVec2 wSize = ImGui::GetWindowSize();
                ImGui::SetWindowPos({
                    GameManager::screenWidth/2 - wSize.x/2,
                    GameManager::screenHeight/2 - wSize.y/2
                });
                
                // Variable Name
                ImGui::InputText("Name", Editor::addVariableName, Editor::BUFFSIZE);

                // Variable type
                ImGui::RadioButton("Number", &Editor::addVariableType, 0);
                ImGui::SameLine();
                ImGui::RadioButton("String", &Editor::addVariableType, 1);
                ImGui::SameLine();
                ImGui::RadioButton("Boolean", &Editor::addVariableType, 2);

                // Variable value
                if (Editor::addVariableType == 0) {
                    ImGui::InputDouble("Value", &Editor::addVariableNumberVal, 1.0f);
                }
                else if (Editor::addVariableType == 1) {
                    ImGui::InputText("Value", Editor::addVariableStringVal, Editor::BUFFSIZE); 
                }
                else if (Editor::addVariableType == 2) {
                    ImGui::RadioButton("False", &Editor::addVariableBoolVal, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("True", &Editor::addVariableBoolVal, 1);
                }

                // Add Variable
                if (ImGui::Button("Add")) {
                    EntityVar var;
                    std::string name = Editor::addVariableName;
                    if (Editor::addVariableType == 0) {
                        var.type = NUMBER;
                        var.numval = Editor::addVariableNumberVal;
                    }
                    else if (Editor::addVariableType == 1) {
                        var.type = STRING;
                        var.stringval = Editor::addVariableStringVal;
                    }
                    else if (Editor::addVariableType == 2) {
                        var.type = BOOLEAN;
                        var.boolval = Editor::addVariableBoolVal;
                    }

                    variables[name] = var;

                    // Reset buffers
                    *Editor::addVariableName = {0};
                    Editor::addVariableType = 0;
                    Editor::addVariableNumberVal = 0;
                    *Editor::addVariableStringVal = {0};
                    Editor::addVariableBoolVal = 0;

                    ImGui::CloseCurrentPopup();
                }

                // Close window
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::EndTabItem();
        }

    ImGui::EndTabBar();
}
