#include "enemy_game_object.h"
#include <iostream>

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
	}

	void EnemyGameObject::Update(double delta_time) {

		// Update the time
		t += delta_time;

		// Motion in PATROLLING_ state
		if(state_ == PATROLLING_ && !IsDisabled()) {
			//std::cout<< "Patrolling" << std::endl;
			float x = start_pos_.x + radius * cos(t);
			float y = start_pos_.y + radius * sin(t);

			float rotation = GameObject::GetRotation();
			GameObject::SetRotation(rotation + delta_time);

			GameObject::SetPosition(glm::vec3(x, y, 0.0f));
		}
		if (state_ == INTERCEPTING_ && !IsDisabled()) {
			//std::cout<< "Intercepting" << std::endl;
			//std::cout<<update_timer_.Finished()<<std::endl;
			if (!update_timer_.Running()) {
				//std::cout << "intercepting timer started" << std::endl;
				velocity = glm::normalize(player_pos_ - GameObject::GetPosition());
				velocity *= speed;
				GameObject::SetRotation(atan2(velocity.y, velocity.x));
				update_timer_.Start(followTime);
			}
			else if (update_timer_.Finished()) {
				velocity = glm::normalize(player_pos_ - GameObject::GetPosition());
				velocity *= speed;
				GameObject::SetRotation(atan2(velocity.y, velocity.x));
				update_timer_.Start(followTime);
			}

			//std::cout << "Velocity: " << velocity.x << " " << velocity.y << " " << velocity.z << std::endl;
			GameObject::SetPosition(GameObject::GetPosition() + velocity * (float)delta_time);
		}
	}


} // namespace game