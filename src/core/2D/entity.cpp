#include "entity.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/types.h"
#include "raylib.h"
#include "imgui.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"
#include <functional>

namespace bacon {
    Entity::Entity() : GameObject() {
        this->class_type = ObjectType::ENTITY;
        this->name = "Entity";
        this->m_texture = {0};
        this->m_texture_path = "";
        this->physics_body = {0};
        this->body_type = BodyType::NONE;
    }

    void Entity::set_texture(const char* path) {
        debug_error("This function has not been implemented yet.");
    }

    void Entity::set_size(float width, float height) {
        this->size = (Vector2){width, height};

        this->set_texture(this->m_texture_path.c_str());
    }

    void Entity::create_body(b2WorldId world_id) {
        b2BodyDef body_def = b2DefaultBodyDef();
        body_def.rotation = b2MakeRot(this->rotation * DEG2RAD);
        body_def.position = (b2Vec2){
            this->position.x + (this->size.x / 2),
            this->position.y + (this->size.y / 2)
        };
        b2Polygon box = b2MakeBox(this->size.x/2, this->size.y/2);

        switch (this->body_type) {
            case STATIC:
            {
                body_def.type = b2_staticBody;
                break;
            }

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

    void Entity::draw() const {
        if (!this->is_visible) return;

        b2Vec2 b_pos = b2Body_GetPosition(this->physics_body);
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
            {this->size.x * 0.5f, this->size.y * 0.5f},
            RAD2DEG * b_radians,
            RED
        );
    }

    void Entity::draw_properties_editor() {
        GameObject::draw_properties_editor();

        // Size
        float size[] = {this->size.x, this->size.y};
        ImGui::ItemLabel("Size", ItemLabelFlag::Left);
        if (ImGui::InputFloat2("##size", size)) {
            this->set_size(size[0], size[1]);
        }

        // Rotation
        ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
        if (ImGui::InputFloat("##rotation", &this->rotation)) {
            this->rotation = b_fmod(this->rotation, 360);
        }

        // Visibility
        ImGui::ItemLabel("Visible", ItemLabelFlag::Left);
        ImGui::Checkbox("##visible", &this->is_visible);

        ImGui::Separator();
    }

    void Entity::save_to_json(nlohmann::json& data) const {
        GameObject::save_to_json(data);

        data["body_type"] = body_type;
        data["texture_path"] = m_texture_path;
    }

    void Entity::load_from_json(nlohmann::json& data) {
        GameObject::load_from_json(data);

        for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
        {
            std::string key = it.key();
            auto value = *it;

            if (key == "body_type")
            {
                this->body_type = BodyType(value);
            }
            else if (key == "texture_path")
            {
                this->m_texture_path = value;

                // This calls set_texture() with m_texture_path
                this->set_size(this->size.x, this->size.y);
            }
        }
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
