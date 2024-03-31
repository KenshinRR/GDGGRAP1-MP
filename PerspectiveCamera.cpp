#include "PerspectiveCamera.h"


PerspectiveCamera::PerspectiveCamera(
    glm::vec3 pos, glm::vec3 WorldUp, glm::vec3 Front,
    float fov, float height, float width) :
    Camera(pos, WorldUp, Front) {
    //initialize the perspective camera
    this->projection = glm::perspective(
        glm::radians(fov),//FOV
        height / width, //aspect ratio
        0.1f, //znear > 0
        1000.f //zfar
    );
}

void PerspectiveCamera::perfromSpecifics(Shader* shaderProg)
{
    this->performFundamentals(shaderProg);
    shaderProg->setMat4("projection", projection);
}

glm::mat4 PerspectiveCamera::getProjection() {
    return this->projection;
}