#include "DirectionLight.h"

DirectionLight::DirectionLight(glm::vec3 lightPos, glm::vec3 lightColor, float ambientStr, glm::vec3 ambientColor, float specStr, float specPhong, glm::vec3 direction, float brightness) :
Light(lightPos, lightColor, ambientStr, specStr, specPhong)
{
    this->direction = direction;
    this->brightness = brightness;
}

//attach the specific values needed for direction light
void DirectionLight::attachSpecifics(Shader* shaderProg)
{
    this->attachFundamentals(shaderProg);
    shaderProg->setVec3("direction", direction);
    shaderProg->setFloat("dl_brightness", this->brightness);
}

void DirectionLight::setBrightness(float brightness)
{
    this->brightness = brightness;
}
