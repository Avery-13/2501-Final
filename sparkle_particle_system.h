#ifndef SPARKLE_PARTICLE_SYSTEM_H_
#define SPARKLE_PARTICLE_SYSTEM_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class SparkleParticleSystem : public GameObject {

        public:
            SparkleParticleSystem(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GameObject *parent);

            void Update(double delta_time) override;

            void Render(glm::mat4 view_matrix, double current_time);

        private:
            GameObject *parent_;

    }; // class SparkleParticleSystem

} // namespace game

#endif // SPARKLE_PARTICLE_SYSTEM_H_
