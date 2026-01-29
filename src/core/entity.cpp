#include "entity.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/types.h"
#include "raylib.h"

#include "util.h"

namespace bacon {
    Entity::Entity(uid_t uid) : GameObject(uid) {
        this->m_texture = {0};
        this->m_texture_path = "";
        this->physics_body = {0};
        this->body_type = body_t::NONE;
    }

    void Entity::set_texture(const char* path) {
        debug_error("This function has not been implemented yet.");
    }

    void Entity::draw(b2WorldId world_id) const {
        b2Vec2 extent = {
            -this->size.x * 0.5f,
            -this->size.y * 0.5f
        };
        b2Vec2 b_pos = b2Body_GetWorldPoint(this->physics_body, extent);
        b2Rot b_rot = b2Body_GetRotation(this->physics_body);
        float b_radians = b2Rot_GetAngle(b_rot);

        // Vector2 pos = {b_pos.x, b_pos.y};

        // DrawTextureEx(
        //     this->m_texture,
        //     pos,
        //     RAD2DEG * b_radians,
        //     1.f,
        //     WHITE
        // );

        DrawRectanglePro(
            {b_pos.x, b_pos.y, this->size.x, this->size.y},
            {this->size.x/2, this->size.y/2},
            RAD2DEG * b_radians,
            RED
        );
    }

    void Entity::create_body(b2WorldId world_id) {
        b2BodyDef body_def = b2DefaultBodyDef();
        body_def.position = (b2Vec2){
            this->position.x + (this->size.x / 2),
            this->position.y + (this->size.y / 2)
        };
        b2Polygon box = b2MakeBox(this->size.x/2, this->size.y/2);

        switch (this->body_type) {
            case DYNAMIC:
            {
                body_def.type = b2_dynamicBody;
                break;
            }

            case KINEMATIC:
            {
                body_def.type = b2_kinematicBody;
                break;
            }

            case STATIC:
            case NONE:
            default:
            {
                break;
            }
        }

        this->physics_body = b2CreateBody(world_id, &body_def);
        b2ShapeDef shape = b2DefaultShapeDef();
        b2CreatePolygonShape(this->physics_body, &shape, &box);
    }

    void Entity::save_to_json() const {
        debug_error("This function has not been implemented yet.");
    }

    void Entity::load_from_json() {
        debug_error("This function has not been implemented yet.");
    }

    size_t Entity::calculate_size() const {
        debug_error("This function has not been implemented yet.");
        return 0;
    }

    uint8_t* Entity::serialize() const {
        debug_error("This function has not been implemented yet.");
        return nullptr;
    }

    void Entity::deserialize(uint8_t* bytes) {
        debug_error("This function has not been implemented yet.");
    }
}
