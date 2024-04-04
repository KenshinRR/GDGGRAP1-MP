#include "PointLight.h"

PointLight::PointLight(glm::vec3 lightPos, glm::vec3 lightColor, float ambientStr, glm::vec3 ambientColor, float specStr, float specPhong, float brightness) :
Light(lightPos,lightColor, ambientStr, specStr, specPhong)
{
	this->brightness = brightness;
}

//attach the specific values needed for point light
void PointLight::attachSpecifics(Shader* shaderProg)
{
    this->attachFundamentals(shaderProg);
    shaderProg->setFloat("brightness", brightness);
}

void PointLight::setBrightness(float brightness)
{
    this->brightness = brightness;
}
