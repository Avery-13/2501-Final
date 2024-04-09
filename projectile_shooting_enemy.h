#ifndef PROJECTILE_SHOOTING_ENEMY_H
#define PROJECTILE_SHOOTING_ENEMY_H

#include "enemy_game_object.h"
//#include "bullet_game_object.h"

namespace game {

    class ProjectileShootingEnemy : public EnemyGameObject {
    public:
        ProjectileShootingEnemy(
            glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture);

        void Update(double delta_time) override;
        void ShootProjectile();

    private:
        float shootCooldownTime_;
        float timeSinceLastShot_;
    };

} // namespace game

#endif // PROJECTILE_SHOOTING_ENEMY_H
