#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.h"

namespace game {

GameObject::GameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
{
    // Initialize all attributes
    position_ = position;
    scale_ = glm::vec2(1.0f, 1.0f);
    angle_ = 0.0;
    geometry_ = geom;
    shader_ = shader;
    texture_ = texture; 
    render_gold_ = false;
}


glm::vec3 GameObject::GetBearing(void) const {

    glm::vec3 dir(cos(angle_), sin(angle_), 0.0);
    return dir;
}


glm::vec3 GameObject::GetRight(void) const {

    float pi_over_two = glm::pi<float>() / 2.0f;
    glm::vec3 dir(cos(angle_ - pi_over_two), sin(angle_ - pi_over_two), 0.0);
    return dir;
}

void GameObject::SetOrbit(float radius, float speed, glm::vec2 centre) {
   isOrbiting_ = true;
   orbit_radius_ = radius;
   orbit_speed_ = speed;
   orbit_centre_ = centre;
}   

void GameObject::SetRotation(float angle){ 

    // Set rotation angle of the game object
    // Make sure angle is in the range [0, 2*pi]
    float two_pi = 2.0f*glm::pi<float>();
    angle = fmod(angle, two_pi);
    if (angle < 0.0){
        angle += two_pi;
    }
    angle_ = angle;
}

glm::mat4 GameObject::CalculateOrbitMatrix(double currentTime) {

	// Calculate the orbit matrix
	float angle = orbit_speed_ * currentTime;
	float x = orbit_centre_.x + orbit_radius_ * cos(angle);
	float y = orbit_centre_.y + orbit_radius_ * sin(angle);
	glm::vec3 position(x, y, 0.0);
	return glm::translate(glm::mat4(1.0f), position);
}

void GameObject::Update(double delta_time) {
    t += delta_time;
}

void GameObject::Render(glm::mat4 view_matrix, double current_time){

    // Set up the shader
    shader_->Enable();

    // Set up the view matrix
    shader_->SetUniformMat4("view_matrix", view_matrix);

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_[0], scale_[1], 1.0));

    // Setup the rotation matrix for the shader
    glm::mat4 rotation_matrix = glm::mat4(1.0f);
    if (isOrbiting_) {
        rotation_matrix = CalculateOrbitMatrix(current_time);
	}
    else {
        rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));
    }

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

    // Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    // Set the "ghost" uniform in the shader
    shader_->SetUniform1i("ghost", render_ghost_ ? 1 : 0);

    // Set the "ghost" uniform in the shader
    shader_->SetUniform1i("gold", render_gold_ ? 1 : 0);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
}

} // namespace game
