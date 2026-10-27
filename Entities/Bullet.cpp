#include "Bullet.h"

using namespace sf;

Bullet::Bullet(const Texture& tex, const Vector2f& pos, const Vector2f& vel, bool fromPlayer)
    : m_fromPlayer(fromPlayer)
{
    const float width = fromPlayer ? 12.f : 10.f;
    const float height = fromPlayer ? 26.f : 10.f;
    setTextureToSize(tex, width, height, true);
    setHitboxLocal(sf::FloatRect(-width * 0.5f, -height * 0.5f, width, height));
    if (fromPlayer) {
        setColor(Color(120, 235, 255));
    }
    setPosition(pos);
    setVelocity(vel);
}

void Bullet::update(float dt) {
    Entity::update(dt);

    const Vector2f p = getPosition();
    const Vector2f bounds = Entity::getWorldBounds();
    if (p.x < -100.f || p.x > bounds.x + 100.f
        || p.y < -100.f || p.y > bounds.y + 100.f) {
        destroy();
    }
}
