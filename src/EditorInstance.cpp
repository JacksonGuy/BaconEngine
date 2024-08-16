#include "EditorInstance.hpp"

#include <iostream>
#include <filesystem>

#include <nfd.h>

#include "GameManager.hpp"
#include "File.hpp"
#include "Text.hpp"
#include "Lua/LuaApi.hpp"
#include "Sound.hpp"
#include "Input.hpp"
#include "Rendering.hpp"
#include "Camera.hpp"

namespace Settings {
    int selectedResolution = 2;         // Default Resolution
    const int resolutionsCount = 6;     // Size of resolutions array below
    std::string EngineVersion;          // Engine Version
    
    // Possible resolutions 
    const char* resolutions[] = {
        "640x480",
        "800x600",
        "1280x720",
        "1440x900",
        "1600x900",
        "1920x1080"
    };

    // String Resolution to Vector
    std::map<std::string, sf::Vector2u> resolutionConvert = {
        {"640x480", sf::Vector2u(640, 480)},
        {"800x600", sf::Vector2u(800, 600)},
        {"1280x720", sf::Vector2u(1280, 720)},
        {"1440x900", sf::Vector2u(1440, 900)},
        {"1600x900", sf::Vector2u(1600, 900)},
        {"1920x1080", sf::Vector2u(1920,1080)}
    };
    
    // Returns the windows current resolution as a vector
    sf::Vector2u GetCurrentResolution() {
        std::string setting = Settings::resolutions[Settings::selectedResolution];
        return Settings::resolutionConvert[setting];
    }

    // Returns the windows current resolution as a string
    std::string GetResolutionString() {
        return Settings::resolutions[Settings::selectedResolution];
    }

    // Applies Config File Settings
    void ApplySettings(File::ConfigState state) {
        for (int i = 0; i < Settings::resolutionsCount; i++) {
            if (Settings::resolutions[i] == state.resolution) {
                selectedResolution = i;
                break;
            }
        }
        Settings::EngineVersion = state.version;
    }

    // Changes the windows resolution
    void ChangeResolution(sf::RenderWindow& window) {
        sf::Vector2u size = Settings::GetCurrentResolution();
        window.setSize(size);
        GameManager::screenWidth = size.x;
        GameManager::screenHeight = size.y;
    }
}

EditorInstance::EditorInstance() {
    this->m_projectTitle = "Untitled Project";
    this->m_loadedProject = false;

    this->m_window = new sf::RenderWindow(sf::VideoMode(GameManager::screenWidth, GameManager::screenHeight),
        "Bacon - " + this->m_projectTitle, sf::Style::Default);
    
    // Create ImGui Context
    if (!ImGui::SFML::Init(*m_window, false)) {
        std::cout << "[ERROR] Failed to initialize ImGui" << std::endl;
        return;
    }
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    SetImGuiStyle();

    // Load Editor Settings
    File::ConfigState settings = File::loadConfig();
    Settings::ApplySettings(settings);
    Settings::ChangeResolution(*m_window);

    // Rendering
    Rendering::frame.create(1280, 720);

    // Set Camera
    this->m_camera = new sf::View(sf::Vector2f(0,0), sf::Vector2f(GameManager::screenWidth, GameManager::screenHeight));
    Rendering::frame.setView(*m_camera);
    // m_window->setView(*m_camera);
    this->m_cameraZoom = 1.0f;

    // Default Font
    GameManager::font.loadFromFile("arial.ttf");

    // Set Engine UI Variables
    this->m_showDockSpace = false;
    this->m_cameraMove = false;                                       // Is the Camera Moving?
    this->m_showEntityCreate = false;                                 // Entity Create Menu
    this->m_showMainMenu = true;                                      // General Main Menu
    this->m_showEntityList = false;                                   // List of Entities
    this->m_showConsole = true;                                      // Debug Console
    this->m_showSettingsMenu = false;                                 // Editor Settings
    this->m_showTextCreate = false;                                   // Text create menu
    this->m_showCameraCreate = false;                                 // Camera create menu

    this->m_failedMessage = "";                                       // What error to display

    // Create Entity Variables
    this->m_createPosition[0] = 0.f;                                  // Default Position
    this->m_createPosition[1] = 0.f;
    strcpy(this->m_createNameBuffer, "Entity");                       // Default Name
    strcpy(this->m_createImagePath, "");                              // Default Image Path
    this->m_createDimension[0] = 128;                                 // Default Entity Dimensions
    this->m_createDimension[1] = 128;

    this->m_HitboxAdjust = 1.f;
    this->m_viewObject = nullptr;

    // Create Text Variables
    this->m_createTextPosition[0] = 0.f;
    this->m_createTextPosition[1] = 0.f;
    strcpy(this->m_createTextName, "Text");
    strcpy(this->m_createTextDetails, "");

    // Create Camera Variables
    strcpy(this->m_createCameraName, "Camera");
    this->m_createCameraPosition[0] = 0.f;
    this->m_createCameraPosition[1] = 0.f;
    this->m_createCameraSize[0] = GameManager::screenWidth;
    this->m_createCameraSize[1] = GameManager::screenHeight;

    // Setup Lua API
    Lua::RegisterFunctions();

    // Input
    Input::InitInputMaps();

    // Other
    this->m_lastFixedUpdate = sf::Time::Zero;
    this->m_frameLimit = 60;                                          // Change if necessary
    this->m_TimePerFrame = sf::seconds(1.f / m_frameLimit);
    this->m_window->setFramerateLimit(165);
}

EditorInstance::~EditorInstance() {
    delete m_window;
    delete m_camera;

    Sound::stop_sounds();
    Sound::clean_music();
    Sound::clean_sounds();
}

// Main Game Loop
void EditorInstance::Run() {
    // DEBUG
    std::string demoPath = "C:/Users/Jackson/Desktop/BaconEngine Projects/Testing/Game.json";
    // std::string demoPath = "/home/jack/BaconProjects/Test/Game.json";
    std::filesystem::path demo = std::filesystem::relative(demoPath);
    m_projectTitle = demoPath;
    m_loadedProject = true;
    m_window->setTitle(m_projectTitle);
    File::load(demo.generic_string());
    m_showEntityList = true;

    while(m_window->isOpen()) {
        sf::Time deltaTime = m_clock.restart();

        this->DrawUI(deltaTime);         // ImGui Frames
        this->Update(deltaTime);         // Input
        this->FixedUpdate(deltaTime);    // Physics and Lua

        Rendering::DrawGameObjects(*m_window);

        if (m_viewObject != nullptr) {
            float gap = 5.f;
            
            if (m_viewObject->type == ENTITY) {
                Entity* e = (Entity*)m_viewObject;
                sf::Vector2f pos = e->position - sf::Vector2f(gap, gap) - sf::Vector2f(e->width/2 , e->height/2);
                sf::Vector2f size = sf::Vector2f(e->width + gap * 2, e->height + gap * 2);

                sf::Vertex border[] = {
                    sf::Vertex(sf::Vector2f(pos.x, pos.y)),
                    sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y)),
                    sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y + size.y)),
                    sf::Vertex(sf::Vector2f(pos.x, pos.y + size.y)),
                    sf::Vertex(sf::Vector2f(pos.x, pos.y)),
                };
                Rendering::frame.draw(border, 5, sf::LinesStrip);
            }
            else if (m_viewObject->type == TEXT) {
                TextObj* text = (TextObj*)m_viewObject;
                sf::FloatRect rect = text->text.getGlobalBounds();
                sf::Vector2f pos(rect.left - gap, rect.top - gap);
                sf::Vector2f size(rect.width + gap*2, rect.height + gap*2);
                
                sf::Vertex border[] = {
                    sf::Vertex(sf::Vector2f(pos.x, pos.y)),
                    sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y)),
                    sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y + size.y)),
                    sf::Vertex(sf::Vector2f(pos.x, pos.y + size.y)),
                    sf::Vertex(sf::Vector2f(pos.x, pos.y)),
                };

                Rendering::frame.draw(border, 5, sf::LinesStrip);
            }
            else if (m_viewObject->type == CAMERA) {
                Camera* cam = (Camera*)m_viewObject;
                cam->isVisible = true;
            }

            Rendering::frame.display();
        }

        ImGui::SFML::Render(*this->m_window);
        this->m_window->display();
    }

    // Save editor settings before closing
    File::ConfigState finalSettings = {
        Settings::EngineVersion,
        Settings::GetResolutionString()
    };
    File::saveConfig(finalSettings);
    std::cout << "Saved Editor Settings" << std::endl;

    this->m_window->close();
    ImGui::SFML::Shutdown();
}

// Draw UI
void EditorInstance::DrawUI(sf::Time deltaTime) {
    float currentTime = deltaTime.asSeconds();
    float fps = 1.0f / currentTime;

    ImGui::SFML::Update(*m_window, deltaTime);
    ImGui::DockSpaceOverViewport();

    // Game Window
    ImGui::Begin("Scene");
        m_swindowFocused = ImGui::IsWindowFocused(); 
        m_swindowpos = (sf::Vector2f)ImGui::GetWindowPos();
        m_swindowsize = (sf::Vector2f)ImGui::GetContentRegionAvail();
        m_sceneMouseCapture = ImGui::IsWindowHovered();

        sf::Vector2f frameSize = (sf::Vector2f)Rendering::frame.getSize();
        if (frameSize.x != m_swindowsize.x || frameSize.y != m_swindowsize.y) {
            
            sf::Vector2u prevSize = Rendering::frame.getSize();
            sf::Vector2f ratio = sf::Vector2f(m_swindowsize.x/prevSize.x, m_swindowsize.y/prevSize.y);
            sf::Vector2f prevCamSize = m_camera->getSize();

            Rendering::frame.create(m_swindowsize.x, m_swindowsize.y);
            m_camera->setSize(prevCamSize.x * ratio.x, prevCamSize.y * ratio.y);
            if (GameManager::camera != nullptr) {
                prevCamSize = GameManager::camera->view->getSize(); 
                GameManager::camera->view->setSize(prevCamSize.x * ratio.x, prevCamSize.y * ratio.y);
            }
            Rendering::frame.setView(*m_camera);
        }

        ImGui::Image(Rendering::frame);
    ImGui::End();

    // Top menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (GameManager::isPlayingGame) {
                ImGui::BeginDisabled();
            }

            if (ImGui::MenuItem("New Project")) {
                bool result;
                std::string path;
                if (!m_loadedProject) {
                    result = File::CreateNew(path);
                }
                else {
                    File::save(m_projectTitle);
                    result = File::CreateNew(path);
                }

                if (result != false) {
                    m_projectTitle = path;
                    m_loadedProject = true;
                    m_window->setTitle("Bacon - " + path);
                }
            }

            if (ImGui::MenuItem("Save")) {
                if (!m_loadedProject) {
                    nfdchar_t* savepath = NULL;
                    nfdresult_t result = NFD_SaveDialog("json", NULL, &savepath);

                    if (result == NFD_OKAY) {
                        GameManager::ConsoleWrite("[ENGINE] Successfully Saved Project");
                        std::string name = savepath;
                        name += ".json";
                        File::save(name);
                        m_projectTitle = name;
                        m_loadedProject = true;
                        m_window->setTitle("Bacon - " + name);
                    }
                    free(savepath);
                }
                else { 
                    File::save(m_projectTitle);
                }
            }
            
            if (ImGui::MenuItem("Save as")) {
                nfdchar_t* savepath = NULL;
                nfdresult_t result = NFD_SaveDialog("json", NULL, &savepath);

                if (result == NFD_OKAY) {
                    GameManager::ConsoleWrite("[ENGINE] Successfully Saved Project");
                    std::string name = savepath;
                    name += ".json";
                    File::save(name);
                    m_projectTitle = name;
                    m_loadedProject = true;
                    m_window->setTitle("Bacon - " + name);
                }
                free(savepath);
            }
            
            if (ImGui::MenuItem("Load")) {
                nfdchar_t* outpath = NULL;
                nfdresult_t result = NFD_OpenDialog("json", NULL, &outpath);

                if (result == NFD_OKAY) {
                    if (File::load(outpath)) {
                        GameManager::ConsoleWrite("[ENGINE] Successfully Loaded Project");
                        m_loadedProject = true;
                        m_projectTitle = outpath;
                        GameManager::entryPoint = outpath;
                        m_window->setTitle("Bacon - " + m_projectTitle);
                    }
                    else {
                        ImGui::OpenPopup("Failed");
                        m_failedMessage = "Error: Failed to load project.";
                    }
                }
                free(outpath);
            }

            if (GameManager::isPlayingGame) {
                ImGui::EndDisabled();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Create")) {
                ImGui::MenuItem("Entity", NULL, &m_showEntityCreate);
                ImGui::MenuItem("Text", NULL, &m_showTextCreate);
                ImGui::MenuItem("Camera", NULL, &m_showCameraCreate);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Object List", NULL, &m_showEntityList);
            ImGui::MenuItem("Game Details", NULL, &m_showMainMenu);
            ImGui::MenuItem("Console", NULL, &m_showConsole);
            ImGui::MenuItem("Menu Docking", NULL, &m_showDockSpace);
            ImGui::MenuItem("Editor Settings", NULL, &m_showSettingsMenu);
            ImGui::EndMenu();
        }

        if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text(m_failedMessage.c_str());
            if (ImGui::Button("OK")) {
                m_failedMessage = "";
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndMainMenuBar();
    }

    // General Menu
    if (m_showMainMenu) {
        ImGui::Begin("Game Details", &m_showMainMenu);
            ImGui::Text(Settings::EngineVersion.c_str());
            ImGui::Text( ("FPS: " + std::to_string((int)fps)).c_str() );
            if (!GameManager::isPlayingGame) {
                if (ImGui::Button("Play Game")) {
                    GameManager::SaveEditorState(*m_window, m_projectTitle);
                    
                    // Save editor camera position and size
                    m_cameraPos = m_camera->getCenter();
                    m_cameraSize = m_camera->getSize();

                    if (GameManager::camera != nullptr) {
                        m_window->setView(*GameManager::camera->view);
                    }

                    GameManager::clock.restart();

                    GameManager::ConsoleWrite("[PLAYER] Editor data saved. Starting game...");
                    GameManager::isPlayingGame = true;
                }
            }
            else {
                if (ImGui::Button("End Game")) {
                    GameManager::RestoreEditorState(*m_window, m_projectTitle);
                    
                    // Restore editor camera
                    m_camera->setCenter(m_cameraPos);
                    m_camera->setSize(m_cameraSize);
                    Rendering::frame.setView(*m_camera);
                    
                    // Stop in game sounds
                    Sound::stop_sounds();

                    GameManager::isPlayingGame = false;
                    GameManager::ConsoleWrite("[PLAYER] Game Ended. Editor data restored.");
                }
            }
            ImGui::Separator();
            ImGui::BeginTabBar("EngineItems");
                if (ImGui::BeginTabItem("Camera")) {
                    float cameraPos[2] = {m_camera->getCenter().x, m_camera->getCenter().y};
                    if (ImGui::InputFloat2("Position", cameraPos)) {
                        m_camera->setCenter(sf::Vector2f(cameraPos[0], cameraPos[1]));
                        Rendering::frame.setView(*m_camera);
                    }

                    float cameraSize[2] = {m_camera->getSize().x, m_camera->getSize().y};
                    if (ImGui::InputFloat2("Size", cameraSize)) {
                        m_camera->setSize(cameraSize[0], cameraSize[1]);
                        Rendering::frame.setView(*m_camera);
                    }
                    ImGui::EndTabItem();

                    if (ImGui::Button("Reset Camera")) {
                        m_camera->setCenter(0,0);
                        m_camera->setSize(m_swindowsize);
                        Rendering::frame.setView(*m_camera);
                    }
                }

                if (ImGui::BeginTabItem("Game Settings")) {
                    ImGui::InputFloat("Gravity", &GameManager::gravity);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Testing")) {
                    if (ImGui::Button("Testing")) {
                        const char* consoleBuff = GameManager::ConsoleLog.begin();
                        int size = GameManager::ConsoleLog.size();
                        std::vector<char> parseBuff;
                        for (int i = 0; i < size; i++) {
                            parseBuff.push_back(consoleBuff[i]); 
                            if (parseBuff.size() == 8) {
                                std::string str;
                                for (char c : parseBuff) {
                                    str.push_back(c);
                                }

                                if (str == "[ENGINE]") {
                                    std::cout << "engine message found\n";
                                }
                                parseBuff.pop_back();
                            }
                        }
                    }
                    
                    if (ImGui::Button("Print Layers")) {
                        for (Rendering::RenderingLayer layer : Rendering::m_layers) {
                            std::cout << "Layer " << layer.layerNum << ":\n";
                            for (GameObject* obj : layer.objects) {
                                std::cout << "\t" << obj->ID << ": " << obj->name << "\n";
                            }
                        }
                        std::cout << "\n";
                    }

                    if (ImGui::Button("Print Entities")) {
                        for (Entity* e : GameManager::Entities) {
                            std::cout << e->ID << ": " << e->name << "\n"; 
                        }
                        std::cout << "\n";
                    }

                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
        ImGui::End();
    }

    // Display GameObjects
    ImGui::Begin("Properties");
        if (m_viewObject != nullptr) {
            // Show Entity Details
            if (m_viewObject->type == ENTITY) {
                Entity* e = (Entity*)m_viewObject;

                std::string idText = "ID: " + std::to_string(e->ID);
                ImGui::Text(idText.c_str());

                char nameBuff[BUFFSIZE];
                strcpy(nameBuff, e->name.data());
                if (ImGui::InputText("Name", nameBuff, BUFFSIZE)) {
                    e->name = nameBuff;
                }

                char tagBuff[BUFFSIZE];
                strcpy(tagBuff, e->tag.data());
                if (ImGui::InputText("Tag", tagBuff, BUFFSIZE)) {
                    e->tag = tagBuff;
                }

                int e_layer = e->layer;
                if (ImGui::InputInt("Layer", &e_layer)) {
                    if (e_layer >= 0) Rendering::SwapLayer(e, e_layer);
                    else e_layer = 0;
                }

                if (GameManager::player != nullptr && GameManager::player != e) {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Checkbox("Player", &e->isPlayer)) {
                    if (GameManager::player == nullptr) {
                        GameManager::player = e;
                    }
                    else {
                        GameManager::player = nullptr;
                    }
                }
                if (GameManager::player != nullptr && GameManager::player != e) {
                    ImGui::EndDisabled();
                }

                ImGui::Separator();

                ImGui::BeginTabBar("EntityDetails");
                if (ImGui::BeginTabItem("Details")) {
                    char textBuff[BUFFSIZE];
                    std::string relpath = std::filesystem::relative(e->texturePath.data(), GameManager::entryPoint).generic_string();
                    strcpy(textBuff, relpath.data());
                    ImGui::InputText("Texture", textBuff, BUFFSIZE);
                    if (ImGui::Button("Change Texture")) {
                        nfdchar_t* outpath = NULL;
                        nfdresult_t result = NFD_OpenDialog("png,jpg", NULL, &outpath);

                        if (result == NFD_OKAY) {
                            //std::string path = std::filesystem::relative(outpath, GameManager::entryPoint).generic_string();
                            //e->SetSprite(path);
                            e->SetSprite(outpath);
                        }
                        free(outpath);
                    }
                    ImGui::Checkbox("Visible", &e->isVisible);
                    ImGui::Separator();
                    
                    // Temporary Conversion to float array for input
                    float e_pos[] = {e->position.x, e->position.y};
                    if (ImGui::InputFloat2("Position", e_pos)) {
                        e->SetPosition({e_pos[0], e_pos[1]});
                    }

                    if (ImGui::InputInt("Width", &e->width)) {
                        e->SetSprite(e->texturePath);
                    }
                    if (ImGui::InputInt("Height", &e->height)) {
                        e->SetSprite(e->texturePath);
                    }

                    if (ImGui::InputFloat("Rotation", &e->rotation)) {
                        // Adjust rotation angle if necessary
                        if (e->rotation > 360) {
                            e->rotation -= 360;
                        }
                        if (e->rotation < -360) {
                            e->rotation += 360;
                        }

                        // Rotate
                        e->sprite.setRotation(e->rotation);
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Save as prefab")) {
                        nfdchar_t* savepath = NULL;
                        nfdresult_t result = NFD_SaveDialog("json", NULL, &savepath);

                        if (result == NFD_OKAY) {
                            //std::string rel = std::filesystem::relative(savepath, GameManager::entryPoint).generic_string();
                            //savePrefab(rel, e);
                            File::savePrefab(savepath, e);
                        }

                        free(savepath);
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Load from prefab")) {
                        nfdchar_t* loadpath = NULL;
                        nfdresult_t result = NFD_OpenDialog("json", NULL, &loadpath);

                        if (result == NFD_OKAY) {
                            // This process is pretty slow and dumb but hey it works
                            Entity* prefabEntity = File::loadPrefab(loadpath);
                            e->Overwrite(*prefabEntity);
                            delete(prefabEntity);
                            GameObject::IDCount--;
                        }

                        free(loadpath);
                    }

                    if (ImGui::Button("Delete")) {
                        delete(e);
                        m_viewObject = nullptr;
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Physics")) {
                    if (ImGui::Checkbox("Solid", &e->isSolid)) {
                        // Can't be a physics objects without being solid
                        if (!e->isSolid) {
                            e->physicsObject = false;
                            e->showHitbox = false;
                            e->velocity = sf::Vector2f(0, 0);
                            e->acceleration = sf::Vector2f(0, 0);
                            e->grounded = false;
                        }
                    }

                    if (e->isSolid) {
                        ImGui::SameLine();
                        ImGui::Checkbox("Physics Object", &e->physicsObject);
                    }

                    if (e->isSolid) {
                        ImGui::Checkbox("Show Hitbox", &e->showHitbox);
                        
                        if (e->showHitbox) {
                            ImGui::SameLine();
                            ImGui::Checkbox("Edit Hitbox", &e->editHitbox);
                        }

                        if (e->editHitbox && e->showHitbox) {
                            for (int i = 0; i < 4; i++) {
                                float pointCoords[] = {e->hitbox[i].x, e->hitbox[i].y};
                                
                                std::string pointName;
                                if (i == 0) pointName = "Top Left";
                                else if (i == 1) pointName = "Top Right";
                                else if (i == 2) pointName = "Bottom Right";
                                else if (i == 3) pointName = "Bottom Left";

                                if (ImGui::InputFloat2(pointName.data(), pointCoords)) {
                                    e->hitbox[i].x = pointCoords[0];
                                    e->hitbox[i].y = pointCoords[1];
                                
                                    if (i == 0) {
                                        e->hitbox[4].x = pointCoords[0];
                                        e->hitbox[4].y = pointCoords[1];
                                    }
                                }

                                // free(pointCoords);
                            }
                        }

                        if (ImGui::Button("Reset Hitbox")) {
                            e->CreateHitbox();
                        }

                        ImGui::Separator();
                    }

                    if (e->physicsObject) {
                        ImGui::InputFloat("Mass", &e->mass);
                        ImGui::SetItemTooltip("This doesn't do anything currently");
                        
                        float velocity[] = {e->velocity.x, e->velocity.y};
                        float acceleration[] = {e->acceleration.x, e->acceleration.y};
                        if (ImGui::InputFloat2("Velocity", velocity)) {
                            e->velocity = sf::Vector2f(velocity[0], velocity[1]);
                        }
                        if (ImGui::InputFloat2("Acceleration", acceleration)) {
                            e->acceleration = sf::Vector2f(acceleration[0], acceleration[1]);
                        }
                        
                        ImGui::BeginDisabled();
                            ImGui::Checkbox("Grounded", &e->grounded);
                        ImGui::EndDisabled();
                    }
                    
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Scripting")) {
                    ImGui::Text("Scripts");
                    auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    for (size_t index = 0; index < e->lua_scripts.size(); index++) {
                        ScriptItem script = e->lua_scripts[index];
                        std::string relPath = std::filesystem::relative(script.path, GameManager::entryPoint).generic_string();
                        //relPath.erase(0, 3);
                        ImGui::TreeNodeEx(relPath.data(), flags);
                        if (ImGui::IsItemClicked()) {
                            script.showDetails = !script.showDetails;
                        }
                        
                        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                        ImGui::PushID(index);
                        if (ImGui::Button("X")) {
                            e->lua_scripts.erase(e->lua_scripts.begin() + index);
                        }
                        ImGui::PopID();
                    }

                    if (ImGui::Button("Add Lua Script")) {
                        m_AddAttributeEntity = e;

                        nfdchar_t* scriptpath = NULL;
                        nfdresult_t result = NFD_OpenDialog("lua", NULL, &scriptpath);

                        if (result == NFD_OKAY) {
                            ScriptItem script;
                            script.path = scriptpath;
                            script.showDetails = false;
                            m_AddAttributeEntity->lua_scripts.push_back(script);
                        }

                        free(scriptpath);
                    }

                    ImGui::Separator();

                    ImGui::Text("Custom Variables");

                    for (size_t i = 0; i < e->variables.size(); i++) {
                        
                        EntityVar var = e->variables[i];
                        if (var.type == NUMBER) {
                            ImGui::InputDouble(var.name.data(), &e->variables[i].numval);

                            ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                            ImGui::PushID(var.name.data());
                            if (ImGui::Button("X")) {
                                e->variables.erase(e->variables.begin() + i);
                            }
                            ImGui::PopID();
                        }
                        else if (var.type == STRING) {
                            char textBuff[512];
                            strcpy(textBuff, e->variables[i].stringval.data());
                            ImGui::InputText(var.name.data(), textBuff, 512);

                            ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                            ImGui::PushID(var.name.data());
                            if (ImGui::Button("X")) {
                                e->variables.erase(e->variables.begin() + i);
                            }
                            ImGui::PopID();
                        }
                    }

                    if (ImGui::Button("Add variable")) {
                        m_AddAttributeEntity = e;
                        ImGui::OpenPopup("AddVariableEntity");
                    }

                    if (ImGui::BeginPopupModal("AddVariableEntity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::Text("Add Variable");
                        ImGui::Separator();
                        ImGui::InputText("Name", m_AddVariableName, 512);
                        
                        ImGui::RadioButton("Number", &m_AddVariableType, 0);
                        ImGui::SameLine();
                        ImGui::RadioButton("String", &m_AddVariableType, 1);

                        if (m_AddVariableType == 0) {
                            ImGui::InputDouble("Value", &m_AddVariableNumber);
                        }
                        else if (m_AddVariableType == 1) {
                            ImGui::InputText("Value", m_AddVariableString, 512);
                        }

                        if (ImGui::Button("Add")) {

                            int count = e->variables.size();
                            EntityVar var;
                            var.name = m_AddVariableName;
                            if (m_AddVariableType == 0) {
                                var.type = NUMBER;
                                var.numval = m_AddVariableNumber;
                            }
                            else {
                                var.type = STRING;
                                var.stringval = m_AddVariableString;
                            }

                            e->variables.push_back(var);

                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        
            // Show Text Details
            else if (m_viewObject->type == TEXT) {
                TextObj* text = (TextObj*)m_viewObject;

                std::string idText = "ID: " + std::to_string(text->ID);
                ImGui::Text(idText.c_str());
                
                char nameBuff[BUFFSIZE];
                strcpy(nameBuff, text->name.data());
                if (ImGui::InputText("Name", nameBuff, BUFFSIZE)) {
                    text->name = nameBuff;
                }

                char tagBuff[BUFFSIZE];
                strcpy(tagBuff, text->tag.data());
                if (ImGui::InputText("Tag", tagBuff, BUFFSIZE)) {
                    text->tag = tagBuff;
                }

                int text_layer = text->layer;
                if (ImGui::InputInt("Layer", &text_layer)) {
                    if (text_layer >= 0) Rendering::SwapLayer(text, text_layer);
                    else text_layer = 0;
                }

                ImGui::Separator();

                float text_pos[] = {text->position.x, text->position.y};
                if (ImGui::InputFloat2("Position", text_pos)) {
                    text->position = sf::Vector2f(text_pos[0], text_pos[1]);
                    text->text.setPosition(text->position);
                }

                float text_rotation = text->text.getRotation();
                if (ImGui::InputFloat("Rotation", &text_rotation)) {
                    text->text.setRotation(text_rotation);
                }

                int text_size = text->text.getCharacterSize();
                if (ImGui::InputInt("Size", &text_size)) {
                    text->text.setCharacterSize(text_size);
                }

                // Cursed conversions
                sf::Color color = text->text.getFillColor();
                float color_arr[] = {color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f};
                if (ImGui::ColorEdit4("Color", color_arr)) {
                    text->text.setFillColor(sf::Color(
                        color_arr[0] * 255,
                        color_arr[1] * 255,
                        color_arr[2] * 255,
                        color_arr[3] * 255));
                }

                ImGui::Checkbox("Visible", &text->isVisible);

                ImGui::Separator();

                std::string textString = text->text.getString();
                char text_buffer[1024 * 16];
                strcpy(text_buffer, textString.c_str()); 
                if (ImGui::InputTextMultiline("Text", text_buffer, 1024 * 16)) {
                    text->text.setString(text_buffer);
                }

                if (ImGui::Button("Delete")) {
                    delete(text);
                    m_viewObject = nullptr;
                }
            }
        
            // Show Camera Details
            else if (m_viewObject->type == CAMERA) {
                Camera* camera = (Camera*)m_viewObject;
                
                std::string idText = "ID: " + std::to_string(camera->ID);
                ImGui::Text(idText.c_str());
                
                char nameBuff[BUFFSIZE];
                strcpy(nameBuff, camera->name.data());
                if (ImGui::InputText("Name", nameBuff, BUFFSIZE)) {
                    camera->name = nameBuff;
                }

                int layerBuff = camera->layer;
                if (ImGui::InputInt("Layer", &layerBuff)) {
                    if (layerBuff >= 0) Rendering::SwapLayer(camera, layerBuff);
                    else layerBuff = 0;
                }

                float posBuff[] = {camera->position.x, camera->position.y};
                if (ImGui::InputFloat2("Position", posBuff)) {
                    camera->SetPosition(sf::Vector2f(posBuff[0], posBuff[1]));
                    camera->view->setCenter(camera->position);
                }

                int sizeBuff[] = {camera->width, camera->height};
                if (ImGui::InputInt2("Size", sizeBuff)) {
                    camera->width = sizeBuff[0];
                    camera->height = sizeBuff[1];
                    camera->view->setSize(camera->width, camera->height);
                }

                if (ImGui::InputFloat("Rotation", &camera->rotation)) {
                    camera->view->setRotation(camera->rotation);
                }

                ImGui::Separator();

                if (GameManager::camera != nullptr && GameManager::camera != camera) {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Checkbox("Active Camera", &camera->isActive)) {
                    if (GameManager::camera == nullptr) {
                        GameManager::camera = camera;
                    }
                    else {
                        GameManager::camera = nullptr;
                    }
                }
                if (GameManager::camera != nullptr && GameManager::camera != camera) {
                    ImGui::EndDisabled();
                }

                ImGui::SameLine();
                ImGui::Checkbox("Visible", &camera->isVisible);

                ImGui::Separator();

                if (ImGui::Button("Delete")) {
                    delete(camera);
                    m_viewObject = nullptr;
                }
            }
        }
    ImGui::End();

    // Create new Entity menu
    if (m_showEntityCreate) {
        ImGui::Begin("Create new entity", &m_showEntityCreate);
            // Displays
            ImGui::InputText("Name", m_createNameBuffer, BUFFSIZE);
            ImGui::InputFloat2("Position", m_createPosition);
            ImGui::InputText("Image Path", m_createImagePath, BUFFSIZE);
            if (ImGui::Button("Select Image")) {
                nfdchar_t* imagepath = NULL;
                nfdresult_t result = NFD_OpenDialog("png,jpg", NULL, &imagepath);

                if (result == NFD_OKAY) {
                    //std::string converted = std::filesystem::relative(imagepath, GameManager::entryPoint).generic_string();
                    //strcpy(createImagePath, converted.c_str());
                    strcpy(m_createImagePath, imagepath);
                }

                free(imagepath);
            } 

            ImGui::InputInt2("Dimensions", m_createDimension);

            // Create Button
            if (ImGui::Button("Create")) {
                Entity* entity = new Entity(sf::Vector2f(m_createPosition[0], m_createPosition[1]));
                entity->name = m_createNameBuffer;
                entity->width = m_createDimension[0];
                entity->height = m_createDimension[1];
                entity->CreateHitbox();
                entity->SetSprite(m_createImagePath);

                Rendering::AddToLayer(entity);

                m_showEntityCreate = false; // Close Window
            }
        ImGui::End();
    }

    // Create new Text Object
    if (m_showTextCreate) {
        ImGui::Begin("Create Text Object", &m_showTextCreate);
            ImGui::InputText("Name", m_createTextName, BUFFSIZE);
            ImGui::InputFloat2("Position", m_createTextPosition);
            ImGui::InputText("Text", m_createTextDetails, 64*BUFFSIZE);

            if (ImGui::Button("Create")) {
                TextObj* text = new TextObj();

                text->name = m_createTextName;
                text->position = sf::Vector2f(m_createTextPosition[0], m_createTextPosition[1]);
                text->text.setString(m_createTextDetails);

                Rendering::AddToLayer(text);

                m_showTextCreate = false; // Close Window
            }
        ImGui::End();
    }

    // Create new Camera
    if (m_showCameraCreate) {
        ImGui::Begin("Create Camera", &m_showCameraCreate);
            ImGui::InputText("Name", m_createCameraName, BUFFSIZE);
            ImGui::InputFloat2("Position", m_createCameraPosition);
            ImGui::InputInt2("Size", m_createCameraSize);

            if (ImGui::Button("Create")) {
                Camera* camera = new Camera();
                camera->name = m_createCameraName;
                camera->position = sf::Vector2f(m_createCameraPosition[0], m_createCameraPosition[1]);
                camera->width = m_createCameraSize[0];
                camera->height = m_createCameraSize[1];

                camera->view->setCenter(camera->position);
                camera->view->setSize(sf::Vector2f(camera->width, camera->height));

                Rendering::AddToLayer(camera);

                m_showCameraCreate = false;
            }
        ImGui::End();
    }

    // List all Objects
    if (m_showEntityList) {
        auto normalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        auto parentFlags =  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        ImGui::Begin("Objects", &m_showEntityList);
            std::vector<int> nodes;
            ImGui::SetNextItemOpen(true);
            // Parent Tree Node
            if (ImGui::TreeNodeEx("Game", parentFlags)) {
                // Handle Drag and Drop
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_LIST_ID")) {
                        // Get Entity
                        unsigned int sourceID = *(unsigned int*)payload->Data;
                        GameObject* sourceObject = GameManager::FindObjectByID(sourceID);

                        // Remove entity from current parent if it has one
                        if (sourceObject->parent != nullptr) {
                            for (size_t i = 0; i < sourceObject->parent->children.size(); i++) {
                                if (sourceObject->parent->children[i]->ID == sourceObject->ID) {
                                    sourceObject->parent->children.erase(sourceObject->parent->children.begin() + i);
                                }
                            }
                        }

                        sourceObject->parent = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }
                
                // Display Entities
                for (GameObject* obj : GameManager::GameObjects) {
                    if (obj->parent == nullptr) {
                        DisplayEntityTree(obj);
                    }
                }
                ImGui::TreePop();
            }
        ImGui::End();
    }

    // Show Debug Console
    if (m_showConsole) {
        ImGui::Begin("Console", &m_showConsole);
            if (ImGui::Button("Clear")) {
                GameManager::ConsoleMessages.clear();
                GameManager::ConsoleLog.clear();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Engine Messages", &m_ConsoleEngineMessages);
            ImGui::Separator();

            GameManager::ConsoleLog.clear();
            for (std::string str : GameManager::ConsoleMessages) {
                std::string prefix = str.substr(0, 8);

                if (prefix == "[ENGINE]") {
                    if (m_ConsoleEngineMessages) {
                        GameManager::ConsoleLog.append(str.data());
                    }
                }
                else {
                    GameManager::ConsoleLog.append(str.data());
                }
            }
            ImGui::TextUnformatted(GameManager::ConsoleLog.begin(), GameManager::ConsoleLog.end());
            
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
        ImGui::End();
    }

    // Editor Settings
    if (m_showSettingsMenu) {
        ImGui::Begin("Settings", &m_showSettingsMenu);
            ImGui::BeginTabBar("SettingsMenus", ImGuiTabBarFlags_None);
                if (ImGui::BeginTabItem("Display")) {
                    ImGui::Text("Resolution");
                    const char* previewValue = Settings::resolutions[Settings::selectedResolution];
                    if (ImGui::BeginCombo("##", previewValue)) {
                        for (int i = 0; i < Settings::resolutionsCount; i++) {
                            const bool isSelected = (Settings::selectedResolution == i);
                            if (ImGui::Selectable(Settings::resolutions[i], isSelected)) {
                                Settings::selectedResolution = i;
                                Settings::ChangeResolution(*m_window);
                            }

                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Sound")) {
                    ImGui::Text("Volume");
                    if (ImGui::SliderFloat("Master", &Sound::masterVolume, 0.f, 100.f)) {
                        Sound::setMasterVolume(Sound::masterVolume);
                    }
                    if (ImGui::SliderFloat("Effects", &Sound::effectsVolume, 0.f, 100.f)) {
                        Sound::setEffectsVolume(Sound::effectsVolume);
                    }
                    if (ImGui::SliderFloat("Music", &Sound::musicVolume, 0.f, 100.f)) {
                        Sound::setMusicVolume(Sound::musicVolume);
                    }
                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
        ImGui::End();
    }
}

// Mostly just editor input
void EditorInstance::Update(sf::Time deltaTime) {    
    GameManager::windowHasFocus = m_window->hasFocus();

    m_sceneMousePos = (sf::Vector2f)sf::Mouse::getPosition(*m_window) - m_swindowpos;
    m_sceneMousePos -= sf::Vector2f(10, 30);
    sf::Vector2f worldMPos = Rendering::frame.mapPixelToCoords((sf::Vector2i)m_sceneMousePos);
    GameManager::mousePos = worldMPos;

    // Process events
    sf::Event event;
    while (m_window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*m_window, event);
        auto& io = ImGui::GetIO();

        // Close Window
        if (event.type == sf::Event::Closed) {
            m_window->close();
        }

        // Keyboard and Mouse input
        if (event.type == sf::Event::KeyPressed) {
            GameManager::lastKeyboardInput = event.key.code;
            m_keypresses[event.key.code] = true;
            GameManager::keypresses[event.key.code] = true;
        }
        if (event.type == sf::Event::KeyReleased) {
            m_keypresses[event.key.code] = false;
            GameManager::keypresses[event.key.code] = false;
        }


        // Copy/Paste GameObject
        if (m_keypresses[sf::Keyboard::LControl] && m_keypresses[sf::Keyboard::C]) {
            if (m_viewObject != nullptr) {
                m_copyObject = m_viewObject;
            }
        }
        
        if (m_keypresses[sf::Keyboard::LControl] && m_keypresses[sf::Keyboard::V]) {
            if (m_copyObject != nullptr) {
                GameObject* copy = (GameObject*)CopyTree(m_copyObject);
                copy->SetPosition(worldMPos);

                m_keypresses[sf::Keyboard::V] = false;
            }
        }

        // Yes I know this is a duplicate
        // Better organization or something something
        if (event.type == sf::Event::MouseButtonPressed) {
            GameManager::lastMouseInput = event.mouseButton.button;
        } else GameManager::lastMouseInput = sf::Mouse::Button(-1);


        // Mouse Button Pressed
        if (event.type == sf::Event::MouseButtonPressed) {
            // If left mouse buttton is pressed, and we are on the Scene window
            if (event.mouseButton.button == 0 && m_swindowFocused) {
                // Editing Game
                if (!GameManager::isPlayingGame) {
                    // Check if we clicked on a Gameobject
                    // If so, set as selected
                    bool selectedSomething = false;
                    for (Entity* e : GameManager::Entities) {
                        sf::Vector2f size = e->hitbox[2] - e->hitbox[0];
                        sf::FloatRect rect(e->hitbox[0], size);

                        if (rect.contains(worldMPos)) {
                            m_currentSelectedObject = e;
                            selectedSomething = true;
                            break;
                        }
                    }
                    for (TextObj* text : GameManager::TextObjects) {
                        if (text->text.getGlobalBounds().contains(worldMPos)) {
                            m_currentSelectedObject = text;
                            selectedSomething = true;
                            break;
                        }
                    }
                    
                    if (!selectedSomething) {
                        // Clear selected object
                        m_currentSelectedObject = nullptr;
                    }
                }
            }

            // Right Mouse
            else if (event.mouseButton.button == 1 && m_sceneMouseCapture) {
                for (Entity* e : GameManager::Entities) {
                    sf::Vector2f size = e->hitbox[2] - e->hitbox[0];
                    sf::FloatRect rect(e->hitbox[0], size);
                    
                    if (rect.contains(worldMPos)) {
                        m_viewObject = e;
                    }
                }

                for (TextObj* text : GameManager::TextObjects) {
                    if (text->text.getGlobalBounds().contains(worldMPos)) {
                        m_viewObject = text;
                    }
                }
            }
        
            // Middle
            else if (event.mouseButton.button == 2 && m_sceneMouseCapture) {
                m_mousePos = sf::Mouse::getPosition(*m_window);
                m_cameraMove = true;
            }
        }
        
        // Mouse button released
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == 2) {
                // Turn off camera move 
                m_cameraMove = false;
                m_currentSelectedObject = nullptr;
            }
        }

        // Mouse moved
        if (event.type == sf::Event::MouseMoved) {
            // Left Mouse
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (m_currentSelectedObject != nullptr) {
                    if (m_currentSelectedObject->type == ENTITY) {
                        Entity* e = (Entity*)m_currentSelectedObject;
                        if (e->editHitbox) {
                            for (int i = 0; i < 4; i++) {
                                sf::CircleShape point(15);
                                point.setPosition(e->hitbox[i]);
                                point.setOrigin(15, 15);
                                
                                if (point.getGlobalBounds().contains(worldMPos)) {
                                    e->hitbox[i] = worldMPos;
                                    if (i == 0) {
                                        e->hitbox[4] = worldMPos;
                                    }
                                }
                            }
                        }
                        else {
                            e->SetPosition(worldMPos);
                        }
                    }
                    else {
                        m_currentSelectedObject->SetPosition(worldMPos);
                    }
                }
            }
        
            // Middle Mouse 
            if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
                if (m_cameraMove) {
                    sf::Vector2i mpos = sf::Mouse::getPosition(*m_window);
                    sf::Vector2i delta = mpos - m_mousePos;
                    delta.x *= -1.2f;
                    delta.y *= -1.2f;
                    m_camera->move(delta.x, delta.y);
                    Rendering::frame.setView(*m_camera);
                    m_mousePos = mpos;
                }
            }
        }

        // Mouse Wheel
        if (event.type == sf::Event::MouseWheelMoved) {
            if (!GameManager::isPlayingGame && m_sceneMouseCapture) {
                // Mouse wheel moved forwards
                if (event.mouseWheel.delta == 1) {
                    m_camera->zoom(0.8);
                    m_cameraZoom *= 1.2;
                }

                // Mouse wheel moved back
                else if (event.mouseWheel.delta == -1) {
                    m_camera->zoom(1.2);
                    m_cameraZoom *= 0.8;
                }

                Rendering::frame.setView(*m_camera);
            }
        }
    }


    // Keyboard Camera Movement
    sf::Vector2f camCenter = m_camera->getCenter();
    if (m_swindowFocused && !GameManager::isPlayingGame && GameManager::windowHasFocus) {
        float camSpeed = 10.f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            camSpeed = 50.f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            m_camera->setCenter(camCenter.x, camCenter.y - camSpeed);
            Rendering::frame.setView(*m_camera);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_camera->setCenter(camCenter.x, camCenter.y + camSpeed);
            Rendering::frame.setView(*m_camera);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            m_camera->setCenter(camCenter.x - camSpeed, camCenter.y);
            Rendering::frame.setView(*m_camera);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            m_camera->setCenter(camCenter.x + camSpeed, camCenter.y);
            Rendering::frame.setView(*m_camera);
        }
    }

    // Manage Sounds
    Sound::clean_sounds();
    Sound::clean_music();
}

// Physics and Lua scripting
void EditorInstance::FixedUpdate(sf::Time deltaTime) {
    m_lastFixedUpdate += deltaTime;

    if (m_lastFixedUpdate > m_TimePerFrame) {
        m_lastFixedUpdate -= m_TimePerFrame;

        // Run Lua Scripts
        if (GameManager::isPlayingGame) {
            Lua::RunLuaUpdates();
        }

        // Player position update
        if (GameManager::player != nullptr && GameManager::isPlayingGame) {
            GameManager::player->SetPosition(GameManager::player->position);

            if (GameManager::camera != nullptr) {
                // m_window->setView(*GameManager::camera->view);
                Rendering::frame.setView(*GameManager::camera->view);
            }
            else {
                // m_window->setView(*m_camera);
                Rendering::frame.setView(*m_camera);
            }
        }

        // Physics
        if (GameManager::isPlayingGame) {            
            for (Entity* e : GameManager::Entities) {
                if (!e->physicsObject) continue;

                // Reset
                e->acceleration = sf::Vector2f(0,0);
                e->grounded = false;

                // Incase of collisions
                sf::Vector2f prevPos = e->position;

                // Gravity
                e->acceleration = sf::Vector2f(0, GameManager::gravity);

                // Apply Changes
                e->velocity += e->acceleration;
                e->SetPosition(e->position + e->velocity);

                if (GameManager::checkCollisionSide(*e, "BOTTOM")) {
                    e->grounded = true;
                    e->velocity.y = 0;
                    e->SetPosition(prevPos);
                }
            }
        }
    }
    else {
        // Return so we aren't holding up the rest of the game
        return;
    }
}

/**
 * @brief Create and ImGui Tree from a GameObject and its children objects
 * 
 * @param obj The starting GameObject node
 */
void EditorInstance::DisplayEntityTree(GameObject* obj) {
    auto normalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    auto parentFlags =  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // Entity has no children (Stop Recursion)
    if (obj->children.size() == 0) {
        
        // DEBUG
        ImGui::SetNextItemOpen(true); // Always display full tree

        // Cast to display names correctly
        if (obj->type == ENTITY) {
            Entity* e = (Entity*)obj;
            ImGui::TreeNodeEx(e->name.data(), normalFlags);
        }
        else if (obj->type == TEXT) {
            TextObj* text = (TextObj*)obj;
            ImGui::TreeNodeEx(text->name.data(), normalFlags);
        }
        else if (obj->type == CAMERA) {
            Camera* camera = (Camera*)obj;
            ImGui::TreeNodeEx(camera->name.data(), normalFlags);
        }
        
        
        if (ImGui::IsItemClicked(1)) {
            m_viewObject = obj;
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("ENTITY_LIST_ID", &obj->ID, sizeof(unsigned int));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_LIST_ID")) {
                // Get Entity
                unsigned int sourceID = *(unsigned int*)payload->Data;
                GameObject* sourceObject = GameManager::FindObjectByID(sourceID);

                // Remove entity from current parent if it has one
                if (sourceObject->parent != nullptr) {
                    for (size_t i = 0; i < sourceObject->parent->children.size(); i++) {
                        if (sourceObject->parent->children[i]->ID == sourceObject->ID) {
                            sourceObject->parent->children.erase(sourceObject->parent->children.begin() + i);
                        }
                    }
                }

                // Add Entity to new parent
                obj->children.push_back(sourceObject);
                sourceObject->parent = obj;
            }
            ImGui::EndDragDropTarget();
        }
    }

    // Entity has children
    else {
        // DEBUG
        ImGui::SetNextItemOpen(true); // Always display full tree
        
        bool is_open;
        
        // Cast to display names correctly
        if (obj->type == ENTITY) {
            Entity* e = (Entity*)obj;
            is_open = ImGui::TreeNodeEx(e->name.data(), parentFlags);
        }
        else if (obj->type == TEXT) {
            TextObj* text = (TextObj*)obj;
            is_open = ImGui::TreeNodeEx(text->name.data(), parentFlags);
        }
        else if (obj->type == CAMERA) {
            Camera* camera = (Camera*)obj;
            is_open = ImGui::TreeNodeEx(camera->name.data(), parentFlags);
        }
        
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("ENTITY_LIST_ID", &obj->ID, sizeof(unsigned int));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_LIST_ID")) {
                // Get Entity
                unsigned int sourceID = *(unsigned int*)payload->Data;
                GameObject* sourceObject = GameManager::FindObjectByID(sourceID);

                // Remove entity from current parent if it has one
                if (sourceObject->parent != nullptr) {
                    for (size_t i = 0; i < sourceObject->parent->children.size(); i++) {
                        if (sourceObject->parent->children[i]->ID == sourceObject->ID) {
                            sourceObject->parent->children.erase(sourceObject->parent->children.begin() + i);
                            break;
                        }
                    }
                }

                // Add Entity to new parent
                obj->children.push_back(sourceObject);
                sourceObject->parent = obj;
            }
            ImGui::EndDragDropTarget();
        }
        
        if (ImGui::IsItemClicked(1)) {
            m_viewObject = obj;
        }
        if (is_open) {
            ImGui::Indent(4);
            
            for (GameObject* child : obj->children) {
                DisplayEntityTree(child);
            }

            ImGui::TreePop();
            ImGui::Unindent(4);
        }
    }
}

/**
 * @brief Copies a GameObject along with its Entity Tree
 * 
 * @param obj The GameObject to copy
 * @return void* The copied object. Cast according to the type of object passed in.
 */
void* EditorInstance::CopyTree(GameObject* obj) {
    if (obj->type == ENTITY) {
        Entity* copy = new Entity();
        Entity* e = (Entity*)obj;
        copy->Overwrite(*e);
        copy->name = e->name;

        for (GameObject* child : obj->children) {
            GameObject* childCopy = (GameObject*)CopyTree(child);
            
            sf::Vector2f delta = child->position - obj->position;
            childCopy->position = copy->position + delta;
            
            copy->children.push_back(childCopy);
            childCopy->parent = copy;
        }

        return copy;
    }
    else if (obj->type == TEXT) {
        TextObj* copy = new TextObj();
        copy->Overwrite(*(TextObj*)obj);
        copy->name = obj->name;

        for (GameObject* child : obj->children) {
            GameObject* childCopy = (GameObject*)CopyTree(child);
            
            sf::Vector2f delta = child->position - obj->position;
            childCopy->position = copy->position + delta;
            
            copy->children.push_back(childCopy);
            childCopy->parent = copy;
        }

        return copy;
    }
    else if (obj->type == CAMERA) {
        Camera* copy = new Camera();
        copy->Overwrite(*(Camera*)obj);

        for (GameObject* child : obj->children) {
            GameObject* childCopy = (GameObject*)CopyTree(child);
            
            sf::Vector2f delta = child->position - obj->position;
            childCopy->position = copy->position + delta;
            
            copy->children.push_back(childCopy);
            childCopy->parent = copy;
        }

        return copy;
    }

    return nullptr;
}

/**
 * @brief Prints the details of a sf::Vector2f to cout
 * 
 * @param vec the vector to print
 */
void EditorInstance::PrintVector(sf::Vector2f vec) {
    std::cout << "(" << vec.x << "," << vec.y << ")\n";
}

/**
 * @brief Sets up ImGui styling (font and colors)
 */
void EditorInstance::SetImGuiStyle() {
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

    ImGui::SFML::UpdateFontTexture();
}
