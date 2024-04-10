#include <stdexcept>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <path_config.h>
#include "game.h"
#include "sprite.h"
#include "particles.h"
#include "sparkle_particles.h"
#include "tile.h"
#include "shader.h"
#include "player_game_object.h"
#include "collectible_game_object.h"
#include "potion_collectible_game_object.h"
#include "disc_collectible_game_object.h"
#include "enemy_game_object.h"
#include "timer.h"
#include "projectile_shooting_enemy.h"
#include "orbit_enemy.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "Game Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);

//Other global
float pi_over_two = glm::pi<float>() / 2.0f;
// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set whether window can be resized
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    tile_ = new Tile();
    sprite_->CreateGeometry();
    tile_->CreateGeometry();
    score = 0;
    lastShotTime_ = -shotCooldown_;

    // Initialize particle geometry
    particles_ = new Particles();
    particles_->CreateGeometry();

    // Initialize sparkle particle geometry
    sparkle_particles_ = new SparkleParticles();
    sparkle_particles_->CreateGeometry();

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize HUD shader
    hud_shader_.Init((resources_directory_g + std::string("/hud_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/hud_fragment_shader.glsl")).c_str());

    // Initialize heart shader
    heart_shader_.Init((resources_directory_g + std::string("/heart_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/heart_fragment_shader.glsl")).c_str());

    // Initialize particle shader
    particle_shader_.Init((resources_directory_g + std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/particle_fragment_shader.glsl")).c_str());

    // Initialize sparkle particle shader
    sparkle_particle_shader_.Init((resources_directory_g + std::string("/sparkle_particle_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/sparkle_particle_fragment_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;
}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    delete particles_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }
    for (int i = 0; i < bullets_.size(); i++) {
        delete bullets_[i];
    }

    delete hud_;

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{
    // Get window dimensions
    int width, height;
    glfwGetWindowSize(window_, &width, &height);

    // Setup the game world

    // Load textures
    SetAllTextures();

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    player_ = new PlayerGameObject(glm::vec3(0.0f, 0.0f, -1.0f), sprite_, &sprite_shader_, tex_[0]);
    game_objects_.push_back(player_);
    game_objects_[0]->SetRotation(pi_over_two);


    // Setup enemy objects
    game_objects_.push_back(new EnemyGameObject(glm::vec3(-5.0f, 1.0f, 0.0f), sprite_, &sprite_shader_, tex_[3]));
    game_objects_[1]->SetRotation(pi_over_two);
    game_objects_[1]->SetDisabled(true);

    game_objects_.push_back(new OrbitEnemy(this, glm::vec3(-5.0f, 1.0f, 0.0f), sprite_, &sprite_shader_, tex_[11], game_objects_[1]));
    game_objects_[2]->SetRotation(pi_over_two);


    // Setup collectible objects
    
    // Bones
    collectibles_.push_back(new CollectibleGameObject(glm::vec3(2.0f, 2.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    collectibles_[0]->SetScale(glm::vec2(0.5f, 0.5f));
    collectibles_.push_back(new CollectibleGameObject(glm::vec3(-2.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    collectibles_[1]->SetScale(glm::vec2(0.5f, 0.5f));
    collectibles_.push_back(new CollectibleGameObject(glm::vec3(-2.0f, 3.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    collectibles_[2]->SetScale(glm::vec2(0.5f, 0.5f));
    collectibles_.push_back(new CollectibleGameObject(glm::vec3(4.0f, 1.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    collectibles_[3]->SetScale(glm::vec2(0.5f, 0.5f));
    collectibles_.push_back(new CollectibleGameObject(glm::vec3(-1.0f, -3.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    collectibles_[4]->SetScale(glm::vec2(0.5f, 0.5f));
    collectibles_.push_back(new CollectibleGameObject(glm::vec3(3.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    collectibles_[5]->SetScale(glm::vec2(0.5f, 0.5f));

    // Potions
    potions_.push_back(new PotionCollectibleGameObject(glm::vec3(5.0f, 4.0f, 0.0f), sprite_, &sprite_shader_, tex_[17]));
    potions_[0]->SetScale(glm::vec2(0.6f, 0.6f));
    potions_.push_back(new PotionCollectibleGameObject(glm::vec3(-3.0f, 1.0f, 0.0f), sprite_, &sprite_shader_, tex_[17]));
    potions_[1]->SetScale(glm::vec2(0.6f, 0.6f));
    potions_.push_back(new PotionCollectibleGameObject(glm::vec3(-4.0f, -3.0f, 0.0f), sprite_, &sprite_shader_, tex_[17]));
    potions_[2]->SetScale(glm::vec2(0.6f, 0.6f));

    // Randomize positions for discs
    srand(time(NULL));

    // Discs
    for (int i = 0; i < 3; ++i) {
        float x = (rand() % 41) - 30.0f; // Random x value between -30 and 30
        float y = (rand() % 41) - 30.0f; // Random y value between -30 and 30
        float z = -1.0f; // Fixed z value

        SpawnOrbitEnemy(glm::vec3(x, y, z));
        discs_.push_back(new DiscCollectibleGameObject(glm::vec3(x, y, z), sprite_, &sprite_shader_, tex_[13 + i]));
        discs_[i]->SetScale(glm::vec2(0.7f, 0.7f));

        std::cout << "Disc " << i << " at: " << x << " " << y << std::endl;
    }

    // Disc particle effects
    GameObject* disc_particles = new SparkleParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), sparkle_particles_, &sparkle_particle_shader_, tex_[8], discs_[0]);
    disc_particles->SetScale(glm::vec2(0.1f, 0.1f));
    disc_particles->SetRotation(-pi_over_two);
    disc_particles->SetCollidable(false);
    discs_[0]->sparkles_ = disc_particles; // Add pointer to the particle system to the disc object

    disc_particles = new SparkleParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), sparkle_particles_, &sparkle_particle_shader_, tex_[8], discs_[1]);
    disc_particles->SetScale(glm::vec2(0.1f, 0.1f));
    disc_particles->SetRotation(-pi_over_two);
    disc_particles->SetCollidable(false);
    discs_[1]->sparkles_ = disc_particles; // Add pointer to the particle system to the disc object

    disc_particles = new SparkleParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), sparkle_particles_, &sparkle_particle_shader_, tex_[8], discs_[2]);
    disc_particles->SetScale(glm::vec2(0.1f, 0.1f));
    disc_particles->SetRotation(-pi_over_two);
    disc_particles->SetCollidable(false);
    discs_[2]->sparkles_ = disc_particles; // Add pointer to the particle system to the disc object

    // Setup background
    GameObject *background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tile_, &sprite_shader_, tex_[4]);
    background->SetScale(glm::vec2(90.0f, 90.0f));
    background_objects_.push_back(background);

    // Initialize the HUD
    hud_ = new HUD(resources_directory_g + "/textures/hud/", &hud_shader_, glm::ortho(0.0f, (float)width, (float)height, 0.0f), &heart_shader_);

    // Setup particle system
    GameObject* particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[10], game_objects_[0]);
    particles->SetScale(glm::vec2(0.4f, 0.4f));
    particles->SetRotation(-pi_over_two);
    particles->SetCollidable(false);
    grass_particle_system_ = dynamic_cast<ParticleSystem*>(particles);

    // Nullify explosion
    explosion_ = NULL;
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image){
        std::cout << "Cannot load texture " << fname << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    // Declare all the textures here
    const char *texture[] = 
        {"/textures/player_frames/left_step.png", "/textures/player_frames/right_step.png", "/textures/player_frames/still.png", "/textures/squirrel_brown.png", "/textures/grass03.png", "/textures/acorn.png", "/textures/explosion0.png",
        "/textures/bone.png", "/textures/star_particle.png", "/textures/bullet.png", "/textures/grass_particle.png", "/textures/squirrel_black.png", "/textures/squirrel_red.png", "/textures/disc_o.png", "/textures/disc_b.png" , "/textures/disc_p.png", 
        "/textures/poop.png", "/textures/potion.png"};
    // Get number of declared textures
    int num_textures = sizeof(texture) / sizeof(char *);
    // Allocate a buffer for all texture references
    tex_ = new GLuint[num_textures];
    glGenTextures(num_textures, tex_);
    // Load each texture
    for (int i = 0; i < num_textures; i++){
        SetTexture(tex_[i], (resources_directory_g+std::string(texture[i])).c_str());
    }
    // Set first texture in the array as default
    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}


void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update window events like input handling
        glfwPollEvents();

        // Handle user input
        HandleControls(delta_time);

        // Update all the game objects
        Update(delta_time);

        // Update the HUD with new information
        hud_->Update(score, player_->hp_, player_->objectsCollected_, player_->isInvincible_, player_->invincibilityTimer_.TimeLeft(), glm::vec2(player_->GetPosition().x, player_->GetPosition().y), player_->goalObjectsCollected_);


        // Render all the game objects
        Render();

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);
    }
}


void Game::HandleControls(double delta_time)
{
    // Get player game object
    GameObject *player = game_objects_[0];
    // Get current position and angle
    glm::vec3 curpos = player->GetPosition();
    float angle = player->GetRotation();
    // Compute current bearing direction
    glm::vec3 dir = player->GetBearing();
    // Adjust motion increment and angle increment 
    // if translation or rotation is too slow
    float speed = delta_time*1500.0;
    float motion_increment = 0.001*speed;
    float angle_increment = (glm::pi<float>() / 1200.0f)*speed;
    // Get window dimensions
    int width, height;
    glfwGetWindowSize(window_, &width, &height);

    // Check for escape key press
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }

    //Reset Player velocity to 0 between keystrokes
    dynamic_cast<PlayerGameObject*>(player)->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    //Reset Player speed to base speed
    dynamic_cast<PlayerGameObject*>(player)->SetSpeed(1.0f);

    // Check for player movement input and make changes accordingly
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        dynamic_cast<PlayerGameObject*>(player)->Drive(1);
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        dynamic_cast<PlayerGameObject*>(player)->Drive(-1);
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        player->SetRotation(angle - angle_increment);
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        player->SetRotation(angle + angle_increment);
    }
    if (glfwGetKey(window_, GLFW_KEY_Z) == GLFW_PRESS) {
        dynamic_cast<PlayerGameObject*>(player)->Strafe(-1);
    }
    if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS) {
        dynamic_cast<PlayerGameObject*>(player)->Strafe(1);
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// Speed boost the player
        dynamic_cast<PlayerGameObject*>(player)->SetSpeed(2.0f);
	}
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window_, &xpos, &ypos);
        glm::vec3 direction = glm::vec3(xpos - width/2, ypos - height/2, 0.0f);
        float bulletAngle = atan2((-1)*direction.y, direction.x);
        direction = CalculateDirectionVector(bulletAngle);
        SpawnBullet(player->GetPosition(), direction, tex_[9], 7.0f, true);
    }
}


void Game::Update(double delta_time)
{
    // Get player game object
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
    //std::cout << "Player position: " << player->GetPosition().x << " " << player->GetPosition().y << std::endl;


    // Animate player
    if (player->animationTimer_.Finished()) {
        player->animationTimer_.Start(player->animationSpeed_);

        // Check if player is moving
        if (player->animationSpeed_ != 0.0f) {
            // Change player texture
            if (player->currentFrame_ == 0) {
                game_objects_[0]->SetTexture(tex_[1]);
                player->currentFrame_ = 1;
            }
            else {
                game_objects_[0]->SetTexture(tex_[0]);
                player->currentFrame_ = 0;
            }
        }
        else {
			// Set player texture to still
			game_objects_[0]->SetTexture(tex_[2]);
        }
	}

    // Update time
    current_time_ += delta_time;
    if (!enemy_timer_.Running()) {
        enemy_timer_.Start(3.0f);
    }

    if (enemy_timer_.Finished()) {
        // Generate random coordinates to spawn a new enemy
        int rand_x = rand() % 25 - 2;
        int rand_y = rand() % 25 - 2;

        // Spawn new enemy
        std::cout << "New enemy at: " << rand_x << " " << rand_y << std::endl;
		game_objects_.push_back(new EnemyGameObject(glm::vec3(rand_x, rand_y, 0.0f), sprite_, &sprite_shader_, tex_[3]));
        game_objects_.back()->SetRotation(pi_over_two);
	}

    if (!collectible_timer_.Running()) {
        collectible_timer_.Start(15.0f);
    }

    if (collectible_timer_.Finished()) {
        // Generate random coordinates to spawn a new bone collectible
        int rand_x = rand() % 35 - 2;
        int rand_y = rand() % 35 - 2;

        // Spawn new collectible
        //std::cout << "New bone at: " << rand_x << " " << rand_y << std::endl;
        collectibles_.push_back(new CollectibleGameObject(glm::vec3(rand_x, rand_y, 0.0f), sprite_, &sprite_shader_, tex_[7]));
        collectibles_.back()->SetScale(glm::vec2(0.5f, 0.5f));

        // Generate random coordinates to spawn a new potion collectible
        rand_x = rand() % 35 - 2;
        rand_y = rand() % 35 - 2;

        // Spawn new collectible
        //std::cout << "New potion at: " << rand_x << " " << rand_y << std::endl;
        potions_.push_back(new PotionCollectibleGameObject(glm::vec3(rand_x, rand_y, 0.0f), sprite_, &sprite_shader_, tex_[17]));
        potions_.back()->SetScale(glm::vec2(0.6f, 0.6f));
    }

    // Check for expired bullets
    for (int i = 0; i < bullets_.size(); i++) {
        BulletGameObject* current_game_object = bullets_[i];
        // Update the current game object
        current_game_object->Update(delta_time);

        if (current_game_object->IsMarkedForDeletion()) {
            // Find the object in the bullets vector
            auto it = std::find(bullets_.begin(), bullets_.end(), current_game_object);

            // If found, remove it
            if (it != bullets_.end()) {
                delete* it; // Free memory if objects are dynamically allocated
                bullets_.erase(it);
            }
        }
    }

    // Check for expired bone collectibles
    for (int i = 0; i < collectibles_.size(); i++) {
        CollectibleGameObject* current_game_object = collectibles_[i];
        // Update the current game object
        current_game_object->Update(delta_time);

        if (current_game_object->IsMarkedForDeletion()) {
            // Find the object in the bullets vector
            auto it = std::find(collectibles_.begin(), collectibles_.end(), current_game_object);

            // If found, remove it
            if (it != collectibles_.end()) {
                delete* it; // Free memory if objects are dynamically allocated
                collectibles_.erase(it);
            }
        }
    }

    // Check for expired potion collectibles
    for (int i = 0; i < potions_.size(); i++) {
		PotionCollectibleGameObject* current_game_object = potions_[i];
		// Update the current game object
		current_game_object->Update(delta_time);

		if (current_game_object->IsMarkedForDeletion()) {
			// Find the object in the potions vector
			auto it = std::find(potions_.begin(), potions_.end(), current_game_object);

			// If found, remove it
			if (it != potions_.end()) {
				delete* it; // Free memory if objects are dynamically allocated
				potions_.erase(it);
			}
		}
	}

    // Check for expired disc collectibles
    for (int i = 0; i < discs_.size(); i++) {
		DiscCollectibleGameObject* current_game_object = discs_[i];
		// Update the current game object
		current_game_object->Update(delta_time);

		if (current_game_object->IsMarkedForDeletion()) {
			// Find the object in the discs vector
			auto it = std::find(discs_.begin(), discs_.end(), current_game_object);

			// If found, remove it
			if (it != discs_.end()) {
                delete discs_[i]->sparkles_; // Delete the associated particle system
				delete* it; // Free memory if objects are dynamically allocated
				discs_.erase(it);
			}
		}
	}

    // Update all game objects

    for (int i = 0; i < game_objects_.size(); i++) {
        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        // Update the current game object
        current_game_object->Update(delta_time);
        
        //Check for any finished explosions
        if (explosions_.size() > 0) {
            //std::cout << "Explosion status: "<< explosions_[0]->timer_.Finished() << std::endl;
            if (explosions_[0]->explosion_timer_.Finished()) {
				explosions_.erase(explosions_.begin());
                std::cout << "Explosion finished!" << std::endl;
				continue;
			}
		}

        // Check for collision with other game objects
        // Note the loop bounds: we avoid testing the last object since
        // it's the background covering the whole game world



        for (int j = i + 1; j < (game_objects_.size()); j++) {
            GameObject* other_game_object = game_objects_[j];

            // Compute distance between object i and object j
            glm::vec3 curr_pos = current_game_object->GetPosition();
            curr_pos.z = 0.0f;
            glm::vec3 other_pos = other_game_object->GetPosition();
            other_pos.z = 0.0f;
            float distance = glm::length(curr_pos - other_pos);

            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
            EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(other_game_object);


            if (player && enemy) {
                enemy->updatePlayerPos(player->GetPosition());
                if (distance < 2.0f) {
                    enemy->state_ = INTERCEPTING_;
                }
            }

            // If distance is below a threshold, we have a collision
            if (distance < 0.6f) {
                // Check if collision is with a collidable object
                if (!(current_game_object->IsCollidable() && other_game_object->IsCollidable())) {
                    break;
                }

                //Check for exceptions where collisions should be ignored
                EnemyGameObject* curr_enemy = dynamic_cast<EnemyGameObject*>(current_game_object);
                EnemyGameObject* other_enemy = dynamic_cast<EnemyGameObject*>(other_game_object);
                if (curr_enemy && other_enemy) {
                    //std::cout<< "Both Enemy Collision" << std::endl;
                    continue;
				}

                CollectibleGameObject* curr_collectible = dynamic_cast<CollectibleGameObject*>(current_game_object);
                if (curr_collectible && other_enemy) {
					//std::cout<< "Collectible Collision" << std::endl;
					continue;
                }

                // Make player take damage
                if (player && !player->isInvincible_) {
                    player->takeDamage();
                    std::cout << "Player took damage! " << std::endl << "Current HP: " << player->hp_ << std::endl;

                    player->isInvincible_ = true;
                    player->invincibilityTimer_.Start(1.5f);

                    if (player->hp_ <= 0) {
                        //Add explosion
                        explosions_.push_back(new ExplosionGameObject(player->GetPosition(), sprite_, &sprite_shader_, tex_[6]));
                        std::cout << "Player has died!" << std::endl;
                        glfwSetWindowShouldClose(window_, true);
                        continue;
                    }
                }

                // This is where you would perform collision response between objects
                if (other_enemy) {
                    score += 100;
                    std::cout<< "Explosion Started" << std::endl;
                    explosions_.push_back(new ExplosionGameObject(other_game_object->GetPosition(), sprite_, &sprite_shader_, tex_[6]));
                    game_objects_.erase(game_objects_.begin() + j);
				}

                if (player) {
                    std::cout<< "Player current HP: " << player->hp_ << std::endl;
                }

            }
        }

        // Check for collision with bullets
        for (int j = 0; j < (bullets_.size()); j++) {
            BulletGameObject* bullet = bullets_[j];
            glm::vec3 curr_pos = current_game_object->GetPosition();
            curr_pos.z = 0.0f;
            glm::vec3 other_pos = bullet->GetPosition();
            other_pos.z = 0.0f;

            float distance = glm::length(curr_pos - other_pos);

            EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(current_game_object);
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);

            //if its friendly bullet collididng with enemy
            if (enemy && bullet->isFriendly) {
                // If distance is below a threshold, we have a collision
                if (distance < 0.6f) {
                    // Check if collision is with a collidable object
                    if (!(enemy->IsCollidable())) {
                        break;
                    }
                    score += 100;
                    std::cout << "Explosion Started" << std::endl;
                    explosions_.push_back(new ExplosionGameObject(enemy->GetPosition(), sprite_, &sprite_shader_, tex_[6]));
                    game_objects_.erase(game_objects_.begin() + i);

                }
            //if its enemy bullet colliding with player
            }else if (player && !bullet->isFriendly && !player->isInvincible_) {
                // If distance is below a threshold, we have a collision
                if (distance < 0.6f) {
                    
                    player->takeDamage();
                    std::cout << "Player took damage! " << std::endl << "Current HP: " << player->hp_ << std::endl;

                    player->isInvincible_ = true;
                    player->invincibilityTimer_.Start(1.5f);


                    if (player->hp_ <= 0) {
                        //Add explosion
                        explosions_.push_back(new ExplosionGameObject(player->GetPosition(), sprite_, &sprite_shader_, tex_[6]));
                        std::cout << "Player has died!" << std::endl;
                        glfwSetWindowShouldClose(window_, true);
                        continue;
                    }

                }
            }

        }

        // Check for collision with bone collectibles
        for (int j = 0; j < collectibles_.size(); j++) {
            CollectibleGameObject* collectible = collectibles_[j];
            glm::vec3 curr_pos = current_game_object->GetPosition();
            curr_pos.z = 0.0f;
            glm::vec3 other_pos = collectible->GetPosition();
            other_pos.z = 0.0f;
            
			float distance = glm::length(curr_pos - other_pos);

			PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);

            if (player) {
				// If distance is below a threshold, we have a collision
                if (distance < 0.6f) {
					// Check if collision is with a collidable object
                    if (!(collectible->IsCollidable())) {
						break;
					}
                    std::cout << "Collectible collected!" << std::endl;
                    collectible->MarkForDeletion();
                    collectible->SetCollidable(false);
                    player->objectsCollected_++;
				}
			}   
        }

        // Check for collision with potion collectibles
        for (int j = 0; j < potions_.size(); j++) {
			PotionCollectibleGameObject* potion = potions_[j];
			glm::vec3 curr_pos = current_game_object->GetPosition();
			curr_pos.z = 0.0f;
			glm::vec3 other_pos = potion->GetPosition();
			other_pos.z = 0.0f;

			float distance = glm::length(curr_pos - other_pos);

			PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);

			if (player) {
				// If distance is below a threshold, we have a collision
				if (distance < 0.6f) {
					// Check if collision is with a collidable object
					if (!(potion->IsCollidable())) {
						break;
					}
					std::cout << "Potion collected!" << std::endl;
					potion->MarkForDeletion();
					potion->SetCollidable(false);

                    // Check if player is at full health
                    if (player->hp_ < 3) {
                        player->hp_ ++;
                    }
				}
			}
		}

        // Check for collision with disc collectibles
        for (int j = 0; j < discs_.size(); j++) {
            DiscCollectibleGameObject* disc = discs_[j];
            glm::vec3 curr_pos = current_game_object->GetPosition();
            curr_pos.z = 0.0f;
            glm::vec3 other_pos = disc->GetPosition();
            other_pos.z = 0.0f;

            float distance = glm::length(curr_pos - other_pos);

            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);

            if (player) {
				// If distance is below a threshold, we have a collision
				if (distance < 0.6f) {
					// Check if collision is with a collidable object
					if (!(disc->IsCollidable())) {
						break;
					}
					std::cout << "Disc collected!" << std::endl;
					disc->MarkForDeletion();
					disc->SetCollidable(false);
					player->goalObjectsCollected_++;
                    std::cout << "Goal objects collected: " << player->goalObjectsCollected_ << std::endl;
                    if(player->goalObjectsCollected_ == 3){
						std::cout << "Player has won!" << std::endl;
						glfwSetWindowShouldClose(window_, true);
					}
				}
			}
        }
       
    }
}


void Game::Render(void){

    // Clear background
    glClearColor(viewport_background_color_g.r,
                 viewport_background_color_g.g,
                 viewport_background_color_g.b, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use aspect ratio to properly scale the window
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    glm::mat4 window_scale_matrix;
    if (width > height){
        float aspect_ratio = ((float) width)/((float) height);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/aspect_ratio, 1.0f, 1.0f));
    } else {
        float aspect_ratio = ((float) height)/((float) width);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f/aspect_ratio, 1.0f));
    }


    // Set view to zoom out centered by default at 0,0
    float camera_zoom = 0.35f;
    glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));
    glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix;
    glm::mat4 hudProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

    view_matrix = glm::translate(view_matrix, glm::vec3(-1 * player_->GetPosition().x, -1 * player_->GetPosition().y, 0.0f));

    // Render the HUD
    hud_->Render(window_scale_matrix * camera_zoom_matrix, current_time_);

    // Render explosions objects
    for (int i = 0; i < explosions_.size(); i++) {
		explosions_[i]->Render(view_matrix, current_time_);
	}
    // Render bullet objects
    for (int i = 0; i < bullets_.size(); i++) {
        bullets_[i]->Render(view_matrix, current_time_);
    }
    // Render all game objects
    for (int i = 1; i < game_objects_.size(); i++) {
        game_objects_[i]->Render(view_matrix, current_time_);
    }
    // Render bone collectible objects
    for (int i = 0; i < collectibles_.size(); i++) {
		collectibles_[i]->Render(view_matrix, current_time_);
	}
    // Render potion collectible objects
    for (int i = 0; i < potions_.size(); i++) {
        potions_[i]->Render(view_matrix, current_time_);
    }
    // Render background objects
    for (int i = 0; i < background_objects_.size(); i++) {
		background_objects_[i]->Render(view_matrix, current_time_);
	}
    // Render grass particle system
	grass_particle_system_->Render(view_matrix, current_time_);

    // Render sparkle particle systems
    for (int i = 0; i < discs_.size(); i++) {
		discs_[i]->sparkles_->Render(view_matrix, current_time_);
	}
    // Render disc collectible objects
    for (int i = 0; i < discs_.size(); i++) {
        discs_[i]->Render(view_matrix, current_time_);
    }

    // Render player object
    game_objects_[0]->Render(view_matrix, current_time_);


}

void Game::SpawnBullet(glm::vec3 position, glm::vec3 direction, GLuint texture, float speed, bool isFriendlyProjectile) {

    double currentTime = glfwGetTime();
    position.z = 0.0f;

    if (currentTime - lastShotTime_ >= shotCooldown_) {
        GLuint bulletTexture = texture; 
        float bulletSpeed = speed;
        BulletGameObject * bullet = new BulletGameObject(position, sprite_, &sprite_shader_, bulletTexture, direction, bulletSpeed, isFriendlyProjectile);
        bullets_.push_back(bullet);
        if (isFriendlyProjectile) { lastShotTime_ = currentTime; }
        
    }

}

void Game::SpawnOrbitEnemy(const glm::vec3& location) {
    // Create a standard EnemyGameObject that will orbit around the OrbitEnemy
    EnemyGameObject* orbitingObject = new EnemyGameObject(location, sprite_, &sprite_shader_, tex_[3]);
    orbitingObject->SetDisabled(true); // Disable its independent behavior
    orbitingObject->SetRotation(pi_over_two);

    // Add the orbiting object to the game's collection of game objects
    game_objects_.push_back(orbitingObject);

    // Create the OrbitEnemy that will handle the orbiting
    OrbitEnemy* orbitEnemy = new OrbitEnemy(this, location, sprite_, &sprite_shader_, tex_[3], orbitingObject);

    // Add the OrbitEnemy to the game's collection of game objects
    game_objects_.push_back(orbitEnemy);
}

void Game::AddBullet(BulletGameObject* bullet) {
    bullets_.push_back(bullet);
}

glm::vec3 Game::CalculateDirectionVector(float angleRadians) {
    return glm::vec3(cos(angleRadians), sin(angleRadians), 0.0f);
}

float Game::GetRotationAngleFromDirection(glm::vec3 direction) {
    // Normalize the direction vector
    glm::vec3 normalizedDirection = glm::normalize(direction);

    // Calculate the angle in radians from the normalized direction vector
    // atan2 returns the angle between the positive x-axis and the point (y, x)
    float angleRadians = atan2(normalizedDirection.y, normalizedDirection.x);

    return angleRadians;
}
      
} // namespace game
