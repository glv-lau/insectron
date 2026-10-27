#include "Entity.h"
#include "../Managers/AssetManager.h"
#include <algorithm>
#include <cmath>

using namespace sf;

namespace {
    Vector2f g_worldBounds(800.f, 600.f);
}

void Entity::setWorldBounds(float width, float height) {
    g_worldBounds = Vector2f(std::max(1.f, width), std::max(1.f, height));
}

Vector2f Entity::getWorldBounds() {
    return g_worldBounds;
}

void Entity::update(float dt) {
    m_sprite.move(m_velocity * dt);
}

void Entity::draw(RenderWindow& win) const {
    if (m_alive && m_visible) {
        win.draw(m_sprite);
    }
}

FloatRect Entity::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void Entity::setHitboxLocal(const FloatRect& localRect) {
    m_hitboxLocal = localRect;
    m_hasCustomHitbox = true;
}

void Entity::resetHitboxToSprite() {
    FloatRect local = m_sprite.getLocalBounds();
    m_hitboxLocal = FloatRect(local.left, local.top, local.width, local.height);
    m_hasCustomHitbox = false;
}

FloatRect Entity::getHitboxLocal() const {
    if (m_hasCustomHitbox) return m_hitboxLocal;
    return m_sprite.getLocalBounds();
}

FloatRect Entity::getHitboxGlobal() const {
    return m_sprite.getTransform().transformRect(getHitboxLocal());
}

bool Entity::intersects(const Entity& other) const {
    return getHitboxGlobal().intersects(other.getHitboxGlobal());
}

void Entity::setTexture(const Texture& tex, bool resetOriginToCenter) {
    m_sprite.setTexture(tex);
    if (resetOriginToCenter) setOriginCenter();

    if (!m_hasCustomHitbox) {
        resetHitboxToSprite();
    }
}

void Entity::setTextureToSize(const Texture& tex, float width, float height,
                              bool resetOriginToCenter) {
    setTexture(tex, resetOriginToCenter);
    fitToSize(width, height);
}

void Entity::fitToSize(float width, float height) {
    const FloatRect bounds = m_sprite.getLocalBounds();
    if (bounds.width > 0.f && bounds.height > 0.f) {
        const float scale = std::min(width / bounds.width, height / bounds.height);
        m_sprite.setScale(scale, scale);
    }
}

void Entity::setTexture(const std::string& assetName,
                        const Color& fallbackColor,
                        unsigned int width,
                        unsigned int height,
                        bool resetOriginToCenter) {
    const Texture& tex = AssetManager::instance().getTexture(assetName, fallbackColor, width, height);
    setTextureToSize(tex, static_cast<float>(width), static_cast<float>(height),
                     resetOriginToCenter);
}

void Entity::setTextureRect(const IntRect& rect) {
    m_sprite.setTextureRect(rect);
    if (!m_hasCustomHitbox) {
        resetHitboxToSprite();
    }
}

void Entity::setOriginCenter() {
    FloatRect b = m_sprite.getLocalBounds();
    m_sprite.setOrigin(b.width / 2.f + b.left, b.height / 2.f + b.top);
    if (!m_hasCustomHitbox) resetHitboxToSprite();
}
