#include "Camera.h"
#include "GameManager.h"

GameCamera::GameCamera() {
    this->camera = {0}; 
    isActive = false;

    GameManager::GameCameras.push_back(this);
}