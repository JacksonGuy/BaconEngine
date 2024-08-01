#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include "GameObject.hpp"

class Camera : public GameObject {
    public:
        sf::View* view;
        bool isActive;

        Camera();
        Camera(Camera& camera);
        ~Camera();
        void Overwrite(Camera& camera);
        void SetPosition(sf::Vector2f position) override;
};

#endif