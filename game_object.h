#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "geometry.h"
#include "tile.h"
#include "sprite.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture);

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            // Getters
            inline glm::vec3 GetPosition(void) const { return position_; }
            inline glm::vec2 GetScale(void) const { return scale_; }
            inline float GetRotation(void) const { return angle_; }
            bool IsCollidable(void) const { return isCollidable_; }
            bool IsOrbiting(void) const { return isOrbiting_; }
            bool IsGhost(void) const { return render_ghost_; }
            bool IsDisabled(void) const { return disabled_; }

            // Get bearing direction (direction in which the game object
            // is facing)
            glm::vec3 GetBearing(void) const;

            // Get vector pointing to the right side of the game object
            glm::vec3 GetRight(void) const;

            // Calculate the orbit matrix
            glm::mat4 GameObject::CalculateOrbitMatrix(double currentTime);

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(glm::vec2 s) { scale_ = s; }
            void SetRotation(float angle);
            void SetOrbit(float radius, float speed, glm::vec2 centre);
            void SetCollidable(bool isCollidable) { isCollidable_ = isCollidable; }
            void SetGhost(bool isGhost) { render_ghost_ = isGhost; }
            void SetGold(bool isGold) { render_gold_ = isGold; }
            void SetTexture(GLuint texture) { texture_ = texture; }
            void SetDisabled(bool disabled) { disabled_ = disabled; }

            // Method to mark the object for deletion
            void MarkForDeletion() { marked_for_deletion_ = true; }

            // Getter to check if the object is marked for deletion
            bool IsMarkedForDeletion() const { return marked_for_deletion_; }


        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            glm::vec2 scale_;
            float angle_;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

            // Object's collision sphere
            bool isCollidable_ = true;

            // Object's orbiting variables
            bool isOrbiting_ = false;
            float orbit_radius_;
            float orbit_speed_;
            glm::vec2 orbit_centre_;

            // Object's rendering mode
            bool render_ghost_ = false;
            bool render_gold_;

            // Object's time variable
            float t = 0.0f;

            bool disabled_ = false;

            bool marked_for_deletion_ = false;

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
