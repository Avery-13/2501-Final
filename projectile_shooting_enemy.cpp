#include "projectile_shooting_enemy.h"

namespace game {

    ProjectileShootingEnemy::ProjectileShootingEnemy(
        glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture)
        : EnemyGameObject(position, geom, shader, texture),
        shootCooldownTime_(2.0f), // Example: 2 seconds cooldown
        timeSinceLastShot_(0.0f) {
    
        speed = 1.0f;
        followTime = 1.0f;
    }

    void ProjectileShootingEnemy::Update(double delta_time) {
        // Call the parent update method
        EnemyGameObject::Update(delta_time);

        // Update shooting cooldown
        timeSinceLastShot_ += delta_time;

        // Check if it's time to shoot
        if (timeSinceLastShot_ >= shootCooldownTime_) {
            ShootProjectile();
            timeSinceLastShot_ = 0.0f; // Reset the cooldown
        }
    }

    void ProjectileShootingEnemy::ShootProjectile() {
        // Implement the logic to shoot a projectile
        // This could involve creating a new BulletGameObject and adding it to the game
        // Example:
        // glm::vec3 bulletDirection = glm::normalize(playerPosition - position_);
        // game->SpawnBullet(position_, bulletDirection, bulletRotation);
    }

} // namespace game
