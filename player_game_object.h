#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
#include "timer.h"

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

        public:
            PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture);

            // Update function for moving the player object around
            void Update(double delta_time) override;
            void Drive(int direction);
            void Strafe(int direction);
            void SetVelocity(glm::vec3 velocity) { velocity_ = velocity; };
            void SetSpeed(float speed);

            void takeDamage();

            int hp_;
            int objectsCollected_;
            bool isInvincible_;
            Timer invincibilityTimer_;
            float base_speed_ = 4.0f;
            float curr_speed_ = base_speed_;

            glm::vec3 velocity_;

            Timer animationTimer_;
            int currentFrame_ = 0;
            float animationSpeed_;

    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
