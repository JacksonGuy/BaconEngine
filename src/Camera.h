#pragma once

#include "GameObject.h"

class GameCamera : public GameObject {
    public:
        Camera2D camera;
        bool isActive;

        GameCamera();
        void MoveCamera(Vector2 change);
        void SaveCameraJson(nlohmann::json& data);
        void LoadCameraJson(nlohmann::json& data);
        void DrawPropertiesUI();
};
