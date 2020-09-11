#ifndef CAMERA_H
#define CAMERA_H
// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
private:
	float const pi = std::acos(-1.0f);

public:
	// Matrices
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	// Vectors and angles
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lookPoint = glm::vec3(0.0f, 0.0f, 0.0f);;
	glm::vec2 lookAngle = glm::vec2(0.0f, 0.0f);;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	// Values
	float orbitRadius = 0.0f;
	double orthoScale = 1.0f;
	// Update behaviours
	void update();
	void updateOrbit();
	// View changes
	void orthographic(int w, int h);
	void perspective(int w, int h);

};

#endif