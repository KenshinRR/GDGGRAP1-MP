#include "Model3D.h"

Model3D::Model3D(glm::vec3 position)
{
	this->position = position;
	this->rotation = { 0,0,0 };
	this->scale = { 0.1f,0.1f,0.1f };
    theta_x = 0.f;
    theta_y = 0.f;
    theta_z = 0.f;

    /* Initialize the identity matrix */
    this->identity_matrix4 = glm::mat4(1.0f);
}

void Model3D::draw(GLuint* shaderProg, GLuint* VAO, std::vector<GLfloat>* fullVertexData)
{
    //start with the translation matrix
    glm::mat4 transformation_matrix = glm::translate(
        identity_matrix4,
        glm::vec3(position.x, position.y, position.z)
    );

    //multiply the resultin matrix with the scale
    transformation_matrix = glm::scale(
        transformation_matrix,
        glm::vec3(scale.x, scale.y, scale.z)
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
    unsigned int transformLoc = glGetUniformLocation(*shaderProg, "transform");
    glUniformMatrix4fv(
        transformLoc, //address of the transform variable
        1, //how many matrices to assign
        GL_FALSE, //transpose?
        glm::value_ptr(transformation_matrix) //pointer to the matrix
    );

    //tell open GL to use this shader for the VAOs below
    glUseProgram(*shaderProg);

    //bind the VAO to prep for drawing
    glBindVertexArray(*VAO);

    //drawing
    glDrawArrays(GL_TRIANGLES, 0, fullVertexData->size() / 8);
}

void Model3D::draw(Shader* shaderProg, GLuint* VAO, std::vector<GLfloat>* fullVertexData)
{
    //start with the translation matrix
    glm::mat4 transformation_matrix = glm::translate(
        identity_matrix4,
        glm::vec3(position.x, position.y, position.z)
    );

    //multiply the resultin matrix with the scale
    transformation_matrix = glm::scale(
        transformation_matrix,
        glm::vec3(scale.x, scale.y, scale.z)
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
        glm::radians(-90.f),
        glm::normalize(glm::vec3(0, 0, 1))
    );

    //setting the transformation
    shaderProg->setMat4("transform", transformation_matrix);

    //tell open GL to use this shader for the VAOs below
    shaderProg->use();

    //bind the VAO to prep for drawing
    glBindVertexArray(*VAO);

    //drawing
    glDrawArrays(GL_TRIANGLES, 0, fullVertexData->size() / 14);
}

void Model3D::rotate(char axis, float theta)
{
    switch (axis)
    {
    case 'x':
        this->theta_x = theta;
        break;
    case 'y':
        this->theta_y = theta;
        break;
    case 'z':
        this->theta_z = theta;
        break;
    default:
        break;
    }
}

