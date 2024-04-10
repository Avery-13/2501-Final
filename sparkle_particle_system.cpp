#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "sparkle_particle_system.h"
#include <iostream>


namespace game {

    SparkleParticleSystem::SparkleParticleSystem(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GameObject *parent)
	: GameObject(position, geom, shader, texture){

    parent_ = parent;
}


void SparkleParticleSystem::Update(double delta_time) {

    // Call the parent's update method to move the object in standard way, if desired
    GameObject::Update(delta_time);
}


void SparkleParticleSystem::Render(glm::mat4 view_matrix, double current_time){

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up the shader
    shader_->Enable();

    // Set up the view matrix
    shader_->SetUniformMat4("view_matrix", view_matrix);

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_.x, scale_.y, 1.0));

    // Setup the rotation matrix for the shader
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix;

    try {
        // Set up the parent transformation matrix
        glm::mat4 parent_rotation_matrix = glm::rotate(glm::mat4(1.0f), parent_->GetRotation(), glm::vec3(0.0, 0.0, 1.0));\
        // Get the parent's position but modify the z value to be 0
        glm::vec3 parent_position = parent_->GetPosition();
        parent_position.z = 0.0f;
        glm::mat4 parent_translation_matrix = glm::translate(glm::mat4(1.0f), parent_position);
        glm::mat4 parent_transformation_matrix = parent_translation_matrix * parent_rotation_matrix;

        transformation_matrix = parent_transformation_matrix * translation_matrix * rotation_matrix * scaling_matrix;
    }
    catch (int i) {
        std::cout << "Error: ParticleSystem::Render: Could not get parent transformation matrix" << std::endl;
    }

    // Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    // Set the time in the shader
    shader_->SetUniform1f("time", current_time);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());

    // Bind the particle texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

    // Disable blending and depth testing after rendering particles
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

} // namespace game
