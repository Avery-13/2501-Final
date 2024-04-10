#ifndef DISC_COLLECTIBLE_GAME_OBJECT_H_
#define DISC_COLLECTIBLE_GAME_OBJECT_H_
#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class DiscCollectibleGameObject : public GameObject {

    public:
        DiscCollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update function for moving the player object around
        void Update(double delta_time) override;

    }; // class DiscCollectibleGameObject

} // namespace game

#endif // DISC_COLLECTIBLE_GAME_OBJECT_H_
