#pragma once

#include "GameObject.h"

class GameCamera : public GameObject {
    public:
        Camera2D camera;
        bool isActive;

        GameCamera();
        GameCamera(const GameCamera* cam);
        ~GameCamera();

        void MoveCamera(Vector2 change);
        void SetPosition(Vector2 pos);
        void CalculateSize(Vector2 windowSize);
        
        void SetActive();

        void SaveCameraJson(nlohmann::json& data);
        void LoadCameraJson(nlohmann::json& data);
        void SaveCameraPrefab(nlohmann::json& data);
        void LoadCameraPrefab(nlohmann::json& data);
        
        void DrawPropertiesUI();
};
