#include <stdexcept>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>

#include <path_config.h>

#include "sprite.h"
#include "tile.h"
#include "shader.h"
#include "player_game_object.h"
#include "collectible_game_object.h"
#include "enemy_game_object.h"
#include "game.h"
#include "timer.h"

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

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;
}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }

    delete hud_;

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{

    // Setup the game world

    // Load textures
    SetAllTextures();

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    player_ = new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[0]);
    game_objects_.push_back(player_);
    game_objects_[0]->SetRotation(pi_over_two);


    // Setup enemy objects
    game_objects_.push_back(new EnemyGameObject(glm::vec3(-5.0f, 1.0f, 0.0f), sprite_, &sprite_shader_, tex_[3]));
    game_objects_[1]->SetRotation(pi_over_two);

    // Setup collectible objects
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(2.0f, 2.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    game_objects_[2]->SetScale(glm::vec2(0.5f, 0.5f));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(-2.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    game_objects_[3]->SetScale(glm::vec2(0.5f, 0.5f));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(-2.0f, 3.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    game_objects_[4]->SetScale(glm::vec2(0.5f, 0.5f));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(4.0f, 1.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    game_objects_[5]->SetScale(glm::vec2(0.5f, 0.5f));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(-1.0f, -3.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    game_objects_[6]->SetScale(glm::vec2(0.5f, 0.5f));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(3.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]));
    game_objects_[7]->SetScale(glm::vec2(0.5f, 0.5f));

    // Setup orbiting axe object
    game_objects_.push_back(new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[8]));
    game_objects_[8]->SetOrbit(1.0f, 3.0f, glm::vec2(2.0f, 2.0f));
    game_objects_[8]->SetScale(glm::vec2(0.5f, 0.5f));
    game_objects_[8]->SetCollidable(false);

    // Setup background
    // In this specific implementation, the background is always the last object
    GameObject *background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tile_, &sprite_shader_, tex_[4]);
    background->SetScale(glm::vec2(90.0f, 90.0f));
    background_objects_.push_back(background);

    // Initialize the HUD
    hud_ = new HUD(resources_directory_g + "/textures/hud/", &sprite_shader_, glm::ortho(0.0f, static_cast<float>(window_width_g), static_cast<float>(window_height_g), 0.0f));


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
        {"/textures/player_frames/left_step.png", "/textures/player_frames/right_step.png", "/textures/player_frames/still.png", "/textures/enemy_orange.png", "/textures/grass03.png", "/textures/orb.png", "/textures/explosion0.png",
        "/textures/coin.png", "/textures/axe.png"};
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
        hud_->Update(score, player_->hp_, player_->objectsCollected_, player_->isInvincible_, player_->invincibilityTimer_.TimeLeft());


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
    float angle_increment = (glm::pi<float>() / 1800.0f)*speed;

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
}


void Game::Update(double delta_time)
{
    // Get player game object
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);


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
    if (!timer_.Running()) {
        timer_.Start(5.0f);
    }

    if (timer_.Finished()) {
        // Generate random coordinates to spawn a new enemy
        int rand_x = rand() % 15 - 2;
        int rand_y = rand() % 15 - 2;

        // Spawn new enemy
        std::cout << "New enemy at: " << rand_x << " " << rand_y << std::endl;
		game_objects_.push_back(new EnemyGameObject(glm::vec3(rand_x, rand_y, 0.0f), sprite_, &sprite_shader_, tex_[3]));
        game_objects_.back()->SetRotation(pi_over_two);

        // Generate random coordinates to spawn a new collectible
        rand_x = rand() % 15 - 2;
        rand_y = rand() % 15 - 2;

        // Spawn new collectible
        std::cout << "New collectible at: " << rand_x << " " << rand_y << std::endl;
        game_objects_.push_back(new CollectibleGameObject(glm::vec3(rand_x, rand_y, 0.0f), sprite_, &sprite_shader_, tex_[7]));
        game_objects_.back()->SetScale(glm::vec2(0.5f, 0.5f));
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
            float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());

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

                // Check if collision is with a collectible object
                CollectibleGameObject* collectible = dynamic_cast<CollectibleGameObject*>(other_game_object);
                PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
                if (collectible && player) {
					game_objects_[j]->SetGhost(true);
                    game_objects_[j]->SetCollidable(false);
					std::cout << "Collected a collectible object!" << std::endl;
                    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[i]);
                    if (player) {
						player->objectsCollected_ += 1;
                        score += 20;
						std::cout << "Player has: " << player->objectsCollected_ << " collected objects!" << std::endl;
					}
                    break;
                }


                //Check for exceptions where collisions should be ignored
                EnemyGameObject* curr_enemy = dynamic_cast<EnemyGameObject*>(current_game_object);
                EnemyGameObject* other_enemy = dynamic_cast<EnemyGameObject*>(other_game_object);
                if (curr_enemy) {
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

                std::cout<< "Player current HP: " << player->hp_ << std::endl;
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
    glm::mat4 hudProjection = glm::ortho(0.0f, static_cast<float>(window_width_g), static_cast<float>(window_height_g), 0.0f);

    view_matrix = glm::translate(view_matrix, glm::vec3(-1 * player_->GetPosition().x, -1 * player_->GetPosition().y, 0.0f));
    // Render the HUD
    hud_->Render(hudProjection, current_time_);
    // Render all game objects
    for (int i = 0; i < explosions_.size(); i++) {
		explosions_[i]->Render(view_matrix, current_time_);
	}

    for (int i = 0; i < game_objects_.size(); i++) {
        game_objects_[i]->Render(view_matrix, current_time_);
    }

    for (int i = 0; i < background_objects_.size(); i++) {
		background_objects_[i]->Render(view_matrix, current_time_);
	}


}
      
} // namespace game
