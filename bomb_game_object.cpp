#include "bomb_game_object.h"

namespace game {

    BombGameObject::BombGameObject(glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture, float blastRadius, float timeToExplode)
        : GameObject(position, geom, shader, texture), blastRadius_(blastRadius), timeToExplode_(timeToExplode), hasExploded_(false) {
        // Constructor implementation
    }

    void BombGameObject::Update(double delta_time) {
        if (hasExploded_)
            return;

        if (timeToExplode_ <= 0) {
            // Bomb explodes
            hasExploded_ = true;
        }
        else {
            // Countdown to explosion
            timeToExplode_ -= delta_time;
        }
    }

    bool BombGameObject::HasExploded() const {
        return hasExploded_;
    }

    float BombGameObject::GetBlastRadius() const {
        return blastRadius_;
    }

} // namespace game
