#ifndef EXPLOSION_GAME_OBJECT
#define EXPLOSION_GAME_OBJECT
#include "game_object.h"
#include "timer.h"

namespace game {
	class ExplosionGameObject : public GameObject {
		public:
			ExplosionGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);
			void Update(double delta_time) override;

			Timer explosion_timer_;

		private: 
	};
} // namespace game
#endif // EXPLOSION_GAME_OBJECT