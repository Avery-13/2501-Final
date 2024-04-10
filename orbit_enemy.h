#ifndef ORBIT_ENEMY_H
#define ORBIT_ENEMY_H

#include "enemy_game_object.h"
#include "game.h"

namespace game {

    class OrbitEnemy : public EnemyGameObject {
    public:
        OrbitEnemy(
            Game* game, glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture, GameObject* orbitObject);

        void Update(double delta_time) override;

    private:
        GameObject* orbitObject_; // The object that will orbit the enemy
        float orbitRadius_; // The radius of the orbit
        float orbitSpeed_; // The speed at which the object orbits
        float orbitAngle_; // Current angle of the orbiting object

        Game* game_;

        void UpdateOrbit(float delta_time);
    };

} // namespace game

#endif // ORBIT_ENEMY_H
