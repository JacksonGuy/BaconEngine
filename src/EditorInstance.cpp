#include "EditorInstance.hpp"

#include <iostream>
#include "LuaApi.hpp"

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
    void ApplySettings(ConfigState state) {
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
    this->projectTitle = "Untitled Project";
    this->loadedProject = false;

    this->window = new sf::RenderWindow(sf::VideoMode(GameManager::screenWidth, GameManager::screenHeight),
        "Bacon - " + this->projectTitle, sf::Style::Default);
    
    // Create ImGui Context
    if (!ImGui::SFML::Init(*window)) {
        std::cout << "[ERROR] Failed to initialize ImGui" << std::endl;
        return;
    }

    // Load Editor Settings
    ConfigState settings = loadConfig();
    Settings::ApplySettings(settings);
    Settings::ChangeResolution(*window);

    // Set Camera
    this->camera = new sf::View(sf::Vector2f(0,0), sf::Vector2f(GameManager::screenWidth, GameManager::screenHeight));
    window->setView(*camera);
    this->cameraZoom = 1.0f;

    // Default Font
    GameManager::font.loadFromFile("./assets/fonts/arial.ttf");

    // Set Engine UI Variables
    this->cameraMove = false;                                       // Is the Camera Moving?
    this->showEntityCreate = false;                                 // Entity Create Menu
    this->showMainMenu = true;                                      // General Main Menu
    this->showEntityList = false;                                   // List of Entities
    this->showConsole = false;                                      // Debug Console
    this->showSettingsMenu = false;                                 // Editor Settings

    this->showFailedPopup = false;                                  // Used for displaying errors with saving/loading
    this->failedMessage = "";                                       // What error to display

    // Save and Load project Variables
    this->showLoadPopup = false;                                    // Load Project Popup
    strcpy(this->loadProjectName, "");                              // Placeholder for Project Name
    this->showSaveAsPopup = false;                                  // Save Project As Popup
    strcpy(this->saveAsProjectName, this->projectTitle.data());     // Placeholder for Project Name

    // Create Entity Variables
    this->createPosition[0] = 0.f;                                  // Default Position
    this->createPosition[1] = 0.f;
    strcpy(this->createNameBuffer, "Entity");                       // Default Name
    strcpy(this->createImagePath, "");                              // Default Image Path

    // Create Text Variables
    this->createTextPosition[0] = 0.f;
    this->createTextPosition[1] = 0.f;
    strcpy(this->createTextName, "Text");
    strcpy(this->createTextDetails, "");
    this->createTextMode = 0;
    this->createTextEntityId = -1;

    // Setup Lua API
    luaL_openlibs(GameManager::LuaState);
    lua_register(GameManager::LuaState, "ConsoleWrite", ConsoleWrite);
    lua_register(GameManager::LuaState, "set_variable", set_variable);
    lua_register(GameManager::LuaState, "get_variable", get_variable);
    lua_register(GameManager::LuaState, "set_text", set_text);
    lua_register(GameManager::LuaState, "get_input", get_input);
    lua_register(GameManager::LuaState, "get_mouse_input", get_mouse_input);
    lua_register(GameManager::LuaState, "get_position", get_position);
    lua_register(GameManager::LuaState, "set_position", set_position);
    lua_register(GameManager::LuaState, "check_collision", check_collision);
    lua_register(GameManager::LuaState, "check_collision_side", check_collision_side);
    lua_register(GameManager::LuaState, "get_velocity", get_velocity);
    lua_register(GameManager::LuaState, "set_velocity", set_velocity);
    lua_register(GameManager::LuaState, "get_acceleration", get_acceleration);
    lua_register(GameManager::LuaState, "get_grounded", get_grounded);

    // Other
    this->lastFixedUpdate = sf::Time::Zero;
    this->frameLimit = 60;                                          // Change if necessary
    this->TimePerFrame = sf::seconds(1.f / frameLimit);
    this->window->setFramerateLimit(165);

    // Keys
    for (int i = 0; i < 26; i++) {
        // I'm sorry Dennis Ritchie...
        char ch = 'A' + i;
        std::string index;
        index.push_back(ch);
        GameManager::key_map[index] = sf::Keyboard::Key(i);
    }
    GameManager::key_map["ESC"] = sf::Keyboard::Escape;
    GameManager::key_map["LCONTROL"] = sf::Keyboard::LControl;
    GameManager::key_map["LSHIFT"] = sf::Keyboard::LShift;
    GameManager::key_map["LALT"] = sf::Keyboard::LAlt;
    GameManager::key_map["RCONTROL"] = sf::Keyboard::RControl;
    GameManager::key_map["RSHIFT"] = sf::Keyboard::RShift;
    GameManager::key_map["RALT"] = sf::Keyboard::RAlt;
    GameManager::key_map["SPACE"] = sf::Keyboard::Space;
    GameManager::key_map["TAB"] = sf::Keyboard::Tab;
    GameManager::key_map["UP"] = sf::Keyboard::Up;
    GameManager::key_map["DOWN"] = sf::Keyboard::Down;
    GameManager::key_map["LEFT"] = sf::Keyboard::Left;
    GameManager::key_map["RIGHT"] = sf::Keyboard::Right;
    GameManager::key_map["LBRACKET"] = sf::Keyboard::LBracket;
    GameManager::key_map["RBRACKET"] = sf::Keyboard::RBracket;

    // for (auto it =  GameManager::key_map.begin(); it != GameManager::key_map.end(); it++) {
    //     std::cout << it->first << " = " << it->second << std::endl;
    // }

    GameManager::mouse_map["MOUSE_LEFT"] = sf::Mouse::Button::Left;
    GameManager::mouse_map["MOUSE_RIGHT"] = sf::Mouse::Button::Right;
    GameManager::mouse_map["MOUSE_MIDDLE"] = sf::Mouse::Button::Middle;
    GameManager::mouse_map["MOUSE_BUTTON_4"] = sf::Mouse::Button::XButton1;
    GameManager::mouse_map["MOUSE_BUTTON_5"] = sf::Mouse::Button::XButton2;
}

// Main Game Loop
void EditorInstance::Run() {
    // Origin
    sf::CircleShape originDot(5);
    originDot.setFillColor(sf::Color::White);
    originDot.setPosition(0.0f, 0.0f);

    // DEBUG
    load("Demo.json");

    while(window->isOpen()) {
        sf::Time deltaTime = clock.restart();

        this->Update(deltaTime);         // Input
        this->FixedUpdate(deltaTime);    // Physics
        this->DrawUI(deltaTime);         // ImGui Frames

        window->clear(sf::Color(40, 40, 40));
        if (!GameManager::isPlayingGame) {
            window->draw(originDot);
        }
        
        GameManager::DrawEntities(*this->window);
        GameManager::DrawText(*this->window);
        ImGui::SFML::Render(*this->window);
        this->window->display();
    }

    // Save editor settings before closing
    ConfigState finalSettings = {
        Settings::EngineVersion,
        Settings::GetResolutionString()
    };
    saveConfig(finalSettings);
    std::cout << "Saved Editor Settings" << std::endl;

    this->window->close();
    ImGui::SFML::Shutdown();
}

// Draw UI
void EditorInstance::DrawUI(sf::Time deltaTime) {
    float currentTime = deltaTime.asSeconds();
    float fps = 1.0f / currentTime;

    ImGui::SFML::Update(*window, deltaTime);

    // Top menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (GameManager::isPlayingGame) {
                ImGui::BeginDisabled();
            }

            if (ImGui::MenuItem("Save")) {
                if (!loadedProject) {
                    showSaveAsPopup = true;
                }
                else {
                    save(projectTitle);
                }
            }
            
            if (ImGui::MenuItem("Save as")) {
                showSaveAsPopup = true;
            }
            
            if (ImGui::MenuItem("Load")) {
                showLoadPopup = true;
            }

            if (GameManager::isPlayingGame) {
                ImGui::EndDisabled();
            }

            // Popups
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Create")) {
                ImGui::MenuItem("Entity", NULL, &showEntityCreate);
                ImGui::MenuItem("Text", NULL, &showTextCreate);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Object List", NULL, &showEntityList));
            if (ImGui::MenuItem("Game Details", NULL, &showMainMenu));
            if (ImGui::MenuItem("Console", NULL, &showConsole));
            if (ImGui::MenuItem("Editor Settings", NULL, &showSettingsMenu));
            ImGui::EndMenu();
        }

        if (showLoadPopup) {
            ImGui::OpenPopup("Load Project");
        }

        if (showSaveAsPopup) {
            ImGui::OpenPopup("Save As Project");
        }

        if (showFailedPopup) {
            ImGui::OpenPopup("Failed");
        }

        if (ImGui::BeginPopupModal("Load Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Load Project");
            ImGui::Separator();
            ImGui::InputText("##", loadProjectName, 64);
            if (ImGui::Button("Load")) {
                ImGui::CloseCurrentPopup();
                showLoadPopup = false;

                // Save Editor State in case of failure
                GameManager::SaveEditorState(*this->window);

                if (load(loadProjectName)) {
                    GameManager::ConsoleWrite("[DEBUG] Successfully Loaded Project");
                    loadedProject = true;
                    projectTitle = loadProjectName;
                    window->setTitle("Bacon - " + projectTitle);
                }
                else {
                    GameManager::RestoreEditorState(*this->window);
                    showFailedPopup = true;
                    failedMessage = "Error: Failed to load project (Project doesn't exist).";
                    showLoadPopup = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            if (ImGui::Button("Cancel")) {
                showLoadPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Save As Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Save Project");
            ImGui::Separator();
            ImGui::InputText("##", saveAsProjectName, 64);
            if (ImGui::Button("Save")) {
                ImGui::CloseCurrentPopup();
                showSaveAsPopup = false;

                save(saveAsProjectName);
                projectTitle = saveAsProjectName;
                loadedProject = true;
                window->setTitle("Bacon - " + projectTitle);
            }
            if (ImGui::Button("Cancel")) {
                showSaveAsPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text(failedMessage.c_str());
            if (ImGui::Button("OK")) {
                showFailedPopup = false;
                failedMessage = "";
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndMainMenuBar();
    }

    // General Menu
    if (showMainMenu) {
        ImGui::Begin("Game Details", &showMainMenu);
            ImGui::Text(Settings::EngineVersion.c_str());
            ImGui::Text( ("FPS: " + std::to_string((int)fps)).c_str() );
            if (!GameManager::isPlayingGame) {
                if (ImGui::Button("Play Game")) {
                    GameManager::SaveEditorState(*window);
                    GameManager::ConsoleWrite("[PLAYER] Editor data saved. Starting game...");

                    GameManager::isPlayingGame = true;
                }
            }
            else {
                if (ImGui::Button("End Game")) {
                    GameManager::RestoreEditorState(*window);
                    GameManager::ConsoleWrite("[PLAYER] Game Ended. Editor data restored.");
                    GameManager::isPlayingGame = false;
                }
            }
            ImGui::Separator();
            ImGui::BeginTabBar("EngineItems");
                if (ImGui::BeginTabItem("Camera")) {
                    float cameraPos[] = {camera->getCenter().x, camera->getCenter().y};
                    ImGui::Text("Position");
                    if (ImGui::InputFloat2("##", cameraPos)) {
                        camera->setCenter(sf::Vector2f(cameraPos[0], cameraPos[1]));
                        window->setView(*camera);
                    }

                    float oldZoom = cameraZoom;
                    ImGui::Text("Zoom");
                    if (ImGui::InputFloat("##", &cameraZoom)) {
                        float zoomFactor = oldZoom / cameraZoom;
                        camera->zoom(zoomFactor);
                        window->setView(*camera);
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Game Settings")) {
                    ImGui::InputFloat("Gravity", &GameManager::gravity);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Testing")) {
                    if (ImGui::Button("This is a button")) {
                        sf::View cam = window->getView();
                        sf::Vector2f center = cam.getCenter();
                        sf::Vector2u win_size = window->getSize();
                        sf::Vector2f corner = sf::Vector2f(center.x - (win_size.x/2), center.y - (win_size.y/2)); 

                        std::string str = "[DEBUG] Top Left: (" + std::to_string(corner.x)
                            + ", " + std::to_string(corner.y) + ")";
                        GameManager::ConsoleWrite(str);
                    }
                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
        ImGui::End();
    }

    // Entity detail menus
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        Entity* e = GameManager::Entities[i];

        if (!e->showDetailMenu) continue;
        std::string name = "Details (ID: " + std::to_string(e->ID) + ")";
        ImGui::Begin(name.c_str(), &(e->showDetailMenu));
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
                strcpy(textBuff, e->texturePath.data());
                ImGui::InputText("Texture", textBuff, 256);
                if (ImGui::Button("Change Texture")) {
                    e->SetSprite(textBuff);
                }
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

                ImGui::Text("Scripts");
                auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                for (size_t i = 0; i < e->lua_scripts.size(); i++) {
                    ScriptItem script = e->lua_scripts[i];
                    ImGui::TreeNodeEx(script.path.data(), flags);
                    if (ImGui::IsItemClicked()) {
                        script.showDetails = !script.showDetails;
                    }
                    
                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    if (ImGui::Button("X")) {
                        e->lua_scripts.erase(e->lua_scripts.begin() + i);
                    }
                }

                if (ImGui::Button("Add Lua Script")) {
                    AddAttributeEntity = e;
                    ImGui::OpenPopup("AddScriptEntity");
                }

                if (ImGui::BeginPopupModal("AddScriptEntity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Add Script");
                    ImGui::Separator();
                    ImGui::InputText("Path", AddScriptName, 256);

                    if (ImGui::Button("Add")) {
                        ScriptItem script;
                        script.path = AddScriptName;
                        script.showDetails = false;
                        AddAttributeEntity->lua_scripts.push_back(script);

                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::Separator();

                ImGui::Text("Custom Variables");

                for (auto it = e->entity_variables.begin(); it != e->entity_variables.end(); it++) {
                    std::string key = it->second;
                    if (e->entity_numbers.find(key) != e->entity_numbers.end()) {
                        ImGui::InputDouble(key.c_str(), &e->entity_numbers[key]);
                        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                        if (ImGui::Button("X")) {
                            e->entity_numbers.erase(key);
                            e->entity_variables.erase(it);
                        }
                    }
                    else if (e->entity_strings.find(key) != e->entity_strings.end()) {
                        char textBuff[256];
                        strcpy(textBuff, e->entity_strings[key].data());
                        if (ImGui::InputText(key.c_str(), textBuff, 256)) {
                            e->entity_strings[key] = textBuff;
                        }
                        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                        if (ImGui::Button("X")) {
                            e->entity_strings.erase(key);
                            e->entity_variables.erase(it);
                        }
                    }
                }

                if (ImGui::Button("Add variable")) {
                    AddAttributeEntity = e;
                    ImGui::OpenPopup("AddVariableEntity");
                }

                if (ImGui::BeginPopupModal("AddVariableEntity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Add Variable");
                    ImGui::Separator();
                    ImGui::InputText("Name", AddVariableName, 256);
                    
                    ImGui::RadioButton("Number", &AddVariableType, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("String", &AddVariableType, 1);

                    if (AddVariableType == 0) {
                        ImGui::InputDouble("Value", &AddVariableNumber);
                    }
                    else if (AddVariableType == 1) {
                        ImGui::InputText("Value", AddVariableString, 256);
                    }

                    if (ImGui::Button("Add")) {
                        int count = e->entity_variables.size();
                        e->entity_variables[count] = AddVariableName;
                        if (AddVariableType == 0) {
                            e->entity_numbers[AddVariableName] = AddVariableNumber;
                        }
                        else {
                            e->entity_strings[AddVariableName] = AddVariableString;
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::Separator();

                if (ImGui::Button("Delete")) {
                    free(GameManager::Entities[i]);
                    GameManager::Entities.erase(GameManager::Entities.begin() + i);
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
                    ImGui::InputFloat("Hitbox Size", &e->hitboxSize);
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

            int text_mode = 0;
            if (text->mode == Absolute) text_mode = 0;
            else if (text->mode == Relative) text_mode = 1;
            else if (text->mode == Screen) text_mode = 2;

            if (ImGui::RadioButton("Absolute", &text_mode, 0)) {
                text->mode = Absolute;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Relative", &text_mode, 1)) {
                text->mode = Relative;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Screen", &text_mode, 2)) {
                text->mode = Screen;
            }

            if (text->mode == Relative) {
                int entity_id = text->target->ID;
                if (ImGui::InputInt("Entity ID", &entity_id)) {
                    Entity* e = GameManager::FindEntityByID(entity_id);
                    if (e != nullptr) {
                        text->target = e;
                    }
                }
            }

            ImGui::Separator();

            std::string textString = text->text.getString();
            int buffer_size = 1024 * 16;
            char text_buffer[buffer_size];
            strcpy(text_buffer, textString.c_str()); 
            if (ImGui::InputTextMultiline("Text", text_buffer, buffer_size)) {
                text->text.setString(text_buffer);
            }

            if (ImGui::Button("Delete")) {
                free(GameManager::TextObjects[i]);
                GameManager::TextObjects.erase(GameManager::TextObjects.begin() + i);
            }
            
        ImGui::End();
    }

    // Create new Entity menu
    if (showEntityCreate) {
        ImGui::Begin("Create new entity", &showEntityCreate);
            // Displays
            ImGui::InputText("Name", createNameBuffer, 256);
            ImGui::InputFloat2("Position", createPosition);
            ImGui::InputText("Image Path", createImagePath, 256);

            // Create Button
            if (ImGui::Button("Create")) {
                Entity* entity = new Entity({createPosition[0], createPosition[1]});
                entity->name = createNameBuffer;
                entity->SetSprite(createImagePath);
            }
        ImGui::End();
    }

    // Create new Text Object
    if (showTextCreate) {
        ImGui::Begin("Create Text Object", &showTextCreate);
            ImGui::InputText("Name", createTextName, 256);
            ImGui::InputFloat2("Position", createTextPosition);
            ImGui::InputText("Text", createTextDetails, 8*256);

            ImGui::RadioButton("Absolute", &createTextMode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Relative", &createTextMode, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Screen", &createTextMode, 2);

            if (createTextMode == 1) {
                ImGui::InputInt("Entity ID", &createTextEntityId);
            }

            if (ImGui::Button("Create")) {
                TextObj* text = new TextObj();

                text->name = createTextName;
                text->position = sf::Vector2f(createTextPosition[0], createTextPosition[1]);
                text->text.setString(createTextDetails);

                switch (createTextMode) {
                    case 0: 
                        text->mode = Absolute;
                        break;
                    case 1: 
                        text->mode = Relative;
                        break;
                    case 2: 
                        text->mode = Screen;
                        break;
                }

                if (text->mode == Relative) {
                    text->target = GameManager::FindEntityByID(createTextEntityId);
                }
                else{
                    text->target = nullptr;
                }
            }
        ImGui::End();
    }

    // List all Objects
    if (showEntityList) {
        ImGui::Begin("Objects", &showEntityList);
            auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            
            ImGui::BeginTabBar("Lists");
                if (ImGui::BeginTabItem("Entities")) {
                    for (Entity* e : GameManager::Entities) {
                        ImGui::TreeNodeEx(e->name.data(), flags);
                        if (ImGui::IsItemClicked()) {
                            e->showDetailMenu = !e->showDetailMenu;
                        }
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Text")) {
                    for (TextObj* obj : GameManager::TextObjects) {
                        ImGui::TreeNodeEx(obj->name.data(), flags);
                        if (ImGui::IsItemClicked()) {
                            obj->showDetails = !obj->showDetails;
                        }
                    }
                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
            
        ImGui::End();
    }

    // Show Debug Console
    if (showConsole) {
        ImGui::Begin("Console", &showConsole);
            if (ImGui::Button("Clear")) {
                GameManager::ConsoleLog.clear();
            }
            ImGui::Separator();
            ImGui::TextUnformatted(GameManager::ConsoleLog.begin(), GameManager::ConsoleLog.end());
        ImGui::End();
    }

    // Editor Settings
    if (showSettingsMenu) {
        ImGui::Begin("Settings", &showSettingsMenu);
            ImGui::BeginTabBar("SettingsMenus", ImGuiTabBarFlags_None);
                ImGui::BeginTabItem("Display");
                    ImGui::Text("Resolution");
                    const char* previewValue = Settings::resolutions[Settings::selectedResolution];
                    if (ImGui::BeginCombo("##", previewValue)) {
                        for (int i = 0; i < Settings::resolutionsCount; i++) {
                            const bool isSelected = (Settings::selectedResolution == i);
                            if (ImGui::Selectable(Settings::resolutions[i], isSelected)) {
                                Settings::selectedResolution = i;
                                Settings::ChangeResolution(*window);
                            }

                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                ImGui::EndTabItem();
            ImGui::EndTabBar();
        ImGui::End();
    }
}

void EditorInstance::Update(sf::Time deltaTime) {
    // Process events
    sf::Event event;
    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);
        auto& io = ImGui::GetIO();

        GameManager::lastinput = event.key.code;

        // Close Window
        if (event.type == sf::Event::Closed) {
            window->close();
        }

        // Keyboard and Mouse input
        // Mouse Button Pressed
        if (event.type == sf::Event::MouseButtonPressed) {
            // If left mouse buttton is pressed, and we aren't clicking on an ImGui window
            if (event.mouseButton.button == 0 && !io.WantCaptureMouse) {
                // Camera drag move
                // Get starting position as reference for movement
                mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
                cameraMove = true;
            }

            // Right Mouse
            else if (event.mouseButton.button == 1) {
                sf::Vector2f converted = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
                
                for (Entity* e : GameManager::Entities) {
                    if (e->rect.contains(converted.x, converted.y)) {
                        // Toggle details menu visability
                        e->showDetailMenu = !e->showDetailMenu;
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
            cameraMove = false;
        }

        if (event.type == sf::Event::MouseMoved) {
            if (cameraMove) {
                // Calculate change
                sf::Vector2f currentPos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
                sf::Vector2f change = (sf::Vector2f)(mousePos - currentPos);

                // Update position
                camera->setCenter(camera->getCenter() + change);
                window->setView(*camera);
            }
        }

        // Mouse Wheel
        if (event.type == sf::Event::MouseWheelMoved) {
            if (!GameManager::isPlayingGame) {
                // Mouse wheel moved forwards
                if (event.mouseWheel.delta == 1) {
                    camera->zoom(0.8);
                    cameraZoom *= 1.2;
                }

                // Mouse wheel moved back
                else if (event.mouseWheel.delta == -1) {
                    camera->zoom(1.2);
                    cameraZoom *= 0.8;
                }

                window->setView(*camera);
            }
        }
    }
}

void EditorInstance::FixedUpdate(sf::Time deltaTime) {
    lastFixedUpdate += deltaTime;

    if (lastFixedUpdate > TimePerFrame) {
        lastFixedUpdate -= TimePerFrame;

        // Run Lua Scripts
        if (GameManager::isPlayingGame) {
            GameManager::RunLuaUpdates();
        }

        // Player position update
        if (GameManager::player != nullptr && GameManager::isPlayingGame) {
            GameManager::player->sprite.setPosition(GameManager::player->position);
            GameManager::player->UpdateRect();
            camera->setCenter(GameManager::player->position);
            window->setView(*camera);
        }

        // Physics
        if (GameManager::isPlayingGame) {            
            for (Entity* e : GameManager::Entities) {
                if (!e->physicsObject) continue;

                // Gravity
                bool collision = false;
                bool bottomCollision = false;
                bool topCollision = false;
                for (Entity* other : GameManager::Entities) {
                    if (e->ID == other->ID) continue;
                    if (!other->isSolid) continue;

                    if (e->bottomRect.intersects(other->rect)) {
                        collision = true;
                        bottomCollision = true;
                    }

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

                    // We bonked our head jumping
                    if (topCollision) {
                        e->velocity.y = 1; // DEBUG
                    }
                }
                // Free Fall
                else {
                    e->acceleration.y = GameManager::gravity;
                    e->grounded = false;
                }

                // Apply Velocity
                e->velocity += e->acceleration;
                e->position += e->velocity;
            }
        }
    }
    else {
        // Return so we aren't holding up the rest of the game
        return;
    }
}