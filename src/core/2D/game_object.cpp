#include "game_object.h"

#include "imgui.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
GameObject::GameObject()
{
    this->object_type = ObjectType::OBJECT;
    this->name = "Object";
    this->tag = "Default";
    this->position = {0.f, 0.f};
    this->size = {1.f, 1.f};
    this->rotation = 0.f;
    this->is_visible = true;
    this->layer = 0;

    this->parent = nullptr;
}

GameObject::GameObject(uint8_t* bytes) { this->deserialize(bytes); }

const GameObject* GameObject::get_parent() const { return this->parent; }

const std::vector<GameObject*>& GameObject::get_children() const
{
    return this->children;
}

const size_t GameObject::get_layer() const { return this->layer; }

void GameObject::draw_properties_editor()
{
    // Name
    char name_buf[ui::_BUF_SIZE];
    strcpy(name_buf, this->name.c_str());
    ImGui::ItemLabel("Name", ItemLabelFlag::Left);
    if (ImGui::InputText("##Name", name_buf, ui::_BUF_SIZE))
    {
        this->name = std::string(name_buf);

        ui::project_was_modified = true;
    }

    // Tag
    char tag_buf[ui::_BUF_SIZE];
    strcpy(tag_buf, this->tag.c_str());
    ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
    if (ImGui::InputText("##Tag", tag_buf, ui::_BUF_SIZE))
    {
        this->tag = std::string(tag_buf);

        ui::project_was_modified = true;
    }

    // Rendering layer
    int render_layer = this->layer;
    ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
    if (ImGui::InputInt("##layer", &render_layer))
    {
        if (render_layer < 0)
        {
            render_layer = 0;
        }

        this->manager->set_object_layer(this, render_layer);

        ui::project_was_modified = true;
    }
}

void GameObject::save_to_json(nlohmann::json& data) const
{
    data["uuid"] = uuid.get_uuid();
    data["type"] = object_type;
    data["name"] = name;
    data["tag"] = tag;
    data["position"] = {position.x, position.y};
    data["size"] = {size.x, size.y};
    data["rotation"] = rotation;
    data["is_visible"] = is_visible;
    data["layer"] = layer;

    if (parent != nullptr)
    {
        data["parent"] = parent->uuid.get_uuid();
    }
    else
    {
        data["parent"] = "null";
    }
}

void GameObject::load_from_json(nlohmann::json& data)
{
    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
    {
        std::string key = it.key();
        auto value = *it;

        if (key == "uuid")
        {
            this->uuid = UUID(value);
        }
        else if (key == "type")
        {
            this->object_type = ObjectType(value);
        }
        else if (key == "name")
        {
            this->name = value;
        }
        else if (key == "tag")
        {
            this->tag = value;
        }
        else if (key == "position")
        {
            this->position = (Vector2){value[0], value[1]};
        }
        else if (key == "size")
        {
            this->size = (Vector2){value[0], value[1]};
        }
        else if (key == "rotation")
        {
            this->rotation = value;
        }
        else if (key == "is_visible")
        {
            this->is_visible = value;
        }
        else if (key == "layer")
        {
            this->layer = value;
            // this->manager->set_object_layer(this, this->layer);
        }
        else if (key == "parent")
        {
            if (value == "null")
            {
                this->parent = nullptr;
            }
            else
            {
                this->parent = this->manager->find_object_by_uuid(value);
            }
        }
    }
}

size_t GameObject::calculate_size() const
{
    debug_error("default calculate_size has not been implemented yet.");
    return 0;
}

uint8_t* GameObject::serialize() const
{
    debug_error("default serialize has not been implemented yet.");
    return nullptr;
}

void GameObject::deserialize(uint8_t* bytes)
{
    debug_error("default deserialize has not been implemented yet.");
}
} // namespace bacon
