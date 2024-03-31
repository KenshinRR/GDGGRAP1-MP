#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 WorldUp, glm::vec3 Front){

    //camera
    
    /*      INITIAL CAMERA VARIABLES        */
    this->cameraPos = pos;
    this->WorldUp = WorldUp;
    this->Front = Front;

    this->viewMatrix = glm::lookAt(pos, pos + Front, WorldUp);
}

void Camera::performFundamentals(Shader* shaderProg)
{
    shaderProg->setMat4("view", viewMatrix);
    shaderProg->setVec3("camerapos", cameraPos);
}

void Camera::setCameraPos(glm::vec3 camPos) {
    this->cameraPos = camPos;
}

void Camera::setFront(glm::vec3 Front) {
    this->Front = Front;
}

glm::vec3 Camera::getFront() {
    return this->Front;
}

glm::vec3 Camera::getWorldUp() {
    return this->WorldUp;
}
	
glm::mat4 Camera::getViewMat() {
    this->viewMatrix = glm::lookAt(this->cameraPos, this->cameraPos + this->Front, this->WorldUp); //update viewMatrix
    return this->viewMatrix;
}

glm::vec3 Camera::getCameraPos() {
    return this->cameraPos;
}