#include "disc_collectible_game_object.h"
#include <iostream>

namespace game {

	/*
		DiscCollectibleGameObject inherits from GameObject
		Different functionality on collision with player
	*/

	DiscCollectibleGameObject::DiscCollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
		: GameObject(position, geom, shader, texture)
	{}

	void DiscCollectibleGameObject::Update(double delta_time) {
		// Rotate the object
		//rotation.y += 0.5f * delta;
	}


} // namespace game
