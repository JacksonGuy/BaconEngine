#ifndef RENDERING_H
#define RENDERING_H

#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "GameObject.hpp"

namespace Rendering {
    struct RenderingLayer {
        std::vector<GameObject*> objects;
        unsigned int layerNum;
    };

    extern std::unordered_map<std::string, sf::Texture*> m_textures;
    extern std::unordered_map<std::string, sf::Font*> m_fonts;
    extern std::vector<RenderingLayer> m_layers;

    sf::Texture* LoadTexture(std::string path);
    sf::Font* LoadFont(std::string path);
    void DrawGameObjects(sf::RenderWindow& window);
    void CreateLayers(int count);
    void AddToLayer(GameObject* obj);
    void SwapLayer(GameObject* obj, unsigned int layer);
    void RemoveFromLayer(GameObject* obj);
}

#endif