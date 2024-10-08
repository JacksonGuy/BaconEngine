#include "Rendering.hpp"
#include "GameManager.hpp"

namespace Rendering {
    std::unordered_map<std::string, sf::Texture*> m_textures;
    std::unordered_map<std::string, sf::Font*> m_fonts;
    std::vector<RenderingLayer> m_layers;
    sf::RenderTexture frame;

    /**
     * @brief Loads new Texture
     * 
     * @param path the system path of the texture image
     * @return sf::Texture* SFML texture object
     */
    sf::Texture* LoadTexture(std::string path) {
        // Texture isn't already loaded in
        if (m_textures.find(path) == m_textures.end()) {
            sf::Texture* texture = new sf::Texture();
            if (!texture->loadFromFile(path)) {
                GameManager::ConsoleWrite("[ERROR] Failed to load texture: " + path);
                return nullptr;
            }
            m_textures[path] = texture;
        }
        return m_textures[path];
   }

    /**
     * @brief Loads new font 
     * 
     * @param path system path of the font file
     * @return sf::Font* SFML font object
     */
    sf::Font* LoadFont(std::string path) {
        // Font isn't already loaded in
        if (m_fonts.find(path) == m_fonts.end()) {
            sf::Font* font = new sf::Font();
            if (!font->loadFromFile(path)) {
                GameManager::ConsoleWrite("[ERROR] Failed to load font: " + path);
                return nullptr;
            }
            m_fonts[path] = font;
        }
        return m_fonts[path];
    }

    /**
     * @brief Renders GameObjects on their layers
     * 
     * @param window The game window to draw to
     */
    void DrawGameObjects(sf::RenderWindow& window) {
        frame.clear(sf::Color(40, 40, 40));

        if (!GameManager::isPlayingGame) {
            // Dot at origin for orientation
            sf::CircleShape originDot(5);
            originDot.setFillColor(sf::Color::White);
            originDot.setPosition(sf::Vector2f(0,0));

            frame.draw(originDot);
        }

        for (RenderingLayer layer : m_layers) {
            for (GameObject* obj : layer.objects) {
                if (obj->type == ENTITY) {
                    Entity* e = (Entity*)obj;
                    if (e->isVisible) {
                        frame.draw(e->sprite);
                    }

                    if (e->showHitbox) {
                        sf::Vertex lines[] = {
                            sf::Vertex(e->hitbox[0], sf::Color::Red),
                            sf::Vertex(e->hitbox[1], sf::Color::Red),
                            sf::Vertex(e->hitbox[2], sf::Color::Red),
                            sf::Vertex(e->hitbox[3], sf::Color::Red),
                            sf::Vertex(e->hitbox[4], sf::Color::Red)
                        };

                        frame.draw(lines, 5, sf::LinesStrip);
                    }

                    if (e->editHitbox && e->showHitbox) {
                        for (int i = 0; i < 4; i++) {
                            sf::CircleShape point(5);
                            point.setPosition(e->hitbox[i]);
                            point.setOrigin(sf::Vector2f(5, 5));
                            point.setFillColor(sf::Color::Blue);
                            frame.draw(point);
                        }
                    }
                }
                else if (obj->type == TEXT) {
                    TextObj* text = (TextObj*)obj;
                    if (text->isVisible) {
                        frame.draw(text->text);
                    }
                }
                else if (obj->type == CAMERA) {
                    Camera* camera = (Camera*)obj;
                    if (camera->isVisible) {
                        sf::Vector2f pos = camera->position;
                        sf::Vector2f size = sf::Vector2f(camera->width, camera->height);

                        sf::Vertex lines[] = {
                            sf::Vertex(sf::Vector2f(pos.x - size.x/2, pos.y - size.y/2), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x + size.x/2, pos.y - size.y/2), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x + size.x/2, pos.y + size.y/2), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x - size.x/2, pos.y + size.y/2), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x - size.x/2, pos.y - size.y/2), sf::Color::Red)
                        };

                        frame.draw(lines, 5, sf::LinesStrip);
                    }
                }
                else {
                    std::cout << "CANT DRAW THING\n"; 
                }
            }
        }

        frame.display();
    }

    /**
     * @brief Creates new RenderingLayers
     * 
     * @param count the number of new layers to create
     */
    void CreateLayers(int count) {
        size_t size = m_layers.size();
        for (int i = 0; i < count; i++) {
            RenderingLayer layer;
            layer.objects = std::vector<GameObject*>();
            layer.layerNum = i + size;
            m_layers.push_back(layer);
        }
    }

    /**
     * @brief Add GameObject to their set layer
     * 
     * @param obj the GameObject to add
     */
    void AddToLayer(GameObject* obj) {
        // User wants to draw object onto a layer that hasn't been created yet
        if (obj->layer >= m_layers.size()) {
            CreateLayers(obj->layer - m_layers.size() + 1);
        }

        m_layers[obj->layer].objects.push_back(obj);
    } 


    /**
     * @brief Swaps GameObject from its current layer to another
     * 
     * @param obj The GameObject to swap
     * @param layer the new layer
     */
    void SwapLayer(GameObject* obj, unsigned int layer) {
        // Find Object in current layer and remove it
        std::vector<GameObject*> objects = m_layers[obj->layer].objects; 
        for (size_t i = 0; i < objects.size(); i++) {
            GameObject* other = objects[i];
            if (other->ID == obj->ID) {
                objects.erase(objects.begin() + i);
            }
        }

        // Add object to new layer
        obj->layer = layer;
        AddToLayer(obj);
    }

    /**
     * @brief Removes the given GameObject from its RenderingLayer
     * 
     * @param obj the GameObject to remove
     */
    void RemoveFromLayer(GameObject* obj) {
        int layer = obj->layer;
        
        for (size_t i = 0; i < m_layers[layer].objects.size(); i++) {
            GameObject* other = m_layers[layer].objects[i]; 
            if (other->ID == obj->ID) {
                m_layers[layer].objects.erase(m_layers[layer].objects.begin() + i);
                break;
            }
        }
    }
}