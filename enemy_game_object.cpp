#include "enemy_game_object.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp> 

namespace game {

	/*
		EnemyGameObject inherits from GameObject
		Has a state machine that determines its movement behaviour
	*/

	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
		: GameObject(position, geom, shader, texture)
	{
		start_pos_ = position;
		Timer update_timer_;
		state_ = PATROLLING_;
		wander_cool_down_ = 0.0;
		velocity = glm::vec3(0.0, 1.0, 0.0);
		speed = 2.2f;
	}

	// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
	float map(float input, float a, float b, float c, float d) {

		return c + (d - c) * (input - a) / (b - a);
	}

	void EnemyGameObject::Update(double delta_time) {

		// Update the time
		t += delta_time;

		// Motion in PATROLLING_ state
		if(state_ == PATROLLING_ && !IsDisabled()) {
			//std::cout<< "Patrolling" << std::endl;
			float x = start_pos_.x + radius * cos(t);
			float y = start_pos_.y + radius * sin(t);

			// Calculate the tangent angle. The derivative of the position (x, y) with respect to t
			float vx = -radius * sin(t); // derivative of cos is -sin
			float vy = radius * cos(t);  // derivative of sin is cos

			// This angle will be tangent to the circle at the current position.
			float tangentAngle = atan2(vy, vx);

			// Set the enemy's rotation to face in the direction of the tangent
			GameObject::SetRotation(tangentAngle);
			GameObject::SetPosition(glm::vec3(x, y, 0.0f));

		}
		if (state_ == INTERCEPTING_ && !IsDisabled()) {
			// Chase steering behavior

			// Compute steering force (acceleration)
			glm::vec3 desired = player_pos_ - position_;
			glm::vec3 acc = desired * speed - velocity;
			glm::vec3 direction = glm::normalize(acc);
			float rotationAngle = atan2(direction.y, direction.x);

			// Add steering to velocity
			velocity += acc * ((float)delta_time);

			velocity = speed * glm::normalize(velocity);

			// Perform arrival
			float r = 0.5; // Distance threshold when arrival kicks in
			float d = glm::length(player_pos_ - position_);
			if (d < r) {
				float z = map(d, 0, r, 0, speed);
				velocity = z * glm::normalize(velocity);
			}

			position_ += velocity* ((float)delta_time);

			SetRotation(rotationAngle);
		}
	}


} // namespace game