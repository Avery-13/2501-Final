#include "explosion_game_object.h"
#include <iostream>

namespace game {

	ExplosionGameObject::ExplosionGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
		: GameObject(position, geom, shader, texture)
	{
		Timer timer_;
		explosion_timer_.Start(5.0f);
	}

	void ExplosionGameObject::Update(double delta_time) {
		// Check if the timer has finished
		if (explosion_timer_.Finished()) {
			std::cout << "Explosion finished!" << std::endl;
		}
		// Updates would go here
	}

} // namespace game