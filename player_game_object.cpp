#include "player_game_object.h"
#include <iostream>

#include "timer.h"

namespace game {

/*
	PlayerGameObject inherits from GameObject
	It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

PlayerGameObject::PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture)
	: GameObject(position, geom, shader, texture) 
{ 
	hp_ = 3; 
	objectsCollected_ = 0;
	isInvincible_ = false;
	velocity_ = glm::vec3(0, 0, 0);
}

// Update function for moving the player object around
void PlayerGameObject::Update(double delta_time) {
	//Normalize the velocity
	float norm = velocity_.length();
	velocity_ /= norm;

	//Movement
	SetPosition(GetPosition() + float(delta_time) * velocity_ * curr_speed_);

	// Special player updates go here
	if (isInvincible_) {
		if (invincibilityTimer_.Finished()) {
			isInvincible_ = false;
			std::cout<< "No longer invincible!" << std::endl;
		}
	}

	if (objectsCollected_ >= 3) {
		isInvincible_ = true;
		std::cout<< "Invincible!" << std::endl;
		objectsCollected_ = 0;

		invincibilityTimer_.Start(10.0f);
	}

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

void PlayerGameObject::Drive(int direction){
	// Convert the current angle to radians
	float angle = GameObject::GetRotation();

	// Calculate the velocity vector given the current angle
	float velocityX = cos(angle);
	float velocityY = sin(angle);

	// Set the velocity
	velocity_.x += velocityX;
	velocity_.y += velocityY;

	// Set the velocity to the desired speed (forwards or backwards)
	velocity_ *= direction;
}

void PlayerGameObject::Strafe(int direction) {
	velocity_ = glm::vec3(1, 0, 0);
	velocity_ *= direction;
}

void PlayerGameObject::SetSpeed(float speed) {
	curr_speed_ = base_speed_ * speed;
}

void PlayerGameObject::takeDamage() {
	hp_--;
}

} // namespace game
