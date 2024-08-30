#pragma once

#include "GameObject.h"

class GameCamera : public GameObject {
    public:
        Camera2D camera;
        bool isActive;

        GameCamera();
        void MoveCamera(Vector2 change);
};
