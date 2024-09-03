#include "Camera.h"
#include "GameManager.h"
#include "Rendering.h"
#include "Editor.h"

GameCamera::GameCamera() : GameObject() {
    this->camera = {0};
    this->camera.target = {0,0}; 
    this->camera.offset = {0,0};
    this->camera.zoom = 1.f;
    this->camera.rotation = 0.0f;
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
}