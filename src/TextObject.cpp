#include "nfd.h"

#include "TextObject.h"
#include "GameManager.h"
#include "Editor.h"
#include "Rendering.h"

TextObject::TextObject() : GameObject() {
    text = "";
    font = GameManager::defaultFont;
    fontSize = 32;
    charSpacing = 8;
    color = BLACK;
}

TextObject::~TextObject() {
    // Nothing right now    
}

/**
 * @brief Set the font of the TextObject
 * 
 * @param path path to the .ttf file
 */
void TextObject::SetFont(std::string path) {
    font = {
        Rendering::b_LoadFont(path),
        path
    };
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

    ImGui::Separator();

    // Font
    {
        namespace fs = std::filesystem;

        // Change font with text input
        char fontPathBuff[Editor::BUFFSIZE];
        std::string relativePath = fs::relative(font.path).generic_string();
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
    ImGui::InputInt("Size", &fontSize);

    // Character Spacing
    ImGui::InputInt("Spacing", &charSpacing);

    // Color
    float colorBuff[] = {color.r, color.g, color.b, color.a};
    if (ImGui::ColorEdit4("Color", colorBuff)) {
        color.r = colorBuff[0];
        color.g = colorBuff[1];
        color.b = colorBuff[2];
        color.a = colorBuff[3];
    }
}