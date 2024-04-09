#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_
#include "game_object.h"
#include "timer.h"

namespace game {
    const int PATROLLING_ = 1;
    const int INTERCEPTING_ = 2;

    // Inherits from GameObject
    class EnemyGameObject : public GameObject {

    public:
        EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update function for moving the enemy object around
        void Update(double delta_time) override;
        void updatePlayerPos(glm::vec3 player_pos) { player_pos_ = player_pos; }

        int state_;
        float radius = 3.0f;
        float t = 0;
        float speed = 1.4f;
        float followTime = 2.0f;
        glm::vec3 velocity;
        glm::vec3 player_pos_;
        glm::vec3 start_pos_;

        Timer update_timer_;

    }; // class EnemyeGameObject

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_
