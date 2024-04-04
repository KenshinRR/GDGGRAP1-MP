#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <string>
#include <iostream>
#include <vector>

#include "Model3D.h"
#include "Shader.h"

class Player : public Model3D {
public:
	Player(glm::vec3 position,
		std::vector<GLfloat> fullVertexData,
		const char* vertexPath,
		const char* fragmentPath, float rot);

	
	void draw();
};