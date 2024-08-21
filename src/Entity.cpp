#include "Entity.h"
#include "Rendering.h"
#include "GameManager.h"

#include <iostream>

Entity::Entity() : GameObject() {
    type = ENTITY;
    
    texture = {0};
    texturePath = "";
    bodytype = STATIC;

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

void Entity::SetTexture(std::string path) {
    UnloadTexture(texture);

    std::cout << "Path: " << path << "\n";
    // Image image = LoadImage(path.c_str());
    Image image = B_LoadImage(path);

    ImageResize(&image, size.x, size.y);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    texturePath = path;
}

void Entity::CreateBody() {
    // Remove current body from world
    b2DestroyBody(body);

    b2BodyDef bodydef = b2DefaultBodyDef();
    switch(bodytype) {
        case STATIC:
            bodydef.type = b2_staticBody;
            break;
        case DYNAMIC:
            bodydef.type = b2_dynamicBody;
            break;
    }
    bodydef.position = {position.x, position.y};
 
    body = b2CreateBody(GameManager::world, &bodydef);    

    b2Polygon boxshape = b2MakeBox(size.x/2, size.y/2);
    b2ShapeDef boxdef = b2DefaultShapeDef();
    boxdef.density = 1.0f;
    boxdef.friction = 0.3f;
    b2CreatePolygonShape(body, &boxdef, &boxshape);
}
