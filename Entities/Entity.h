#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Entity {
public:
    Entity() = default;
    virtual ~Entity() = default;

    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& win) const;
    sf::FloatRect getBounds() const;
    void setHitboxLocal(const sf::FloatRect& localRect);
    void resetHitboxToSprite();
    sf::FloatRect getHitboxLocal() const;
    sf::FloatRect getHitboxGlobal() const;
    bool intersects(const Entity& other) const;
    static void setWorldBounds(float width, float height);
    static sf::Vector2f getWorldBounds();

    bool isAlive() const { return m_alive; }
    void destroy() { m_alive = false; }
    void setVisible(bool v) { m_visible = v; }
    bool isVisible() const { return m_visible; }
    void setPosition(const sf::Vector2f& pos) { m_sprite.setPosition(pos); }
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    void setVelocity(const sf::Vector2f& v) { m_velocity = v; }
    const sf::Vector2f& getVelocity() const { return m_velocity; }
    void setTexture(const sf::Texture& tex, bool resetOriginToCenter = true);
    void setTextureToSize(const sf::Texture& tex, float width, float height,
                          bool resetOriginToCenter = true);
    void fitToSize(float width, float height);
    void setTexture(const std::string& assetName,
                   const sf::Color& fallbackColor,
                   unsigned int width,
                   unsigned int height,
                   bool resetOriginToCenter = true);
    void setTextureRect(const sf::IntRect& rect);
    void setOriginCenter();
    void setScale(const sf::Vector2f& s) { m_sprite.setScale(s); }
    void setRotation(float angle) { m_sprite.setRotation(angle); }
    void setColor(const sf::Color& c) { m_sprite.setColor(c); }

protected:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity{0.f, 0.f};
    bool m_alive = true;
    bool m_visible = true;

    sf::FloatRect m_hitboxLocal{0.f, 0.f, 0.f, 0.f};
    bool m_hasCustomHitbox = false;
};
