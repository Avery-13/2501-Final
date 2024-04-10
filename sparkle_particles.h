#ifndef SPARKLE_PARTICLES_H_
#define SPARKLE_PARTICLES_H_

#include "geometry.h"

#define NUM_SPARKLES 20

namespace game {

    // A set of particles that can be rendered
    class SparkleParticles : public Geometry {

        public:
            SparkleParticles(void);

            // Create the geometry (called once)
            void CreateGeometry(void);

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class SparkleParticles
} // namespace game

#endif // SPARKLE_PARTICLES_H_
