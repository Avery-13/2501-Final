#ifndef POTION_COLLECTIBLE_GAME_OBJECT_H_
#define POTION_COLLECTIBLE_GAME_OBJECT_H_
#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class PotionCollectibleGameObject : public GameObject {

    public:
        PotionCollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update function for moving the player object around
        void Update(double delta_time) override;

    }; // class PotionCollectibleGameObject

} // namespace game

#endif // POTION_COLLECTIBLE_GAME_OBJECT_H_
