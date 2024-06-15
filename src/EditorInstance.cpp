#include "EditorInstance.hpp"

#include <iostream>

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

    // Set Engine UI Variables
    this->cameraMove = false;                           // Is the Camera Moving?
    this->showEntityCreate = false;                     // Entity Create Menu
    this->showMainMenu = true;                          // General Main Menu
    this->showEntityList = false;                       // List of Entities
    this->showConsole = false;                          // Debug Console
    this->showSettingsMenu = false;                     // Editor Settings

    this->showFailedPopup = false;                      // Used for displaying errors with saving/loading
    this->failedMessage = "";                           // What error to display

    // Save and Load project Variables
    this->showLoadPopup = false;                        // Load Project Popup
    this->loadProjectName = "";                         // Placeholder for Project Name
    this->showSaveAsPopup = false;                      // Save Project As Popup
    this->saveAsProjectName = this->projectTitle;       // Placeholder for Project Name

    // Create Entity Variables
    this->createPosition[0] = 0.f;                      // Default Position
    this->createPosition[1] = 0.f;
    this->createNameBuffer = "Entity";                  // Default Name
    this->createImagePath = "";                         // Default Image Path

    // Other
    this->lastFixedUpdate = sf::Time::Zero;
    this->frameLimit = 60;                              // Change if necessary
    this->TimePerFrame = sf::seconds(1.f / frameLimit);
}

// Main Game Loop
void EditorInstance::Run() {
    // Origin
    sf::CircleShape originDot(5);
    originDot.setFillColor(sf::Color::White);
    originDot.setPosition(0.0f, 0.0f);
    
    // DEBUG
    load("Game.json");

    while(window->isOpen()) {
        sf::Time deltaTime = clock.restart();

        this->Update(deltaTime);         // Input
        this->FixedUpdate(deltaTime);    // Physics
        this->DrawUI(deltaTime);         // ImGui Frames

        this->window->clear(sf::Color(40, 40, 40));
        if (!GameManager::isPlayingGame) {
            window->draw(originDot);
        }
        GameManager::DrawEntities(*this->window);
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
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Entity List", NULL, &showEntityList));
            if (ImGui::MenuItem("Test Menu", NULL, &showMainMenu));
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
            ImGui::InputText("##", loadProjectName.data(), 64);
            if (ImGui::Button("Load")) {
                ImGui::CloseCurrentPopup();
                showLoadPopup = false;
                if (load(loadProjectName)) {
                    //std::cout << "[DEBUG] Successfully Loaded Project" << std::endl;
                    GameManager::ConsoleWrite("[DEBUG] Successfully Loaded Project");
                    loadedProject = true;
                    projectTitle = loadProjectName;
                    window->setTitle("Bacon - " + projectTitle);
                }
                else {
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
            ImGui::InputText("##", saveAsProjectName.data(), 64);
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

    // General Test Menu
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

                if (ImGui::BeginTabItem("Testing")) {
                    if (ImGui::Button("This is a button")) {
                        GameManager::ConsoleWrite("Test");
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
            ImGui::Text("ID: %d", e->ID);

            ImGui::InputText("Name", e->name.data(), sizeof(e->name.data()));

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

            ImGui::Checkbox("Solid", &e->isSolid);
            ImGui::SameLine();
            ImGui::Checkbox("Physics Object", &e->physicsObject);

            ImGui::Separator();
            ImGui::InputText("Texture", e->texturePath.data(), 64);
            if (ImGui::Button("Change Texture")) {
                e->SetSprite(e->texturePath);
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

            if (ImGui::Button("Delete")) {
                free(GameManager::Entities[i]);
                GameManager::Entities.erase(GameManager::Entities.begin() + i);
            }
        ImGui::End();
    }

    if (showEntityCreate) {
        ImGui::Begin("Create new entity", &showEntityCreate);
            // Displays
            ImGui::InputText("Name", createNameBuffer.data(), 32);
            ImGui::InputFloat2("Position", createPosition);
            ImGui::InputText("Image Path", createImagePath.data(), 32);

            // Create Button
            if (ImGui::Button("Create")) {
                Entity* entity = new Entity({createPosition[0], createPosition[1]});
                entity->name = createNameBuffer;
                entity->SetSprite(createImagePath);
            }
        ImGui::End();
    }

    if (showEntityList) {
        ImGui::Begin("Entities", &showEntityList);
            auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            for (Entity* e : GameManager::Entities) {
                ImGui::TreeNodeEx(e->name.data(), flags);
                if (ImGui::IsItemClicked()) {
                    e->showDetailMenu = !e->showDetailMenu;
                }
            }
        ImGui::End();
    }

    if (showConsole) {
        ImGui::Begin("Console", &showConsole);
            if (ImGui::Button("Clear")) {
                GameManager::ConsoleLog.clear();
            }
            ImGui::Separator();
            ImGui::TextUnformatted(GameManager::ConsoleLog.begin(), GameManager::ConsoleLog.end());
        ImGui::End();
    }

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
    sf::Event event;
    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);
        auto& io = ImGui::GetIO();

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
        // Player Input
        float speed = 0.3f;
        if (GameManager::player != nullptr && GameManager::isPlayingGame) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if (!GameManager::checkCollisionSide(GameManager::player->topRect)) {
                    GameManager::player->position.y -= speed;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                if (!GameManager::checkCollisionSide(GameManager::player->bottomRect)) {
                    GameManager::player->position.y += speed;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                if (!GameManager::checkCollisionSide(GameManager::player->leftRect)) {
                    GameManager::player->position.x -= speed;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                if (!GameManager::checkCollisionSide(GameManager::player->rightRect)) {
                    GameManager::player->position.x += speed;
                }
            }

            GameManager::player->sprite.setPosition(GameManager::player->position);
            GameManager::player->UpdateRect();
            camera->setCenter(GameManager::player->position);
            window->setView(*camera);
        }
    }
    else {
        // Return so we aren't holding up the rest of the game
        return;
    }
}