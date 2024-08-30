#include "Camera.h"
#include "GameManager.h"

GameCamera::GameCamera() {
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
    this->camera.offset = this->position;
}