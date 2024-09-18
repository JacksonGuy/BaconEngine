#include "Camera.h"
#include "GameManager.h"
#include "Rendering.h"
#include "Editor.h"

GameCamera::GameCamera() : GameObject() {
    type = CAMERA;
    
    camera = {0};
    camera.target = {0,0}; 
    camera.offset = {0,0};
    camera.zoom = 1.f;
    camera.rotation = 0.0f;
    isActive = false;

    GameManager::GameCameras.push_back(this);
}

/**
 * @brief Moves the camera by some delta vector
 * 
 * @param change the amount to move the camera by
 */
void GameCamera::MoveCamera(Vector2 change) {
    this->position.x += change.x;
    this->position.y += change.y;
    this->camera.target = this->position;
}

/**
 * @brief Writes Camera details to JSON object
 * 
 * @param data the JSON object to save to
 */
void GameCamera::SaveCameraJson(nlohmann::json& data) {
    SaveGameObjectJson(data);

    data["isActive"] = isActive;
}

/**
 * @brief Loads Camera details from JSON object
 * 
 * @param data the JSON object to load from
 */
void GameCamera::LoadCameraJson(nlohmann::json& data) {
    LoadGameObjectJson(data);

    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        if (key == "isActive") {
            isActive = value;

            if (isActive) {
                GameManager::activeCameraTracker = this;
            }
        }
    }
}

/**
 * @brief ImGui UI which shows details about the GameCamera
 * 
 */
void GameCamera::DrawPropertiesUI() {
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

    // Position
    float posBuff[] = {position.x, position.y};
    if (ImGui::InputFloat2("Position", posBuff)) {
        position = {posBuff[0], posBuff[1]};
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

    // Visible
    ImGui::Checkbox("Visible", &isVisible);

    ImGui::Separator();

    // Is Active
    if (GameManager::activeCameraTracker != nullptr && GameManager::activeCameraTracker != this) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Checkbox("Active Camera", &isActive)) {
        if (isActive == false) {
            GameManager::current_camera = nullptr;
        }
        else {
            GameManager::current_camera = this;
        }
    }

    if (GameManager::activeCameraTracker != nullptr && GameManager::activeCameraTracker != this) {
        ImGui::EndDisabled();
    }
}