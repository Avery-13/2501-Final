#include "bullet_game_object.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp> 
namespace game {

    BulletGameObject::BulletGameObject(const glm::vec3& position, Geometry* geometry, Shader* shader, GLuint texture, const glm::vec3& direction, float speed)
        : GameObject(position, geometry, shader, texture), direction_(direction), speed_(speed), lifespan_(5.0f), life_timer_(0.0f) {

        SetScale(glm::vec2(0.5, 0.5));
        glm::vec3 normalizedDirection = glm::normalize(direction);

        // Calculate the angle in radians from the normalized direction vector
        // atan2 returns the angle between the positive x-axis and the point (y, x)
        float angleRadians = atan2(normalizedDirection.y, normalizedDirection.x);
        SetRotation(angleRadians - (glm::pi<float>() / 2.0f));


    }

    BulletGameObject::~BulletGameObject() {

    }

    void BulletGameObject::Update(double delta_time) {
        // Update bullet position based on its velocity 
        if (!this->IsDisabled()) {
            position_ += direction_ * speed_ * static_cast<float>(delta_time);
        }
        life_timer_ += static_cast<float>(delta_time);

        // Check if the bullet's life_timer_ exceeds its lifespan_
        if (life_timer_ >= lifespan_) {
            MarkForDeletion(); 
        }

        GameObject::Update(delta_time);


        // Call the parent's update method if it does anything additional
        GameObject::Update(delta_time);
    }

    void BulletGameObject::Render(glm::mat4 view_matrix, double current_time) {

        GameObject::Render(view_matrix, current_time);

    }



} // namespace game