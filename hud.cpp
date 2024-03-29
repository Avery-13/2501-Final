#include "hud.h"
#include <SOIL/SOIL.h>
#include <iostream>


namespace game {

    HUD::HUD(const std::string& hudTexturePath, Shader* shader, const glm::mat4& projectionMatrix)
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

        // Load the score label texture
        LoadTexture(scoreLabelTexture, (hudTexturePath + "score.png").c_str());
        LoadTexture(heartTexture, (hudTexturePath + "heart.png").c_str());
        LoadTexture(emptyHeartTexture, (hudTexturePath + "emptyHeart.png").c_str());

        // Position HUD elements at the top-left corner of the screen
        glm::vec3 scorePosition(-0.7f, 0.8f, 0.0f); 
        GameObject* scoreLabelGO = new GameObject(scorePosition, sprite_, shader_, scoreLabelTexture);
        scoreLabelGO->SetScale(glm::vec2(0.5f, 0.5f));
        hudElements.push_back(scoreLabelGO);


        glm::vec3 digitPosition(-0.3f, 0.8f, 0.0f); // Position on the HUD for the first digit
        for (int i = 0; i < 4; ++i) { // Assuming a 4 digit score
            // Initialize each digit GameObject with the texture for '0'
            GameObject* digitGO = new GameObject(digitPosition, sprite_, shader_, numberTextures[0]);
            digitGO->SetScale(glm::vec2(0.3, 0.3)); // Set an appropriate scale for your HUD
            scoreDigits_.push_back(digitGO);
            digitPosition.x += 0.2f; // Move position for the next digit
        }

        glm::vec3 heartPosition(-0.8f, -0.8f, 0.0f); // Position on the HUD for the first digit
        for (int i = 0; i < 3; ++i) { // Assuming a 4 digit score
            // Initialize each digit GameObject with the texture for '0'
            GameObject* heartGO = new GameObject(heartPosition, sprite_, shader_, heartTexture);
            heartGO->SetScale(glm::vec2(0.3, 0.3)); // Set an appropriate scale for your HUD
            hearts.push_back(heartGO);
            heartPosition.x += 0.2f; // Move position for the next digit
        }

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

    void HUD::Render(glm::mat4 hudProjection, double currentTime) {
        shader_->Enable();
        shader_->SetUniformMat4("projectionMatrix", hudProjection);

        // Since we're now rendering in screen space, the view matrix is typically an identity matrix
        glm::mat4 identityMatrix = glm::mat4(1.0f);

        // Go through each element and render it with the viewMatrix and currentTime
        for (auto element : hudElements) {
            element->Render(identityMatrix, currentTime);
        }
        for (auto element : scoreDigits_) {
            element->Render(identityMatrix, currentTime);
        }
        for (auto element : hearts) {
            element->Render(identityMatrix, currentTime);
        }
    }

    void HUD::Update(int score, int health, int collectibles) {
        score_ = score;
        health_ = health;
        collectibles_ = collectibles;

        std::string scoreStr = std::to_string(score);
        // Ensure the score string has exactly 4 characters, padding with zeros if necessary
        scoreStr = std::string(4 - scoreStr.length(), '0') + scoreStr;

        for (int i = 0; i < 3; ++i) {
            if (i < health) {
                // Player still has this heart
                hearts[i]->SetTexture(heartTexture);
            }
            else {
                // Player has lost this heart
                hearts[i]->SetTexture(emptyHeartTexture);
            }
        }

        // Update the textures for each score digit
        for (size_t i = 0; i < scoreDigits_.size(); ++i) {
            int digitValue = scoreStr[i] - '0'; // Convert char to int
            scoreDigits_[i]->SetTexture(numberTextures[digitValue]);
        }
    }

    // Rest of your HUD class methods...

} // namespace game
