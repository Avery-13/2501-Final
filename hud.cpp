#include "hud.h"
#include <SOIL/SOIL.h>
#include <iostream>


namespace game {

    HUD::HUD(const std::string& hudTexturePath, Shader* shader, const glm::mat4& projectionMatrix, Shader* heart_shader)
        : shader_(shader), projectionMatrix_(projectionMatrix), score_(0), health_(0), collectibles_(0) {

        sprite_ = new Sprite();
        sprite_->CreateGeometry(); 

        // Load number textures
        for (int i = 0; i < 10; ++i) {
            GLuint textureID;
            std::string filename = hudTexturePath + std::to_string(i) + ".png";
            LoadTexture(textureID, filename.c_str());
            numberTextures[i] = textureID; // Save the texture ID
        }
        // Load symbol textures
        LoadTexture(symbolTextures[0], (hudTexturePath + "x.png").c_str());
        LoadTexture(symbolTextures[1], (hudTexturePath + "y.png").c_str());
        LoadTexture(symbolTextures[2], (hudTexturePath + "-.png").c_str());

        // Load the score label texture
        LoadTexture(scoreLabelTexture, (hudTexturePath + "score.png").c_str());
        LoadTexture(heartTexture, (hudTexturePath + "heart.png").c_str());
        LoadTexture(emptyHeartTexture, (hudTexturePath + "emptyHeart.png").c_str());
        LoadTexture(boneTexture, (hudTexturePath + "bone.png").c_str());
        LoadTexture(emptyBoneTexture, (hudTexturePath + "bone_empty.png").c_str());


        // Position HUD elements at the top-left corner of the screen
        glm::vec3 scorePosition(-2.7f, 2.4f, 0.0f);
        GameObject* scoreLabelGO = new GameObject(scorePosition, sprite_, shader_, scoreLabelTexture);
        scoreLabelGO->SetScale(glm::vec2(1.8f, 0.6f));
        hudElements.push_back(scoreLabelGO);

        // Initialize the score digits
        glm::vec3 digitPosition(-1.5f, 2.4f, 0.0f); // Position on the HUD for the first digit
        for (int i = 0; i < 4; ++i) { // Assuming a 4 digit score
            // Initialize each digit GameObject with the texture for '0'
            GameObject* digitGO = new GameObject(digitPosition, sprite_, shader_, numberTextures[0]);
            digitGO->SetScale(glm::vec2(0.4, 0.4)); // Set an appropriate scale for your HUD
            scoreDigits_.push_back(digitGO);
            digitPosition.x += 0.3f; // Move position for the next digit
        }

        // Initialize the heart health indicators
        glm::vec3 heartPosition(-3.3f, -2.4f, 0.0f); // Position on the HUD for the first heart
        for (int i = 0; i < 3; ++i) { 
            
            GameObject* heartGO = new GameObject(heartPosition, sprite_, heart_shader, heartTexture);
            heartGO->SetScale(glm::vec2(0.8, 0.8)); // Set an appropriate scale for your HUD
            hearts.push_back(heartGO);
            heartPosition.x += 0.8f; // Move position for the next heart
        }

        // Initialize the bone collectibles
        glm::vec3 bonePosition(1.8f, -2.4f, 0.0f); // Position on the HUD for the first bone
        for (int i = 0; i < 3; ++i) { 
            
            GameObject* boneGO = new GameObject(bonePosition, sprite_, shader_, emptyBoneTexture);
            boneGO->SetScale(glm::vec2(0.6, 0.6)); // Set an appropriate scale for your HUD
            bones.push_back(boneGO);
            bonePosition.x += 0.8f; // Move position for the next bone
        }

        // Initialize the invincibility timer digits
        glm::vec3 timerDigitPosition(-3.3f, -1.8f, 0.0f); // Adjust position as needed
        for (int i = 0; i < 2; ++i) { 
            GameObject* digitGO = new GameObject(timerDigitPosition, sprite_, shader_, numberTextures[0]);
            digitGO->SetScale(glm::vec2(0.5, 0.5)); 
            invincibilityTimerDigits_.push_back(digitGO);
            timerDigitPosition.x += 0.3f; 
        }

        // Initialize the coordinate digits
        glm::vec3 coordinatesPosition(2.6f, 2.4f, 0.0f); // Adjust position as needed

        // Init x symbol
        GameObject* xGo = new GameObject(coordinatesPosition, sprite_, shader_, symbolTextures[0]);
        xGo->SetScale(glm::vec2(0.3, 0.3));
        xSymbol_ = xGo;
        coordinatesPosition.x += 0.4f;
        // Init x negative symbol
        GameObject* xNegativeGo = new GameObject(coordinatesPosition, sprite_, shader_, symbolTextures[2]);
        xNegativeGo->SetScale(glm::vec2(0.3, 0.3));
        xMinusSymbol_ = xNegativeGo;
        coordinatesPosition.x += 0.3f;
        // Init x coordinate
        for (int i = 0; i < 2; ++i) { 
			GameObject* digitGO = new GameObject(coordinatesPosition, sprite_, shader_, numberTextures[0]);
			digitGO->SetScale(glm::vec2(0.3, 0.3)); 
            coordinateDigits_.push_back(digitGO);
			coordinatesPosition.x += 0.3f; 
		}

        // Reset coordinates position
        coordinatesPosition = glm::vec3(2.6f, 2.0f, 0.0f); // Match previous x coordinate position, lower y

        // Init y symbol
        GameObject* yGo = new GameObject(coordinatesPosition, sprite_, shader_, symbolTextures[1]);
        yGo->SetScale(glm::vec2(0.3, 0.3));
        ySymbol_ = yGo;
        coordinatesPosition.x += 0.4f;
        // Init y negative symbol
        GameObject* yNegativeGo = new GameObject(coordinatesPosition, sprite_, shader_, symbolTextures[2]);
        yNegativeGo->SetScale(glm::vec2(0.3, 0.3));
        yMinusSymbol_ = yNegativeGo;
        coordinatesPosition.x += 0.3f;
        // Init y coordinate
        for (int i = 0; i < 2; ++i) {
            GameObject* digitGO = new GameObject(coordinatesPosition, sprite_, shader_, numberTextures[0]);
            digitGO->SetScale(glm::vec2(0.3, 0.3));
            coordinateDigits_.push_back(digitGO);
            coordinatesPosition.x += 0.3f;
        }

        std::cout << "Coord Digits size: " << coordinateDigits_.size() << std::endl;
    }

    HUD::~HUD() {
        for (auto element : hudElements) {
            delete element;
        }
        for (auto element : scoreDigits_) {
            delete element;
        }
        for (auto element : hearts) {
            delete element;
        }
        for (auto element : bones) {
            delete element;
        }
        for (auto element : invincibilityTimerDigits_) {
            delete element;
        }
        delete sprite_;
        
    }

    void HUD::LoadTexture(GLuint& texture, const char* filepath) {

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height;
        unsigned char* image = SOIL_load_image(filepath, &width, &height, 0, SOIL_LOAD_RGBA);
        if (image) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            SOIL_free_image_data(image);
        }
        else {
            std::cerr << "HUD texture failed to load at path: " << filepath << std::endl;
            texture = 0;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void HUD::Render(glm::mat4 viewMatrix, double currentTime) {
        shader_->Enable();

        // Since we're now rendering in screen space, the view matrix is typically an identity matrix
        glm::mat4 identityMatrix = glm::mat4(1.0f);

        // Go through each element and render it with the viewMatrix and currentTime
        for (auto element : hudElements) {
            element->Render(viewMatrix, currentTime);
        }
        for (auto element : scoreDigits_) {
            element->Render(viewMatrix, currentTime);
        }
        for (auto element : hearts) {
            element->Render(viewMatrix, currentTime);
        }
        for (auto element : bones) {
            element->Render(viewMatrix, currentTime);
        }
        for (auto element : coordinateDigits_) {
			element->Render(viewMatrix, currentTime);
		}
        xSymbol_->Render(viewMatrix, currentTime);
        ySymbol_->Render(viewMatrix, currentTime);

        // Render the negative symbols iff the player is in the negative coordinates
        if (coordinates_.x < 0) {
			xMinusSymbol_->Render(viewMatrix, currentTime);
		}
        if (coordinates_.y < 0) {
            yMinusSymbol_->Render(viewMatrix, currentTime);
        }

        // Render the invincibility timer digits if the timer is running
        if (invincibilityTimer_.Running() && invincibilityTimer_.TimeLeft() > 0.0f) {
            for (auto digit : invincibilityTimerDigits_) {
                digit->Render(viewMatrix, currentTime);
            }
        }
    }

    void HUD::Update(int score, int health, int collectibles, bool isInvincible, float invincibilityTimeLeft, glm::vec2 coordinates) {
        score_ = score;
        health_ = health;
        collectibles_ = collectibles;
        coordinates_ = coordinates;

        std::string scoreStr = std::to_string(score);
        // Ensure the score string has exactly 4 characters, padding with zeros if necessary
        scoreStr = std::string(4 - scoreStr.length(), '0') + scoreStr;

        // Convert the coordinates to positive integer strings
        // The coordinates are stored as floats, so we need to convert them to integers first
        int xCoord = static_cast<int>(coordinates.x);
        int yCoord = static_cast<int>(coordinates.y);
        std::string xCoordStr = std::to_string(abs(xCoord));
        std::string yCoordStr = std::to_string(abs(yCoord));

        // Ensure the coordinate strings have exactly 2 characters, padding with zeros if necessary
        xCoordStr = std::string(2 - xCoordStr.length(), '0') + xCoordStr;
        yCoordStr = std::string(2 - yCoordStr.length(), '0') + yCoordStr;
        
        for (int i = 0; i < 3; ++i) {
            if (i < health) {
                // Player still has this heart
                if (isInvincible) {
                    hearts[i]->SetGold(true);
                }
                else {
                    hearts[i]->SetGold(false);
                    hearts[i]->SetTexture(heartTexture);
                }

            }
            else {
                // Player has lost this heart
                hearts[i]->SetTexture(emptyHeartTexture);
            }
        }

        for (int i = 0; i < 3; ++i) {
            if (i < collectibles) {
                bones[i]->SetTexture(boneTexture);
            }
            else {
                bones[i]->SetTexture(emptyBoneTexture);
            }
        }

        // Update the textures for each score digit
        for (size_t i = 0; i < scoreDigits_.size(); ++i) {
            int digitValue = scoreStr[i] - '0'; // Convert char to int
            scoreDigits_[i]->SetTexture(numberTextures[digitValue]);
        }

        // Update the coordinates based on string size
        for (size_t i = 0; i < coordinateDigits_.size(); ++i) {
            if (i < 2) {
				int digitValue = xCoordStr[i] - '0'; // Convert char to int
				coordinateDigits_[i]->SetTexture(numberTextures[digitValue]);
			}
			else {
                int digitValue = yCoordStr[i - 2] - '0'; // Convert char to int
                coordinateDigits_[i]->SetTexture(numberTextures[digitValue]);
			}
		}

        // Manage the invincibility timer
        if (isInvincible) {
            if (!invincibilityTimer_.Running()) {
                invincibilityTimer_.Start(10.0f); // Start with a 10-second duration
            }
            // Convert the remaining time to digits and update the textures
            int secondsLeft = static_cast<int>(invincibilityTimeLeft);
            invincibilityTimerDigits_[0]->SetTexture(numberTextures[secondsLeft / 10]);
            invincibilityTimerDigits_[1]->SetTexture(numberTextures[secondsLeft % 10]);
        }
        else {
            // Stop the timer when invincibility ends
            invincibilityTimer_.Stop();
        }

    }

    // Rest of your HUD class methods...

} // namespace game
