#include "game_object.h"

#include "imgui.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon {
    GameObject::GameObject(uid_t uid) {
        this->class_type = object_t::OBJECT;
        this->name = "Object (" + std::to_string(uid) + ")";
        this->tag = "Default";
        this->position = {0.f, 0.f};
        this->size = {1.f, 1.f};
        this->rotation = 0.f;
        this->is_visible = true;
        this->layer = 0;

        this->m_uid = uid;
        this->parent = nullptr;
    }

    GameObject::GameObject(uint8_t* bytes) {
        this->deserialize(bytes);
    }

    uid_t GameObject::get_uid() const {
        return this->m_uid;
    }

    GameObject* GameObject::get_parent() const {
        return this->parent;
    }

    const std::vector<GameObject*>& GameObject::get_children() const {
        return this->children;
    }

    void GameObject::draw_properties_editor() {
        // ID
        std::string id_text = "ID: " + std::to_string(this->get_uid());
        ImGui::Text("%s", id_text.c_str());

        // Name
        char name_buf[ui::_BUF_SIZE];
        strcpy(name_buf, this->name.c_str());
        ImGui::ItemLabel("Name", ItemLabelFlag::Left);
        if (ImGui::InputText("##Name", name_buf, ui::_BUF_SIZE)) {
            this->name = std::string(name_buf);
        }

        // Tag
        char tag_buf[ui::_BUF_SIZE];
        strcpy(tag_buf, this->tag.c_str());
        ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
        if (ImGui::InputText("##Tag", tag_buf, ui::_BUF_SIZE)) {
            this->tag = std::string(tag_buf);
        }

        // Rendering layer
        int render_layer = this->layer;
        ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
        if (ImGui::InputInt("##layer", &render_layer)) {
            if (render_layer < 0) {
                render_layer = 0;
            }

            this->manager->set_object_layer(this, render_layer);
        }

        // Position
        float position[] = {this->position.x, this->position.y};
        ImGui::ItemLabel("Position", ItemLabelFlag::Left);
        if (ImGui::InputFloat2("##position", position)) {
            this->position = (Vector2){position[0], position[1]};
        }
    }

    void GameObject::save_to_json(nlohmann::json& data) const {
        debug_error("default save_to_json has not been implemented yet.");
    }

    void GameObject::load_from_json(nlohmann::json& data) {
        debug_error("default load_from_json has not been implemented yet.");
    }

    size_t GameObject::calculate_size() const {
        debug_error("default calculate_size has not been implemented yet.");
        return 0;
    }

    uint8_t* GameObject::serialize() const {
        debug_error("default serialize has not been implemented yet.");
        return nullptr;
    }

    void GameObject::deserialize(uint8_t* bytes) {
        debug_error("default deserialize has not been implemented yet.");
    }
}
