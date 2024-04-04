#ifndef HUD_H
#define HUD_H

#include "game_object.h"
#include "shader.h"
#include <vector>
#include <glm/glm.hpp>
#include "sprite.h"
#include <string>
#include "timer.h"

namespace game {

    class HUD {
    public:
        HUD(const std::string& hudTexturePath, Shader* shader, const glm::mat4& projectionMatrix);
        ~HUD();

        void Update(int score, int health, int collectibles, bool isInvincible, float invincibilityTimeLeft);
        void Render(glm::mat4 hudProjection, double currentTime);

    private:
        Shader* shader_;
        glm::mat4 projectionMatrix_;
        std::vector<GameObject*> hudElements; // Stores HUD elements
        std::vector<GameObject*> scoreDigits_;
        std::vector<GameObject*> hearts;
        std::vector<GameObject*> coins;

        Timer invincibilityTimer_; // Timer for invincibility duration
        std::vector<GameObject*> invincibilityTimerDigits_; // HUD elements for the timer digits


        int score_;
        int health_;
        int collectibles_;

        Geometry* sprite_;

        GLuint numberTextures[10]; // Textures for numbers 0-9
        GLuint scoreLabelTexture;  // Texture for the "score" label
        GLuint heartTexture;
        GLuint emptyHeartTexture;
        GLuint coinTexture;
        GLuint emptyCoinTexture;
        GLuint goldHeartTexture;;


        void LoadTexture(GLuint& texture, const char* filepath);

    };

} // namespace game

#endif // HUD_H
