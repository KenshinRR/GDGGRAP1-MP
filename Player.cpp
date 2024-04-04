#include "Player.h"

Player::Player(glm::vec3 position,
    std::vector<GLfloat> fullVertexData,
    const char* vertexPath, const char* fragmentPath, float rot) : Model3D(position, fullVertexData, vertexPath, fragmentPath,rot) {

}

void Player::draw()
{
    //start with the translation matrix
    glm::mat4 transformation_matrix = glm::translate(
        identity_matrix4,
        glm::vec3(position.x, position.y, position.z)
    );

    //multiply the resultin matrix with the scale
    transformation_matrix = glm::scale(
        transformation_matrix,
        glm::vec3(0.01, 0.01, 0.01)
    );

    transformation_matrix = glm::rotate(
        transformation_matrix,
        glm::radians(theta_x),
        glm::normalize(glm::vec3(1, 0, 0))
    );

    transformation_matrix = glm::rotate(
        transformation_matrix,
        glm::radians(theta_y),
        glm::normalize(glm::vec3(0, 1, 0))
    );

    transformation_matrix = glm::rotate(
        transformation_matrix,
        glm::radians(theta_z),
        glm::normalize(glm::vec3(0, 0, 1))
    );

    //setting the transformation
    this->shaderProg->setMat4("transform", transformation_matrix);

    //tell open GL to use this shader for the VAOs below
    this->shaderProg->use();

    //bind the VAO to prep for drawing
    glBindVertexArray(this->VAO);

    //drawing
    glDrawArrays(GL_TRIANGLES, 0, this->fullVertexData.size() / 14);
}