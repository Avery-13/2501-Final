#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "game_object.h"
#include "explosion_game_object.h"
#include "timer.h"
#include "geometry.h"
#include "tile.h"
#include "hud.h"
#include "player_game_object.h"
#include "bullet_game_object.h"
#include "collectible_game_object.h"
#include "disc_collectible_game_object.h"
#include "potion_collectible_game_object.h"
#include "particle_system.h"
#include "sparkle_particle_system.h"

namespace game {

    // A class for holding the main game objects
    class Game {

        //Test
        
        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

            glm::vec3 CalculateDirectionVector(float angleRadians);

            float GetRotationAngleFromDirection(glm::vec3 direction);

            void SpawnBullet(glm::vec3 position, glm::vec3 direction, GLuint texture, float speed, bool isFriendlyProjectile);

            void AddBullet(BulletGameObject* bullet);

            void SpawnOrbitEnemy(const glm::vec3& location);

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            //hud
            HUD* hud_;

            // Sprite geometry
            Geometry *sprite_;

            // Tile sprite geometry
            Geometry *tile_;

            // Particle geometry
            Geometry *particles_;

            // Sparkle particle geometry
            Geometry *sparkle_particles_;

            // Shader for rendering sprites in the scene
            Shader sprite_shader_;

            // Shader for rendering particles
            Shader particle_shader_;

            // Shader for rendering sparkle particles
            Shader sparkle_particle_shader_;

            // Shader for HUD
            Shader hud_shader_;

            // Shader for hearts
            Shader heart_shader_;

            // References to textures
            // This needs to be a pointer
            GLuint *tex_;

            // Player object
            PlayerGameObject *player_;

            // List of game objects
            std::vector<GameObject*> game_objects_;

            // List of background objects
            std::vector<GameObject*> background_objects_;

            //List of explosion objects
            std::vector<ExplosionGameObject*> explosions_;

            // list for bullets
            std::vector<BulletGameObject*> bullets_;

            // List of bone collectible objects
            std::vector<CollectibleGameObject*> collectibles_;

            // List of disc collectible objects
            std::vector<DiscCollectibleGameObject*> discs_;

            // list of potions
            std::vector<PotionCollectibleGameObject*> potions_;

            // Explosion object
            GameObject* explosion_;

            // Grass Particle System
            ParticleSystem* grass_particle_system_;

            // Keep track of time
            double current_time_;

            // Timer for spawning enemies
            Timer enemy_timer_;

            // Timer for spawning collectibles
            Timer collectible_timer_;

            // Current collided object (int index in vector)
            int curr_collided_object_;

            //total score
            int score;

            float lastShotTime_; // Time since the last shot was fired
            const float shotCooldown_ = 1.0f; // Cooldown period in seconds

            // Current amount of collected objects
            int collected_objects_;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void HandleControls(double delta_time);

            // Update all the game objects
            void Update(double delta_time);
 
            // Render the game world
            void Render(void);



    }; // class Game

} // namespace game

#endif // GAME_H_
