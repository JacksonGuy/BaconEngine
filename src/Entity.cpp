#include "Entity.h"
#include "Rendering.h"
#include "GameManager.h"

Entity::Entity() {
    texture = nullptr;
    texturePath = "";
    bodytype = STATIC;
}

Entity::~Entity() {
    // Remove from Entities list
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        if (GameManager::Entities[i]->ID == ID) {
            GameManager::Entities.erase(GameManager::Entities.begin() + i);
            break;
        }
    }

    // Unload Texture
    if (texture != nullptr) {
        UnloadTexture(*texture);
        free(texture);
    }
}

void Entity::SetTexture(std::string path) {
    if (texture != nullptr) {
        UnloadTexture(*texture);
        
        // Not sure this actually does anything useful once UnloadTexture is called
        free(texture);
        texture = nullptr;
    }

    Image image = LoadImage(path.c_str());
    ImageResize(&image, size.x, size.y);
    *texture = LoadTextureFromImage(image);
    UnloadImage(image);
    texturePath = path;
}

void Entity::CreateBody() {
    // Remove current body from world
    b2DestroyBody(body);

    b2BodyDef bodydef = b2DefaultBodyDef();
    switch(m_bodytype) {
        case STATIC:
            bodydef.type = b2_staticBody;
            break;
        case DYNAMIC:
            bodydef.type = b2_dynamicBody;
            break;
    }
    bodyDef.position = (b2Vec2){position.x, position.y};
 
    body = b2CreateBody(GameManager::world, &bodyDef);    

    b2Polygon boxshape = b2MakeBox(size.x/2, size.y/2);
    b2ShapeDef boxdef = b2DefaultShapeDef();
    boxdef.density = 1.0f;
    boxdef.friction = 0.3f;
    b2CreatePolygonShape(body, &boxdef, &boxshape);
}
