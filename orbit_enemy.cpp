#include "orbit_enemy.h"
#define GLM_FORCE_RADIANS
#define GLEW_STATIC
#include <glm/gtc/matrix_transform.hpp> 

namespace game {

    OrbitEnemy::OrbitEnemy(
        Game* game, glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture, GameObject* orbitObject)
        : EnemyGameObject(position, geom, shader, texture),
        orbitObject_(orbitObject),
        orbitRadius_(1.5f), // Set the initial orbit radius
        orbitSpeed_(7.0f), // Set the initial orbit speed
        orbitAngle_(0.0f) // Start with an initial angle of 0
        
    {
        game_ = game;
        orbitObjectRotationSpeed_ = 3.0f * glm::pi<float>();
        // You might want to initialize the orbiting object's position here
    }

    void OrbitEnemy::Update(double delta_time) {
        // Call the parent update method
        EnemyGameObject::Update(delta_time);

        // Update the orbiting object
        UpdateOrbit(delta_time);
    }

    void OrbitEnemy::UpdateOrbit(float delta_time) {
        // Update the angle based on the orbit speed
        orbitAngle_ += orbitSpeed_ * delta_time;

        // Calculate the new position of the orbiting object
        float newX = position_.x + orbitRadius_ * cos(orbitAngle_);
        float newY = position_.y + orbitRadius_ * sin(orbitAngle_);

        // Update the orbiting object's position
        orbitObject_->SetPosition(glm::vec3(newX, newY, position_.z));

        // Update the rotation of the orbiting object
        float rotationAngle = orbitObject_->GetRotation();
        rotationAngle -= orbitObjectRotationSpeed_ * delta_time; // Increment the angle

        // Set the new rotation of the orbiting object
        orbitObject_->SetRotation(rotationAngle);
    }

} // namespace game
