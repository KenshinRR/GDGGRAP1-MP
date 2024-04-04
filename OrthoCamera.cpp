#include "OrthoCamera.h"


OrthoCamera::OrthoCamera(glm::vec3 pos, glm::vec3 WorldUp, glm::vec3 Front,
float fov, float height, float width, float zfar, bool Dstate) : Camera(pos, WorldUp, Front, Dstate) {
	this->projection = glm::ortho(
        -1.5f, //left
        1.5f, //right
        -1.5f, //bot
        1.5f, //top
        -0.1f, //z near
        zfar);  //z far
    
}

void OrthoCamera::perfromSpecifics(Shader* shaderProg)
{
    this->performFundamentals(shaderProg);
    shaderProg->setMat4("projection", projection);
}

glm::mat4 OrthoCamera::getProjection() {
	return this->projection;
}