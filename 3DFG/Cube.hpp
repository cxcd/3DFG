#ifndef CUBE_H
#define CUBE_H

// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
// STD
#include <vector>

class Cube {
private:
	GLuint vboID, iboID, vaoID;
	std::vector<GLfloat> vertices = {
		// front
		-0.5, -0.5,  0.5,
		 0.5, -0.5,  0.5,
		 0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
		// back
		-0.5, -0.5, -0.5,
		 0.5, -0.5, -0.5,
		 0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5
	};
	std::vector<GLuint> indices = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};
public:

	~Cube();

	void build();

	void render();

};

#endif