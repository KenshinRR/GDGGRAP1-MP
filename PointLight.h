#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"
#include "Shader.h"

class PointLight : public Light{
private:
	float brightness;

public:
	PointLight(
		glm::vec3 lightPos,
		glm::vec3 lightColor,
		float ambientStr,
		glm::vec3 ambientColor,
		float specStr,
		float specPhong,
		float brightness
	);

public:
	void attachSpecifics(Shader* shaderProg);

public:
	void setBrightness(float brightness);

};