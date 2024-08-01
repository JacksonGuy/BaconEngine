#include "Camera.hpp"
#include "GameManager.hpp"

Camera::Camera() : GameObject() {
    this->name = "Camera";
    this->type = CAMERA;

    this->view = new sf::View();
    this->view->setCenter(this->position);
    this->view->setSize(sf::Vector2f(this->width, this->height));
    this->isActive = false;

    GameManager::Cameras.push_back(this);
}

Camera::Camera(Camera& camera) : GameObject(camera) { 
    this->view = new sf::View();
    this->view->setCenter(camera.view->getCenter());
    this->view->setSize(camera.view->getSize());

    this->isActive = camera.isActive;
}

Camera::~Camera() {
    delete this->view;

    // Remove from cameras
    for (size_t i = 0; i < GameManager::Cameras.size(); i++) {
        if (this->ID == GameManager::Cameras[i]->ID) {
            GameManager::Cameras.erase(GameManager::Cameras.begin() + i);
            break;
        }
    }
}

/**
 * @brief Copies the variables of the given Camera
 * 
 * @param camera the Camera to copy
 */
void Camera::Overwrite(Camera& camera) {
    GameObject::Overwrite(camera);
    this->name = camera.name;

    this->view->setCenter(camera.view->getCenter());
    this->view->setSize(camera.view->getSize());
}

/**
 * @brief Sets the position of the Camera and children objects
 * 
 * @param position 
 */
void Camera::SetPosition(sf::Vector2f position) {
    sf::Vector2f delta = position - this->position;

    this->position = position;
    this->view->setCenter(this->position);

    this->UpdateChildrenPositions(delta);
}