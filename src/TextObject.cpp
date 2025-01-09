#include <iostream>

#include "nfd.h"

#include "TextObject.h"
#include "GameManager.h"
#include "Editor.h"
#include "Rendering.h"

TextObject::TextObject() : GameObject() {
    type = TEXT;

    text = "";
    font = GameManager::defaultFont;
    fontSize = 32;
    charSpacing = 0;
    color = BLACK;

    GameManager::TextObjects.push_back(this);
}

TextObject::TextObject(const TextObject* textobj) : GameObject(textobj) {
    type = TEXT;

    text = textobj->text;
    font = textobj->font;
    fontSize = textobj->fontSize;
    charSpacing = textobj->charSpacing;
    color = textobj->color;

    GameManager::TextObjects.push_back(this);
}

TextObject::~TextObject() {
    // Remove from TextObjects
    for (size_t i = 0; i < GameManager::TextObjects.size(); i++) {
        if (GameManager::TextObjects[i]->ID == ID) {
            GameManager::TextObjects.erase(GameManager::TextObjects.begin() + i);
            break;
        }
    }
}

/**
 * @brief Set the font of the TextObject
 * 
 * @param path path to the .ttf file
 */
void TextObject::SetFont(std::string path) {
    if (path == "") return;

    font = Rendering::b_LoadFont(path);
    fontPath = path;
}

/**
 * @brief Finds the size of the GameObject from text
 * and fontSize 
 * 
 */
void TextObject::CalculateSize() {
    size = MeasureTextEx(font, text.c_str(), fontSize, charSpacing);
}

/**
 * @brief Write TextObject data to the JSON object
 * 
 * @param data JSON object to write to 
 */
void TextObject::SaveTextObjectJson(nlohmann::json& data) {
    SaveGameObjectJson(data);

    data["text"] = text;
    data["font"] = std::filesystem::relative(fontPath, GameManager::projectEntryPath).generic_string();
    data["fontSize"] = fontSize;
    data["charSpacing"] = charSpacing;
    data["color"] = {
        color.r,
        color.g,
        color.b,
        color.a
    };
}

/**
 * @brief Loads TextObject data from JSON object
 * 
 * @param data the JSON object to load from
 */
void TextObject::LoadTextObjectJson(nlohmann::json& data) {
    LoadGameObjectJson(data);

    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        if (key == "text") {
            text = value;
        }
        else if (key == "font") {
            if (value == "") {
                font = GameManager::defaultFont;
                continue;
            }

            std::string absPath = GameManager::projectEntryPath + "/" + std::string(value);
            font = Rendering::b_LoadFont(absPath);
            fontPath = absPath;
        }
        else if (key == "fontSize") {
            fontSize = value;
        }
        else if (key == "charSpacing") {
            charSpacing = value;
        }
        else if (key == "color") {
            color = {
                value[0],
                value[1],
                value[2],
                value[3]
            };
        }
    }
    CalculateSize();
}


/**
 * @brief Saves the TextObject data to a JSON prefab file
 * 
 * @param data the file to save to
 */
void TextObject::SaveTextObjectPrefab(nlohmann::json& data) {
    SaveGameObjectPrefab(data);

    data["text"] = text;
    data["font"] = std::filesystem::relative(fontPath, GameManager::projectEntryPath).generic_string();
    data["fontSize"] = fontSize;
    data["charSpacing"] = charSpacing;
    data["color"] = {
        color.r,
        color.g,
        color.b,
        color.a
    };
}

/**
 * @brief Loads TextObject data from JSON prefab file
 * 
 * @param data the file to load from
 */
void TextObject::LoadTextObjectPrefab(nlohmann::json& data) {
    LoadGameObjectPrefab(data);

    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        if (key == "text") {
            text = value;
        }
        else if (key == "font") {
            if (value == "") {
                font = GameManager::defaultFont;
                continue;
            }

            std::string absPath = GameManager::projectEntryPath + "/" + std::string(value);
            font = Rendering::b_LoadFont(absPath);
            fontPath = absPath;
        }
        else if (key == "fontSize") {
            fontSize = value;
        }
        else if (key == "charSpacing") {
            charSpacing = value;
        }
        else if (key == "color") {
            color = {
                value[0],
                value[1],
                value[2],
                value[3]
            };
        }
    }
    CalculateSize();
}

/**
 * @brief ImGui UI which shows details about the TextObject
 * 
 */
void TextObject::DrawPropertiesUI() {
    // ID
    std::string idText = "ID: " + std::to_string(ID);
    ImGui::Text(idText.c_str());

    // Name
    char nameBuff[Editor::BUFFSIZE];
    strcpy(nameBuff, name.c_str());
    if (ImGui::InputText("Name", nameBuff, Editor::BUFFSIZE)) {
        name = nameBuff;
    }

    // Tag
    char tagBuff[Editor::BUFFSIZE];
    strcpy(tagBuff, tag.c_str());
    if (ImGui::InputText("Tag", tagBuff, Editor::BUFFSIZE)) {
        tag = tagBuff;
    }

    // Rendering Layer
    int e_layer = this->layer;
    if (ImGui::InputInt("Layer", &e_layer)) {
        // Can't have non-negative layer number
        if (e_layer < 0) {
            e_layer = 0;
        }

        // Swap Layer
        Rendering::RemoveFromLayer(this);
        this->layer = e_layer;
        Rendering::AddToLayer(this);
    }

    ImGui::Separator();

    // Position
    float posBuff[] = {position.x, position.y};
    if (ImGui::InputFloat2("Position", posBuff)) {
        position = {posBuff[0], posBuff[1]};
    }

    // Rotation
    if (ImGui::InputFloat("Rotation", &rotation)) {
        if (rotation >= 360) {
            rotation -= 360;
        }

        if (rotation <= -360) {
            rotation += 360;
        }
    }

    // Visible
    ImGui::Checkbox("Visible", &isVisible);

    // Bounding Box
    ImGui::Checkbox("Show Bounding Box", &showBoundingBox);

    ImGui::Separator();

    // Font
    {
        namespace fs = std::filesystem;

        // Change font with text input
        char fontPathBuff[Editor::BUFFSIZE];
        std::string relativePath = fs::relative(fontPath).generic_string();
        strcpy(fontPathBuff, relativePath.c_str());
        if (ImGui::InputText("Font", fontPathBuff, Editor::BUFFSIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
            SetFont(fontPathBuff);
        }

        // Change font with NFD
        if (ImGui::Button("Select Font")) {
            nfdu8char_t* outpath = NULL;
            nfdopendialogu8args_t args = {0};
            nfdresult_t result = NFD_OpenDialogU8_With(&outpath, &args);

            if (result == NFD_OKAY) {
                SetFont(outpath);
            }

            free(outpath);
        }
    }

    // Font Size
    if (ImGui::InputInt("Size", &fontSize)) {
        CalculateSize();
    }

    // Character Spacing
    if (ImGui::InputInt("Spacing", &charSpacing)) {
        CalculateSize();
    }

    // Color
    // We have to divide/multiply by 255 because of weird ImGui quirk
    float colorBuff[] = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
    if (ImGui::ColorEdit4("Color", colorBuff)) {
        color.r = colorBuff[0] * 255;
        color.g = colorBuff[1] * 255;
        color.b = colorBuff[2] * 255;
        color.a = colorBuff[3] * 255;
    }

    // Text Edit
    char textBuff[1024 * 16];
    strcpy(textBuff, text.c_str());
    if (ImGui::InputTextMultiline("Text", textBuff, 1024 * 16)) {
        text = textBuff;
        CalculateSize();
    }

    // Delete button
    if (ImGui::Button("Delete")) {
        delete(this);
    }
}
