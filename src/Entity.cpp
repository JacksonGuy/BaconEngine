#include <iostream>

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

    showHitbox = false;

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

    UnloadTexture(texture);
}

/**
 * @brief Changes the entity's texture
 * 
 * @param path 
 */
void Entity::SetTexture(std::string path) {
    Image image = LoadImage(path.c_str());

    // Check if image was successfully loaded
    if (image.data == NULL) {
        GameManager::ConsoleError("Failed to load image: " + path);
        return;
    }

    ImageResize(&image, size.x, size.y);
    texture = LoadTextureFromImage(image);
    texturePath = path;

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
    for (size_t i = 0; i < variables.size(); i++) {
        EntityVar var = variables[i];
        data["variables"][i] = {
            {"name", var.name},
            {"type", var.type},
            {"stringVal", var.stringval},
            {"numvVal", var.numval}
        };
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
                var.name = data["variables"][i][0];
                var.type = EntityVar_t(data["variables"][i][1]);
                var.stringval = data["variables"][i][2];
                var.numval = data["variables"][i][3];
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
                LoadTexture(texturePath.c_str());
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

            ImGui::Separator();

            // Load from prefab
            if (ImGui::Button("Load from prefab")) {
                // TODO
            }

            ImGui::SameLine();

            // Save to prefab
            if (ImGui::Button("Save as prefab")) {
                // TODO
            }

            // Delete
            if (ImGui::Button("Delete")) {
                if (Editor::viewPropertiesObject == this) {
                    Editor::viewPropertiesObject = nullptr;
                }
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

                // Show hitbox (rect)
                ImGui::Checkbox("Show hitbox", &showHitbox);
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
            for (size_t i = 0; i < variables.size(); i++) {
                EntityVar var = variables[i];

                if (var.type == NUMBER) {
                    ImGui::InputDouble(var.name.data(), &variables[i].numval);

                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    ImGui::PushID(var.name.data());
                    if (ImGui::Button("X")) {
                        variables.erase(variables.begin() + i);
                    }
                    ImGui::PopID();
                }
                else if (var.type == STRING) {
                    char textBuff[512];
                    strcpy(textBuff, variables[i].stringval.data());
                    ImGui::InputText(var.name.data(), textBuff, 512);

                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    ImGui::PushID(var.name.data());
                    if (ImGui::Button("X")) {
                        variables.erase(variables.begin() + i);
                    }
                    ImGui::PopID();
                }
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

                // Variable value
                if (Editor::addVariableType == 0) {
                    ImGui::InputDouble("Value", &Editor::addVariableNumberVal, 1.0f);
                }
                else if (Editor::addVariableType == 1) {
                    ImGui::InputText("Value", Editor::addVariableStringVal, Editor::BUFFSIZE); 
                }

                // Add Variable
                if (ImGui::Button("Add")) {
                    EntityVar var;
                    var.name = Editor::addVariableName;
                    if (Editor::addVariableType == 0) {
                        var.type = NUMBER;
                        var.numval = Editor::addVariableNumberVal;
                    }
                    else if (Editor::addVariableType == 1) {
                        var.type = STRING;
                        var.stringval = Editor::addVariableStringVal;
                    }

                    variables.push_back(var);

                    // Reset buffers
                    *Editor::addVariableName = {0};
                    Editor::addVariableType = 0;
                    Editor::addVariableNumberVal = 0;
                    *Editor::addVariableStringVal = {0};

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
