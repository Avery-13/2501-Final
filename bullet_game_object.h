#ifndef BULLET_GAME_OBJECT_H
#define BULLET_GAME_OBJECT_H

#include "game_object.h"
#include "particle_system.h"

namespace game {

    class BulletGameObject : public GameObject {
    public:
        BulletGameObject(const glm::vec3& position, Geometry* geometry, Shader* shader, GLuint texture, const glm::vec3& direction, float speed, bool isFriendlyProjectile);
        ~BulletGameObject();
        // Ensure the Update method matches the capitalization and parameter type of the base class
        void Update(double delta_time) override;
        void Render(glm::mat4 view_matrix, double current_time) override;
        void SetTrail(ParticleSystem* trail) { trail_ = trail; } //not used
        bool isFriendly; //used in collisions to check if its a player bullet

    private:
        glm::vec3 direction_;
        float speed_;
        float lifespan_; // Lifespan in seconds
        float life_timer_; // Timer to track the bullet's age
        ParticleSystem* trail_; // not used

    };

} // namespace game
#endif // BULLET_GAME_OBJECT_H