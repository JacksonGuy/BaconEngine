#include <iostream>
#include <ctime>

#include <stdlib.h>
#include <crtdbg.h>

#include "raylib.h"
#include "box2d/box2d.h"
#include "imgui.h"
#include "rlImGui.h"
#include <nfd.h>

#include "Editor.h"
#include "GameManager.h"
#include "Rendering.h"
#include "File.h"
#include "LuaAPI.h"
#include "Input.h"

// #define _CRTDBG_MAP_ALLOC
// #define SOL_ALL_SAFETIES_ON 1 

namespace Editor {
    // Project Details
    std::string projectTitle = "Bacon - Untitled Project";
    bool loadedProject = false;

    // Physics
    f32 lastFixedUpdate = 0;
    f32 fixedUpdateRate = 1.0f / 60.0f;

    // Scene Window
    bool sceneWindowFocused = false;
    Vector2 sceneWindowPosition = {0,0};
    Vector2 sceneWindowSize = {0,0};
    bool sceneWindowMouseCapture = false;

    // Editor Camera
    GameCamera* camera = nullptr;
    bool ToggleCameraMove = false;

    // Input
    Vector2 LastFrameMousePos = {0,0};

    // ImGui Toggles
    bool showEditorMessages = true;
    bool showCreateEntityMenu = false;
    bool showCreateTextMenu = false;
    bool showCreateCameraMenu = false;

    // ImGui Variables
    GameObject* viewPropertiesObject = nullptr;

    // Create GameObject buffers
    char createNameBuff[BUFFSIZE];
    float createPositionBuff[2] = {0,0};
    char createTexturePathBuff[BUFFSIZE];
    int createSizeBuff[2] = {64, 64};
    char createTextBuff[TEXTBUFFSIZE];

    // External UI Variables
    char addVariableName[BUFFSIZE] = {0};
    i32 addVariableType = 0;
    f64 addVariableNumberVal = 0;
    char addVariableStringVal[BUFFSIZE] = {0};
};

/**
 * @brief Create and display an ImGui tree from a GameObject and its children
 * 
 * @param obj The base node GameObject
 */
void DisplayEntityTree(GameObject* obj) {
    auto normalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    auto parentFlags =  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
                        | ImGuiTreeNodeFlags_DefaultOpen;

    bool is_open = true;

    if (obj->children.size() == 0) {    
        ImGui::TreeNodeEx(obj->name.c_str(), normalFlags);
    }
    else {
        is_open = ImGui::TreeNodeEx(obj->name.c_str(), parentFlags);
    }

    // Change properties view if right clicked
    if (ImGui::IsItemClicked(1)) {
        Editor::viewPropertiesObject = obj;
    }

    // Handle mouse dragging
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("OBJECT_ID", &obj->ID, sizeof(u32));
        ImGui::EndDragDropSource();
    }

    // Handle mouse drop
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_ID")) {
            // Get Object
            u32 sourceID = *(u32*)payload->Data;
            GameObject* sourceObject = GameManager::FindObjectByID(sourceID);

            if (sourceObject != nullptr) {
                // Remove from parent
                if (sourceObject->parent != nullptr) {
                    // Linear search for itself in the parent's children vector
                    for (size_t i = 0; i < sourceObject->parent->children.size(); i++) {
                        if (sourceObject->parent->children[i]->ID == sourceObject->ID) {
                            sourceObject->parent->children.erase(sourceObject->parent->children.begin() + i);
                            break;
                        }
                    }
                }

                // Add to new parent
                obj->children.push_back(sourceObject);
                sourceObject->parent = obj;
            }
        }
        ImGui::EndDragDropTarget();
    }

    // If children has children and is open, display children
    if (obj->children.size() != 0 && is_open) {
        ImGui::Indent(4);

        for (GameObject* child : obj->children) {
            DisplayEntityTree(child);
        }

        ImGui::TreePop();
        ImGui::Unindent(4);
    }
}

/**
 * @brief Clears Editor's create GameObject buffers
 * 
 */
void ClearEditorBuffers() {
    strcpy(Editor::createNameBuff, "");
    Editor::createPositionBuff[0] = 0, Editor::createPositionBuff[1] = 0;
    strcpy(Editor::createTexturePathBuff, "");
    Editor::createSizeBuff[0] = 64, Editor::createSizeBuff[1] = 64;
    strcpy(Editor::createTextBuff, "");
}


/**
 * @brief Ends engine play test of game.
 * 
 */
void EndGame() {
    GameManager::Reset();

    File::LoadProject(Editor::projectTitle);
    Editor::viewPropertiesObject = nullptr;

    // Set camera back
    GameManager::current_camera = Editor::camera;

    GameManager::ConsoleMessage("Game ended. Editor data restored.");
    GameManager::isPlayingGame = false;
}

/**
 * @brief Draw ImGui windows
 * 
 */
void DrawUI(f32 deltaTime) {
    rlImGuiBegin();
    ImGui::DockSpaceOverViewport();

    i16 fps = GetFPS();

    // Scene Window
    {
        ImGui::Begin("Scene");
            // TODO this is lazy but it works
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
                ImGui::SetWindowFocus();
            }

            // Get scene window details
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            // Set Editor variables 
            Editor::sceneWindowFocused = ImGui::IsWindowFocused();                                    
            Editor::sceneWindowPosition = {windowPos.x, windowPos.y};
            Editor::sceneWindowSize = {windowSize.x - 20, windowSize.y - 40};
            Editor::sceneWindowMouseCapture = ImGui::IsWindowHovered();

            // Check if scene window was resized
            Vector2 frameSize = {Rendering::frame.texture.width, Rendering::frame.texture.height};
            if (frameSize.x != Editor::sceneWindowSize.x || frameSize.y != Editor::sceneWindowSize.y) {

                // Maintain aspect ratio upon resize
                Vector2 ratio = {Editor::sceneWindowSize.x / frameSize.x, Editor::sceneWindowSize.y / frameSize.y};

                // Recreate Frame
                UnloadRenderTexture(Rendering::frame);
                Rendering::frame = LoadRenderTexture(frameSize.x * ratio.x, frameSize.y * ratio.y);
            }  

            // Display RenderTexture
            rlImGuiImageRenderTexture(&Rendering::frame);

        ImGui::End();
    }

    // Top Menu Bar
    {
        ImGui::BeginMainMenuBar(); 
            if (ImGui::BeginMenu("File")) {
                // We don't want the user to save and load project while testing it  
                if (GameManager::isPlayingGame) {
                    ImGui::BeginDisabled();
                }

                if (ImGui::MenuItem("New Project")) {
                    std::string path;
                    
                    if (!Editor::loadedProject) {
                        if (File::CreateNewProject(path, true)) {
                            Editor::projectTitle = path;
                            Editor::loadedProject = true;
                            std::string windowTitle = "Bacon - " + path;
                            SetWindowTitle(windowTitle.c_str());
                        }
                    }
                    else {
                        File::SaveProject(Editor::projectTitle); // Save current project before creating new one
                        if (File::CreateNewProject(path, true)) {
                            Editor::projectTitle = path;
                            Editor::loadedProject = true;
                            std::string windowTitle = "Bacon - " + path;
                            SetWindowTitle(windowTitle.c_str());
                        }
                    }
                }

                if (ImGui::MenuItem("Save")) {
                    if (!Editor::loadedProject) {
                        std::string path;
                        bool result = File::CreateNewProject(path, false);
                        if (!result) {
                            GameManager::ConsoleError("Failed to create new project");
                        }
                        else {
                            Editor::loadedProject = true;
                            Editor::projectTitle = path;
                            File::SaveProject(Editor::projectTitle);
                        }
                    }
                    else {
                        File::SaveProject(Editor::projectTitle);
                    }
                }

                if (ImGui::MenuItem("Save as")) {
                    std::string path;
                    bool result = File::CreateNewProject(path, false);
                    if (!result) {
                        GameManager::ConsoleError("Failed to create new project");
                    }
                    else {
                        Editor::loadedProject = true;
                        Editor::projectTitle = path;
                        File::SaveProject(Editor::projectTitle);
                    }
                }

                if (ImGui::MenuItem("Load")) {
                    nfdu8char_t* path = NULL;
                    nfdopendialogu8args_t args = {0};
                    nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

                    if (result == NFD_OKAY) {
                        if (File::LoadProject(path)) {
                            GameManager::ConsoleMessage("Successfully Loaded Project");
                            Editor::loadedProject = true;
                            Editor::projectTitle = path;
                            SetWindowTitle(std::string("Bacon - " + std::string(path)).c_str());
                        }
                    }
                }

                if (GameManager::isPlayingGame) {
                    ImGui::EndDisabled();
                }

                ImGui::EndMenu();
            }
        
            if (ImGui::BeginMenu("Create")) {
                ImGui::MenuItem("Entity", NULL, &Editor::showCreateEntityMenu);
                ImGui::MenuItem("Text", NULL, &Editor::showCreateTextMenu);
                ImGui::MenuItem("Camera", NULL, &Editor::showCreateCameraMenu);
                
                ImGui::EndMenu();
            }
        ImGui::EndMainMenuBar();
    }

    // General Menu
    {
        ImGui::Begin("Game Details");
            ImGui::Text(GameManager::engineVersion.c_str());
            ImGui::Text(("FPS: " + std::to_string(fps)).c_str());

            // Playtest game button
            if (!GameManager::isPlayingGame) {
                if (ImGui::Button("Play Game")) {
                    File::SaveProject(Editor::projectTitle);

                    // Change Camera
                    if (GameManager::activeCameraTracker != nullptr) {
                        GameManager::current_camera = GameManager::activeCameraTracker;
                    }

                    GameManager::ConsoleMessage("Editor data saved. Starting game...");
                    GameManager::isPlayingGame = true;
                }
            }
            // End playtest button
            else {
                if (ImGui::Button("End Game")) {
                    EndGame();
                }
            }

            // General Menus
            ImGui::Separator();
            ImGui::BeginTabBar("EngineItems");
                if (ImGui::BeginTabItem("Camera")) {
                    
                    // Camera Position
                    float cameraPos[2] = {Editor::camera->camera.target.x, Editor::camera->camera.target.y};
                    if (ImGui::InputFloat2("Position", cameraPos)) {
                        Editor::camera->camera.target = {cameraPos[0], cameraPos[1]};
                    }

                    // Camera zoom
                    ImGui::InputFloat("Zoom", &Editor::camera->camera.zoom);

                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();

        ImGui::End();
    }

    // GameObject view
    {
        ImGui::Begin("Properties");
            if (Editor::viewPropertiesObject != nullptr) {
                if (Editor::viewPropertiesObject->type == ENTITY) {
                    Entity* e = (Entity*)Editor::viewPropertiesObject;
                    e->DrawPropertiesUI();
                }
                else if (Editor::viewPropertiesObject->type == TEXT) {
                    TextObject* text = (TextObject*)Editor::viewPropertiesObject;
                    text->DrawPropertiesUI();
                }
                else if (Editor::viewPropertiesObject->type == CAMERA) {
                    GameCamera* camera = (GameCamera*)Editor::viewPropertiesObject;
                    camera->DrawPropertiesUI();
                }
            }
        ImGui::End();
    }

    // Create Entity
    {
        if (Editor::showCreateEntityMenu) {
            ImGui::Begin("Create Entity", &Editor::showCreateEntityMenu);
                // Name
                ImGui::InputText("Name", Editor::createNameBuff, Editor::BUFFSIZE);
                
                // Position
                ImGui::InputFloat2("Position", Editor::createPositionBuff);

                // Texture
                ImGui::InputText("Texture", Editor::createTexturePathBuff, Editor::BUFFSIZE);
                if (ImGui::Button("Select Texture")) {
                    nfdu8char_t* outpath = NULL;
                    nfdopendialogu8args_t args = {0};
                    nfdresult_t result = NFD_OpenDialogU8_With(&outpath, &args);

                    if (result == NFD_OKAY) {
                        strcpy(Editor::createTexturePathBuff, outpath);
                    }

                    free(outpath);
                }

                // Size
                ImGui::InputInt2("Size", Editor::createSizeBuff);

                // Create Entity
                if (ImGui::Button("Create")) {
                    Entity* entity = new Entity();
                    entity->name = Editor::createNameBuff;
                    entity->position = {Editor::createPositionBuff[0], Editor::createPositionBuff[1]};
                    entity->size = {(float)Editor::createSizeBuff[0], (float)Editor::createSizeBuff[1]};
                    entity->SetTexture(Editor::createTexturePathBuff);
                    Rendering::AddToLayer(entity);

                    ClearEditorBuffers();
                    Editor::showCreateEntityMenu = false;
                }

            ImGui::End();
        }
    }

    // Create TextObject
    {
        if (Editor::showCreateTextMenu) {
            ImGui::Begin("Create Text", &Editor::showCreateTextMenu);
                // Name
                ImGui::InputText("Name", Editor::createNameBuff, Editor::BUFFSIZE);

                // Position
                ImGui::InputFloat2("Position", Editor::createPositionBuff);

                // Text
                ImGui::InputTextMultiline("Text", Editor::createTextBuff, Editor::TEXTBUFFSIZE);

                // Create Text Object
                if (ImGui::Button("Create")) {
                    TextObject* text = new TextObject();
                    text->name = Editor::createNameBuff;
                    text->position = {Editor::createPositionBuff[0], Editor::createPositionBuff[1]};
                    text->text = Editor::createTextBuff;
                    text->CalculateSize();
                    Rendering::AddToLayer(text);

                    ClearEditorBuffers();
                    Editor::showCreateTextMenu = false;
                }

            ImGui::End();
        }
    }

    // Create GameCamera
    {
        if (Editor::showCreateCameraMenu) {
            ImGui::Begin("Create Camera", &Editor::showCreateCameraMenu);
                // Name
                ImGui::InputText("Name", Editor::createNameBuff, Editor::BUFFSIZE);

                // Position
                ImGui::InputFloat2("Position", Editor::createPositionBuff);

                // Create Camera
                if (ImGui::Button("Create")) {
                    GameCamera* camera = new GameCamera();
                    camera->name = Editor::createNameBuff;
                    camera->position = {Editor::createPositionBuff[0], Editor::createPositionBuff[1]};
                    Rendering::AddToLayer(camera);

                    ClearEditorBuffers();
                    Editor::showCreateCameraMenu = false;
                }
            ImGui::End();
        }
    }

    // GameObject tree
    {
        auto normalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        auto parentFlags =  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        
        ImGui::Begin("Objects");
            ImGui::SetNextItemOpen(true);

            // Parent Scene Node
            // TODO change name when multiple scenes are introduced 
            if (ImGui::TreeNodeEx("Scene", parentFlags)) {
                // Handle drag and drop
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_ID")) {
                        // Get Entity
                        u32 sourceID = *(u32*)payload->Data;
                        GameObject* sourceObject = GameManager::FindObjectByID(sourceID);

                        if (sourceObject != nullptr) {
                            // Remove from parent
                            if (sourceObject->parent != nullptr) {
                                // Linear search for itself in the parent's children vector
                                for (size_t i = 0; i < sourceObject->parent->children.size(); i++) {
                                    if (sourceObject->parent->children[i]->ID == sourceObject->ID) {
                                        sourceObject->parent->children.erase(sourceObject->parent->children.begin() + i);
                                        sourceObject->parent = nullptr;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // Display Children
                for (GameObject* obj : GameManager::GameObjects) {
                    // Objects without a parent act as base nodes
                    if (obj->parent == nullptr) {
                        DisplayEntityTree(obj);
                    }
                }

                // I don't remember what this line does honestly
                ImGui::TreePop();
            }
        ImGui::End();
    }

    // Debug Console
    {
        ImGui::Begin("Console");
            if (ImGui::Button("Clear")) {
                GameManager::ConsoleMessages.clear();
                // GameManager::ConsoleBuffer.clear();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Engine Messages", &Editor::showEditorMessages);
            
            ImGui::Separator();
        
            // Filter console messages
            GameManager::ConsoleBuffer.clear();
            for (std::string str : GameManager::ConsoleMessages) {
                GameManager::ConsoleBuffer.append(str.c_str());
            }

            // Display messages
            ImGui::TextUnformatted(GameManager::ConsoleBuffer.begin(), GameManager::ConsoleBuffer.end());
            
            // Auto scroll
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.f);
            }
        ImGui::End();
    }

    rlImGuiEnd();
}

/**
 * @brief Input and general updates
 * 
 */
void Update(f32 deltaTime) {
    bool windowFocused = IsWindowFocused(); // Entire program window
    Vector2 mouse_pos = GetMousePosition();
    Vector2 world_mouse_pos = {mouse_pos.x - Editor::sceneWindowPosition.x - 8, 
                                mouse_pos.y - Editor::sceneWindowPosition.y - 30};
    world_mouse_pos = GetScreenToWorld2D(world_mouse_pos, GameManager::current_camera->camera);

    // We are editing the game
    if (!GameManager::isPlayingGame) {
        // Editor Camera Movement
        if (Editor::sceneWindowFocused && windowFocused) {
            f32 camSpeed = 8.f;
            
            // Camera speed modifier
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                camSpeed *= 2.0f;
            }

            // Camera Movement
            if (IsKeyDown(KEY_W)) {
                Editor::camera->MoveCamera({0, -camSpeed});
            }
            if (IsKeyDown(KEY_S)) {
                Editor::camera->MoveCamera({0, camSpeed});
            }
            if (IsKeyDown(KEY_A)) {
                Editor::camera->MoveCamera({-camSpeed, 0});
            }
            if (IsKeyDown(KEY_D)) {
                Editor::camera->MoveCamera({camSpeed, 0});
            }

            // Camera Zoom
            if (GetMouseWheelMove() > 0) {
                Editor::camera->camera.zoom += 0.1;
            }
            else if (GetMouseWheelMove() < 0) {
                Editor::camera->camera.zoom -= 0.1;
            }

            // Mouse Select
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                for (GameObject* obj : GameManager::GameObjects) {
                    if (GameManager::PointIntersects(*obj, world_mouse_pos)) {
                        Editor::viewPropertiesObject = obj;
                        break;
                    }
                }
            }

            // Middle Mouse Button
            // AKA Camera Move
            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
                Editor::ToggleCameraMove = true;
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
                Editor::ToggleCameraMove = false;
            }
            if (Editor::ToggleCameraMove) {
                Vector2 delta = {Editor::LastFrameMousePos.x - mouse_pos.x, 
                                 Editor::LastFrameMousePos.y - mouse_pos.y};
                // Multiply for speed
                delta.x *= 1.2;
                delta.y *= 1.2;
                Editor::camera->MoveCamera(delta);
            }
        }
    }
    // We are playing the game 
    else {
        GameManager::WorldMousePosition = world_mouse_pos;

        // Lua Scripts
        for (Entity* e : GameManager::Entities) {
            if (e->lua_scripts.empty()) continue;

            GameManager::lua["this"] = Lua::CreateLuaObject(e);
            GameManager::lua["variables"] = sol::new_table();

            // Run scripts
            for (std::string script : e->lua_scripts) {
                try {
                    auto result = GameManager::lua.script_file(script);
                    if (!result.valid()) {
                        GameManager::ConsoleError("Could not find script: " + script);
                    }
                } catch (sol::error error) {
                    GameManager::ConsoleError("Error running script: " + script);
                    GameManager::ConsoleError(error.what());
                    EndGame();
                }
            }
        }
    }

    for (GameObject* obj : GameManager::GameObjects) {
        Vector2 deltaPos = {obj->position.x - obj->previousPosition.x,
                            obj->position.y - obj->previousPosition.y};

        if (deltaPos.x != 0 || deltaPos.y != 0) {
            for (GameObject* child : obj->children) {
                child->position = {child->position.x + deltaPos.x,
                                    child->position.y + deltaPos.y};
            }
        }
        obj->previousPosition = obj->position;

        if (obj->type == CAMERA) {
            GameCamera* camera = (GameCamera*)obj;
            camera->camera.target = camera->position;
            camera->CalculateSize(Editor::sceneWindowSize);
        }
    }

    Editor::LastFrameMousePos = mouse_pos;
}

/**
 * @brief Physics
 * 
 */
void FixedUpdate(f32 deltaTime) {
    if (!GameManager::isPlayingGame) return;
    
    Editor::lastFixedUpdate += deltaTime;

    if (Editor::lastFixedUpdate >= Editor::fixedUpdateRate) {
        Editor::lastFixedUpdate -= Editor::fixedUpdateRate;
        for (Entity* e : GameManager::Entities) {
            if (e->bodytype == STATIC) continue;
            
            // Set Gravity
            if (!e->grounded) {
                e->velocity.y += 9.8f * Editor::fixedUpdateRate;
            }

            // Check for Collisions
            bool bottomCollision = false;
            
            Vector2 bottomPoint = {e->rect.x + e->rect.width/2, e->rect.y + e->rect.height};
            
            for (Entity* other : GameManager::Entities) {
                if (other->ID == e->ID) continue;

                // Bottom collision
                if (CheckCollisionPointRec(bottomPoint, other->rect)) {
                    if (!e->grounded) {
                        f32 YDiff = (e->rect.y + e->rect.height) - other->rect.y;
                        e->position.y -= YDiff;
                        e->UpdateRect();
                        e->velocity.y = 0.f;
                        e->grounded = true;
                        bottomCollision = true;
                    }
                }
            }

            if (!bottomCollision) {
                e->grounded = false;
            }

            // Apply Velocity
            e->position.x += e->velocity.x;
            e->position.y += e->velocity.y;
            e->UpdateRect();
        }
    }
}

/**
 * @brief Sets ImGui Colors and font
 * 
 */
void SetImGuiStyle() {
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    
    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Font
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    
    float fontSize = 18.f;
    io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", fontSize);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", fontSize);

    rlImGuiReloadFonts();
}

int main() {
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    // Initialize variables
    // Window
    GameManager::screenWidth = 1280;
    GameManager::screenHeight = 720;
    GameManager::framerateLimit = 244;

    // Project Details
    std::string projectTitle;
    bool loadedProject = false;

    // Scene Window
    bool sceneWindowFocused = false;
    Vector2 sceneWindowPos = {0,0};
    Vector2 sceneWindowSize = {0,0};
    Vector2 sceneMousePos = {0,0};
    bool sceneWindowMouseCapture = false;

    // Create Window and set framerate limit
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameManager::screenWidth, GameManager::screenHeight, "BaconEngine");
    SetTargetFPS(GameManager::framerateLimit);
    rlImGuiSetup(true);
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    SetImGuiStyle();
    
    // Initialize Frame
    Rendering::frame = LoadRenderTexture(GameManager::screenWidth, GameManager::screenHeight);
    BeginTextureMode(Rendering::frame);
        ClearBackground(DARKGRAY);
    EndTextureMode();

    // GameManager
    Input::InitInputMaps();
    GameManager::defaultFont = Rendering::b_LoadFont("./arial.ttf");
    GameManager::lua.open_libraries(sol::lib::base, sol::lib::package);
    Lua::RegisterFunctions();
    Lua::RegisterClasses();

    // Set Editor Camera
    Editor::camera = new GameCamera();
    GameManager::GameObjects.pop_back(); // Remove from Object lists since we don't count
    GameManager::GameCameras.pop_back(); // the editor camera as apart of the game
    GameManager::current_camera = Editor::camera;

    // Disable Raylib Logging
    SetTraceLogLevel(LOG_WARNING);

    // NativeFileDialog
    NFD_Init();

    // --------------------------------------------

    // For Faster Testing/Debugging
    // Remove at some point 

    File::LoadProject("C:/Users/Jackson/Desktop/BaconEngine Projects/Testing/Game.json");
    Editor::projectTitle = "C:/Users/Jackson/Desktop/BaconEngine Projects/Testing/Game.json";
    Editor::loadedProject = true;
    SetWindowTitle(std::string("Bacon - " + Editor::projectTitle).c_str());

    // --------------------------------------------

    while (!WindowShouldClose()) {
        f32 deltaTime = GetFrameTime();
        
        Rendering::DrawGameObjects();
        BeginDrawing();
            DrawUI(deltaTime);
        EndDrawing();
        
        Update(deltaTime);        
        FixedUpdate(deltaTime);
    }

    rlImGuiShutdown();
    CloseWindow();

    // _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    // _CrtDumpMemoryLeaks();

    return 0;
}
