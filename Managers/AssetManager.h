#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

class AssetManager {
public:
    struct TextureEntry {
        sf::Texture texture;
        bool usedFallback = false;
    };

    static AssetManager& instance();

    const sf::Texture& getTexture(const std::string& assetName,
                                  const sf::Color& fallbackColor,
                                  unsigned int width,
                                  unsigned int height,
                                  bool* usedFallback = nullptr);

private:
    AssetManager() = default;

    std::unordered_map<std::string, TextureEntry> m_textures;
};
