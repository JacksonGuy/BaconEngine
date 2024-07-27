#include "Rendering.hpp"
#include "GameManager.hpp"

namespace Rendering {
    std::unordered_map<std::string, sf::Texture*> m_textures;
    std::unordered_map<std::string, sf::Font*> m_fonts;
    std::vector<RenderingLayer> m_layers;

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
        for (RenderingLayer layer : m_layers) {
            for (GameObject* obj : layer.objects) {
                if (obj->type == ENTITY) {
                    Entity* e = (Entity*)obj;
                    if (e->isVisible) {
                        window.draw(e->sprite);
                    }

                    if (e->showHitbox) {
                        sf::Vector2f pos = e->rect.getPosition();
                        sf::Vector2f size = e->rect.getSize();

                        sf::Vertex lines[] = {
                            sf::Vertex(sf::Vector2f(pos.x, pos.y), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y + size.y), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x, pos.y + size.y), sf::Color::Red),
                            sf::Vertex(sf::Vector2f(pos.x, pos.y), sf::Color::Red)
                        };

                        window.draw(lines, 5, sf::LinesStrip);
                    }
                }
                else if (obj->type == TEXT) {
                    TextObj* text = (TextObj*)obj;
                    if (text->isVisible) {
                        window.draw(text->text);
                    }
                }
            }
        }
    }

    /**
     * @brief Creates new RenderingLayers
     * 
     * @param count the number of new layers to create
     */
    void CreateLayers(int count) {
        for (int i = 0; i < count; i++) {
            RenderingLayer layer;
            layer.objects = std::vector<GameObject*>();
            layer.layerNum = i + m_layers.size();
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
}