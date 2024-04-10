#ifndef BOMB_GAME_OBJECT_H
#define BOMB_GAME_OBJECT_H

#include "game_object.h"

namespace game {

    class BombGameObject : public GameObject {
    public:
        BombGameObject(glm::vec3 position, Geometry* geom, Shader* shader, GLuint texture, float blastRadius, float timeToExplode);
        void Update(double delta_time) override;
        bool HasExploded() const;
        float GetBlastRadius() const;

    private:
        float blastRadius_;
        float timeToExplode_;
        bool hasExploded_;
    };

} // namespace game

#endif // BOMB_GAME_OBJECT_H
