#include "projectile_shooting_enemy.h"
#include <path_config.h>
#include <SOIL/SOIL.h>
#include <iostream>

namespace game {

    ProjectileShootingEnemy::ProjectileShootingEnemy(
        Game* game, glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture)
        : EnemyGameObject(position, geom, shader, texture),
        shootCooldownTime_(4.0f), //seconds cooldown
        timeSinceLastShot_(0.0f) {
        
        game_ = game;
        speed = 1.0f;
        followTime = 0.5f;

        const std::string resources_directory_g = RESOURCES_DIRECTORY;

        LoadTexture(bulletTex, (resources_directory_g + "/textures/acorn.png").c_str());
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

        game_->SpawnBullet(bulletStartPosition, this->GetBearing(), bulletTex, 3.0f, false);
    }

    void ProjectileShootingEnemy::LoadTexture(GLuint& texture, const char* filepath) {

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height;
        unsigned char* image = SOIL_load_image(filepath, &width, &height, 0, SOIL_LOAD_RGBA);
        if (image) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            SOIL_free_image_data(image);
        }
        else {
            std::cerr << "HUD texture failed to load at path: " << filepath << std::endl;
            texture = 0;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // namespace game
