#include "EditorInstance.hpp"

#include <iostream>
#include <filesystem>
#include <nfd.h>

#include "Lua/LuaApi.hpp"
#include "Sound.hpp"
#include "Input.hpp"
#include "Rendering.hpp"

namespace Settings {
    int selectedResolution = 2;         // Default Resolution
    const int resolutionsCount = 6;     // Size of resolutions array below
    std::string EngineVersion = "";     // Engine Version
    
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
    if (!ImGui::SFML::Init(*m_window)) {
        std::cout << "[ERROR] Failed to initialize ImGui" << std::endl;
        return;
    }
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Load Editor Settings
    File::ConfigState settings = File::loadConfig();
    Settings::ApplySettings(settings);
    Settings::ChangeResolution(*m_window);

    // Set Camera
    this->m_camera = new sf::View(sf::Vector2f(0,0), sf::Vector2f(GameManager::screenWidth, GameManager::screenHeight));
    m_window->setView(*m_camera);
    this->m_cameraZoom = 1.0f;

    // Default Font
    GameManager::font.loadFromFile("arial.ttf");

    // Set Engine UI Variables
    this->m_showDockSpace = false;
    this->m_cameraMove = false;                                       // Is the Camera Moving?
    this->m_showEntityCreate = false;                                 // Entity Create Menu
    this->m_showMainMenu = true;                                      // General Main Menu
    this->m_showEntityList = false;                                   // List of Entities
    this->m_showConsole = false;                                      // Debug Console
    this->m_showSettingsMenu = false;                                 // Editor Settings

    this->m_failedMessage = "";                                       // What error to display

    // Create Entity Variables
    this->m_createPosition[0] = 0.f;                                  // Default Position
    this->m_createPosition[1] = 0.f;
    strcpy(this->m_createNameBuffer, "Entity");                       // Default Name
    strcpy(this->m_createImagePath, "");                              // Default Image Path
    this->m_createDimension[0] = 128;                                 // Default Entity Dimensions
    this->m_createDimension[1] = 128;

    // Create Text Variables
    this->m_createTextPosition[0] = 0.f;
    this->m_createTextPosition[1] = 0.f;
    strcpy(this->m_createTextName, "Text");
    strcpy(this->m_createTextDetails, "");
    this->m_createTextMode = 0;
    this->m_createTextEntityId = -1;

    // Setup Lua API
    Lua::RegisterFunctions();

    // Input
    Input::InitInputMaps();

    // Other
    this->m_lastFixedUpdate = sf::Time::Zero;
    this->m_frameLimit = 60;                                          // Change if necessary
    this->m_TimePerFrame = sf::seconds(1.f / m_frameLimit);
    this->m_window->setFramerateLimit(165);
    
    // Create RenderLayers
    Rendering::CreateLayers(5);
}

EditorInstance::~EditorInstance() {
    delete m_window;
    delete m_camera;
}

// Main Game Loop
void EditorInstance::Run() {
    // Origin
    sf::CircleShape originDot(5);
    originDot.setFillColor(sf::Color::White);
    originDot.setPosition(0.0f, 0.0f);

    // DEBUG
    std::string demoPath = "C:/Users/Jackson/Desktop/BaconEngine Projects/Testing/Game.json";
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

        m_window->clear(sf::Color(40, 40, 40));
        if (!GameManager::isPlayingGame) {
            m_window->draw(originDot);
        }
        
        Rendering::DrawGameObjects(*m_window);
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
    if (m_showDockSpace) ImGui::DockSpaceOverViewport();

    // Top menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (GameManager::isPlayingGame) {
                ImGui::BeginDisabled();
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

            // Popups
            //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            //ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Create")) {
                ImGui::MenuItem("Entity", NULL, &m_showEntityCreate);
                ImGui::MenuItem("Text", NULL, &m_showTextCreate);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Object List", NULL, &m_showEntityList));
            if (ImGui::MenuItem("Game Details", NULL, &m_showMainMenu));
            if (ImGui::MenuItem("Console", NULL, &m_showConsole));
            if (ImGui::MenuItem("Menu Docking", NULL, &m_showDockSpace));
            if (ImGui::MenuItem("Editor Settings", NULL, &m_showSettingsMenu));
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
                    m_cameraPos = m_camera->getCenter();
                    m_cameraSize = m_camera->getSize();
                    GameManager::ConsoleWrite("[PLAYER] Editor data saved. Starting game...");
                    GameManager::isPlayingGame = true;
                }
            }
            else {
                if (ImGui::Button("End Game")) {
                    GameManager::RestoreEditorState(*m_window, m_projectTitle);
                    m_camera->setCenter(m_cameraPos);
                    m_camera->setSize(m_cameraSize);
                    m_window->setView(*m_camera);
                    Sound::stop_sounds();
                    GameManager::ConsoleWrite("[PLAYER] Game Ended. Editor data restored.");
                    GameManager::isPlayingGame = false;
                }
            }
            ImGui::Separator();
            ImGui::BeginTabBar("EngineItems");
                if (ImGui::BeginTabItem("Camera")) {
                    float cameraPos[] = {m_camera->getCenter().x, m_camera->getCenter().y};
                    ImGui::Text("Position");
                    if (ImGui::InputFloat2("##", cameraPos)) {
                        m_camera->setCenter(sf::Vector2f(cameraPos[0], cameraPos[1]));
                        m_window->setView(*m_camera);
                    }

                    float oldZoom = m_cameraZoom;
                    ImGui::Text("Zoom");
                    if (ImGui::InputFloat("##", &m_cameraZoom)) {
                        float zoomFactor = oldZoom / m_cameraZoom;
                        m_camera->zoom(zoomFactor);
                        m_window->setView(*m_camera);
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Game Settings")) {
                    ImGui::InputFloat("Gravity", &GameManager::gravity);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Testing")) {
                    ImGui::InputInt("Test ID", &m_DebugIntInput);

                    if (ImGui::Button("Add Entity")) {
                        Entity* parent = GameManager::FindEntityByID(m_DebugIntInput);
                        Entity* newChild = new Entity();
                        newChild->parent = parent;
                        parent->children.push_back(newChild);
                    }

                    if (ImGui::Button("Testing")) {
                        for (GameObject* obj : GameManager::GameObjects) {
                            if (obj->parent != nullptr) {
                                std::cout << obj->parent->ID << ": " << obj->parent->name << std::endl;
                            }
                        }
                    }

                    if (ImGui::Button("Sort")) {
                        GameManager::SortObjectsByID();
                    }
                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
        ImGui::End();
    }

    // Entity detail menus
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        Entity* e = GameManager::Entities[i];

        if (!e->showDetails) continue;
        std::string name = "Details (ID: " + std::to_string(e->ID) + ")";
        ImGui::Begin(name.c_str(), &(e->showDetails));
            char nameBuff[256];
            strcpy(nameBuff, e->name.data());
            if (ImGui::InputText("Name", nameBuff, 256)) {
                e->name = nameBuff;
            }

            char typeBuff[256];
            strcpy(typeBuff, e->entity_type.data());
            if (ImGui::InputText("Type", typeBuff, 256)) {
                e->entity_type = typeBuff;
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
                char textBuff[256];
                std::string relpath = std::filesystem::relative(e->texturePath.data(), GameManager::entryPoint).generic_string();
                strcpy(textBuff, relpath.data());
                ImGui::InputText("Texture", textBuff, 256);
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
                    e->UpdateRect();
                    e->SetSprite(e->texturePath);
                }
                if (ImGui::InputInt("Height", &e->height)) {
                    e->UpdateRect();
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
                    e->UpdateRect();
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

                if (ImGui::Button("Copy")) {
                    m_copyObject = e;
                }

                if (ImGui::Button("Delete")) {
                    delete(GameManager::Entities[i]);
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
                    if (ImGui::InputFloat("Hitbox Size", &e->hitboxSize)) {
                        e->UpdateRect();
                    }
                    ImGui::Checkbox("Show Hitbox", &e->showHitbox);
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
                    ImGui::Checkbox("Grounded", &e->grounded);
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

                for (auto it = e->entity_variables.begin(); it != e->entity_variables.end(); it++) {
                    std::string key = it->second;
                    if (e->entity_numbers.find(key) != e->entity_numbers.end()) {
                        ImGui::InputDouble(key.c_str(), &e->entity_numbers[key]);
                        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                        
                        ImGui::PushID(key.c_str());
                        if (ImGui::Button("X")) {
                            e->entity_numbers.erase(key);
                            e->entity_variables.erase(it);
                        }
                        ImGui::PopID();
                    }
                    else if (e->entity_strings.find(key) != e->entity_strings.end()) {
                        char textBuff[256];
                        strcpy(textBuff, e->entity_strings[key].data());
                        if (ImGui::InputText(key.c_str(), textBuff, 256)) {
                            e->entity_strings[key] = textBuff;
                        }
                        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                        
                        ImGui::PushID(key.c_str());
                        if (ImGui::Button("X")) {
                            e->entity_strings.erase(key);
                            e->entity_variables.erase(it);
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
                    ImGui::InputText("Name", m_AddVariableName, 256);
                    
                    ImGui::RadioButton("Number", &m_AddVariableType, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("String", &m_AddVariableType, 1);

                    if (m_AddVariableType == 0) {
                        ImGui::InputDouble("Value", &m_AddVariableNumber);
                    }
                    else if (m_AddVariableType == 1) {
                        ImGui::InputText("Value", m_AddVariableString, 256);
                    }

                    if (ImGui::Button("Add")) {
                        int count = e->entity_variables.size();
                        e->entity_variables[count] = m_AddVariableName;
                        if (m_AddVariableType == 0) {
                            e->entity_numbers[m_AddVariableName] = m_AddVariableNumber;
                        }
                        else {
                            e->entity_strings[m_AddVariableName] = m_AddVariableString;
                        }
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
        ImGui::End();
    }

    // Show Text Detail
    for (size_t i = 0; i < GameManager::TextObjects.size(); i++) {
        TextObj* text = GameManager::TextObjects[i];
        if (!text->showDetails) continue;

        std::string name = "Text Details (ID: " + std::to_string(text->ID) + ")";
        ImGui::Begin(name.c_str(), &text->showDetails);
            ImGui::InputText("Name", text->name.data(), 256);

            int text_layer = text->layer;
            if (ImGui::InputInt("Layer", &text_layer)) {
                if (text_layer >= 0) Rendering::SwapLayer(text, text_layer);
                else text_layer = 0;
            }

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
            int buffer_size = 1024 * 16;
            char text_buffer[buffer_size];
            strcpy(text_buffer, textString.c_str()); 
            if (ImGui::InputTextMultiline("Text", text_buffer, buffer_size)) {
                text->text.setString(text_buffer);
            }

            if (ImGui::Button("Copy")) {
                m_copyObject = text;
            }

            if (ImGui::Button("Delete")) {
                free(GameManager::TextObjects[i]);
                GameManager::TextObjects.erase(GameManager::TextObjects.begin() + i);
            }
            
        ImGui::End();
    }

    // Create new Entity menu
    if (m_showEntityCreate) {
        ImGui::Begin("Create new entity", &m_showEntityCreate);
            // Displays
            ImGui::InputText("Name", m_createNameBuffer, 256);
            ImGui::InputFloat2("Position", m_createPosition);
            ImGui::InputText("Image Path", m_createImagePath, 256);
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
                entity->SetSprite(m_createImagePath);
                m_showEntityCreate = false; // Close Window
            }
        ImGui::End();
    }

    // Create new Text Object
    if (m_showTextCreate) {
        ImGui::Begin("Create Text Object", &m_showTextCreate);
            ImGui::InputText("Name", m_createTextName, 256);
            ImGui::InputFloat2("Position", m_createTextPosition);
            ImGui::InputText("Text", m_createTextDetails, 8*256);

            ImGui::RadioButton("Absolute", &m_createTextMode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Relative", &m_createTextMode, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Screen", &m_createTextMode, 2);

            if (m_createTextMode == 1) {
                ImGui::InputInt("Entity ID", &m_createTextEntityId);
            }

            if (ImGui::Button("Create")) {
                TextObj* text = new TextObj();

                text->name = m_createTextName;
                text->position = sf::Vector2f(m_createTextPosition[0], m_createTextPosition[1]);
                text->text.setString(m_createTextDetails);

                m_showTextCreate = false; // Close Window
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
                GameManager::ConsoleLog.clear();
            }
            ImGui::Separator();
            ImGui::TextUnformatted(GameManager::ConsoleLog.begin(), GameManager::ConsoleLog.end());
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
        }
        if (event.type == sf::Event::KeyReleased) {
            m_keypresses[event.key.code] = false;
        }


        // Paste GameObject
        if (m_keypresses[sf::Keyboard::LControl] && m_keypresses[sf::Keyboard::V]) {
            if (m_copyObject != nullptr) {
                GameObject* copy = (GameObject*)CopyTree(m_copyObject);
                sf::Vector2f converted = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
                copy->SetPosition(converted);

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
            // If left mouse buttton is pressed, and we aren't clicking on an ImGui window
            if (event.mouseButton.button == 0 && !io.WantCaptureMouse) {
                // Editing Game
                if (!GameManager::isPlayingGame) {
                    sf::Vector2f converted = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));

                    // Check if we clicked on a Gameobject
                    // If so, set as selected
                    bool selectedSomething = false;
                    for (Entity* e : GameManager::Entities) {
                        if (e->rect.contains(converted.x, converted.y)) {
                            m_currentSelectedObject = e;
                            selectedSomething = true;
                            break;
                        }
                    }
                    for (TextObj* text : GameManager::TextObjects) {
                        if (text->text.getGlobalBounds().contains(converted.x, converted.y)) {
                            m_currentSelectedObject = text;
                            selectedSomething = true;
                            break;
                        }
                    }
                    
                    if (!selectedSomething) {
                        // Camera drag move
                        // Get starting position as reference for movement
                        
                        //mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
                        m_mousePos = converted;
                        m_cameraMove = true;
                        m_currentSelectedObject = nullptr;
                    }
                }
            }

            // Right Mouse
            else if (event.mouseButton.button == 1) {
                sf::Vector2f converted = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
                
                for (Entity* e : GameManager::Entities) {
                    if (e->rect.contains(converted.x, converted.y)) {
                        // Toggle details menu visability
                        e->showDetails = !e->showDetails;
                    }
                }

                for (TextObj* text : GameManager::TextObjects) {
                    if (text->text.getGlobalBounds().contains(converted.x, converted.y)) {
                        text->showDetails = !text->showDetails;
                    }
                }
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            // Turn off camera move 
            m_cameraMove = false;
            m_currentSelectedObject = nullptr;
        }

        if (event.type == sf::Event::MouseMoved) {
            if (m_cameraMove) {
                // Calculate change
                sf::Vector2f currentPos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
                sf::Vector2f change = (sf::Vector2f)(m_mousePos - currentPos);

                // Update position
                m_camera->setCenter(m_camera->getCenter() + change);
                m_window->setView(*m_camera);
            } 
            // Move Entity
            else {
                if (m_currentSelectedObject != nullptr && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
                    m_currentSelectedObject->SetPosition(mousePos);
                }
            }
        }

        // Mouse Wheel
        if (event.type == sf::Event::MouseWheelMoved) {
            if (!GameManager::isPlayingGame && !io.WantCaptureMouse) {
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

                m_window->setView(*m_camera);
            }
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

        // Get info
        GameManager::mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));

        // Run Lua Scripts
        if (GameManager::isPlayingGame) {
            Lua::RunLuaUpdates();
        }

        // Player position update
        if (GameManager::player != nullptr && GameManager::isPlayingGame) {
            GameManager::player->SetPosition(GameManager::player->position);
            m_camera->setCenter(GameManager::player->position);
            m_window->setView(*m_camera);
        }

        // Physics
        if (GameManager::isPlayingGame) {            
            for (Entity* e : GameManager::Entities) {
                if (!e->physicsObject) continue;

                // Gravity and collisions
                bool collision = false;
                bool bottomCollision = false;
                bool topCollision = false;
                for (Entity* other : GameManager::Entities) {
                    if (e->ID == other->ID) continue;
                    if (!other->isSolid) continue;

                    // Have we hit the ground?
                    if (e->bottomRect.intersects(other->rect)) {
                        collision = true;
                        bottomCollision = true;
                    }

                    // Did the top of our entity hit something?
                    if (e->topRect.intersects(other->rect)) {
                        collision = true;
                        topCollision = true;
                    }
                }

                // We hit something
                if (collision) {
                    // Landed on ground
                    if (bottomCollision) {
                        // weren't already on ground
                        if (!e->grounded) {
                            // Stop Moving
                            e->grounded = true;
                            e->acceleration.y = 0;
                            e->velocity.y = 0;
                        }
                        // Else, do nothing
                    }

                    // We bonked our head
                    if (topCollision) {
                        if (!bottomCollision) {
                            e->velocity.y = 1; // DEBUG
                        }
                    }
                }
                // Free Fall
                else {
                    e->acceleration.y = GameManager::gravity;
                    e->grounded = false;
                }

                // Apply Velocity
                e->velocity += e->acceleration;
                e->SetPosition(e->position + e->velocity);
            }
        }
    }
    else {
        // Return so we aren't holding up the rest of the game
        return;
    }
}

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
        
        
        if (ImGui::IsItemClicked(1)) {
            obj->showDetails = !obj->showDetails;
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
            obj->showDetails = !obj->showDetails;
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

    return nullptr;
}
