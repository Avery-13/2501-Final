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
        HUD(const std::string& hudTexturePath, Shader* shader, const glm::mat4& projectionMatrix, Shader* heart_shader);
        ~HUD();

        void Update(int score, int health, int collectibles, bool isInvincible, float invincibilityTimeLeft, glm::vec2 coordinates);
        void Render(glm::mat4 hudProjection, double currentTime);

    private:
        Shader* shader_;
        glm::mat4 projectionMatrix_;
        std::vector<GameObject*> hudElements; // Stores HUD elements
        std::vector<GameObject*> scoreDigits_; // HUD elements for the score digits
        std::vector<GameObject*> hearts; // HUD elements for the hearts
        std::vector<GameObject*> bones; // HUD elements for the bones
        std::vector<GameObject*> coordinateDigits_; // HUD elements for the coordinates
        GameObject* xSymbol_; // HUD element for the "x" symbol
        GameObject* ySymbol_; // HUD element for the "y" symbol
        GameObject* xMinusSymbol_; // HUD element for the "-" symbol for the x coordinate
        GameObject* yMinusSymbol_; // HUD element for the "-" symbol for the y coordinate

        Timer invincibilityTimer_; // Timer for invincibility duration
        std::vector<GameObject*> invincibilityTimerDigits_; // HUD elements for the timer digits


        int score_;
        int health_;
        int collectibles_;
        glm::vec2 coordinates_;

        Geometry* sprite_;

        GLuint numberTextures[10]; // Textures for numbers 0-9
        GLuint symbolTextures[3]; // Textures for the symbols "x", "y' and "-"
        GLuint scoreLabelTexture;  // Texture for the "score" label
        GLuint heartTexture;
        GLuint emptyHeartTexture;
        GLuint boneTexture;
        GLuint emptyBoneTexture;


        void LoadTexture(GLuint& texture, const char* filepath);

    };

} // namespace game

#endif // HUD_H
