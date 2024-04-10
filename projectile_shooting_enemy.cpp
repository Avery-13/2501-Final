#include "projectile_shooting_enemy.h"

namespace game {

    ProjectileShootingEnemy::ProjectileShootingEnemy(
        Game* game, glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture)
        : EnemyGameObject(position, geom, shader, texture),
        shootCooldownTime_(2.0f), //2 seconds cooldown
        timeSinceLastShot_(0.0f) {
        
        game_ = game;
        speed = 1.0f;
        followTime = 1.0f;
    }

    void ProjectileShootingEnemy::Update(double delta_time) {
        // Call the parent update method
        EnemyGameObject::Update(delta_time);

        // Update shooting cooldown
        timeSinceLastShot_ += delta_time;

        // Check if it's time to shoot
        if (state_==INTERCEPTING_) {
            if (timeSinceLastShot_ >= shootCooldownTime_) {
                ShootProjectile();
                timeSinceLastShot_ = 0.0f; // Reset the cooldown
            }
        }

    }

    void ProjectileShootingEnemy::ShootProjectile() {
        glm::vec3 direction = glm::normalize(player_pos_ - this->GetPosition());
        glm::vec3 bulletStartPosition = this->GetPosition() + direction;

        game_->SpawnBullet(bulletStartPosition, direction);
    }

} // namespace game
