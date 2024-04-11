#ifndef PROJECTILE_SHOOTING_ENEMY_H
#define PROJECTILE_SHOOTING_ENEMY_H

#include "enemy_game_object.h"
#include "bullet_game_object.h"
#include "game.h"

namespace game {

    class ProjectileShootingEnemy : public EnemyGameObject {
    public:
        ProjectileShootingEnemy(
            Game* game, glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture);

        void Update(double delta_time) override;
        void ShootProjectile(); // shots projectile 

    private:
        float shootCooldownTime_; // cooldown time
        float timeSinceLastShot_;
        Game* game_; // game reference
        GLuint bulletTex; // bullet texture

        //load teture
        void LoadTexture(GLuint& texture, const char* filepath);
    };

} // namespace game

#endif // PROJECTILE_SHOOTING_ENEMY_H
