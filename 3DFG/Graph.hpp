#ifndef GRID_H
#define GRID_H

#define NOMINMAX // For clip
// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
// STD
#include <vector>
// User
#include "exprutil.hpp"

class Graph {
private:
	GLuint vboID, iboID, vaoID;
	GLuint height1ID, height2ID;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	glm::vec2 rangeX = glm::vec2(-5.0f, 5.0f);
	glm::vec2 rangeY = glm::vec2(-5.0f, 5.0f);
	glm::vec2 rangeZ = glm::vec2(-5.0f, 5.0f);
	std::vector<GLfloat> heights;
	ExprUtil::ExprFloat expression;
	int res = 0;
public:
	bool height1Set = false;
	~Graph();
	// Create an n x n grid on the XZ plane
	void build(int n);
	// Render the object
	void render();
	// Set the expression
	void setHeights();
	// Set the expression
	bool setExpression(std::string expr);
	// Set X range
	void setRangeX(glm::vec2 range);
	// Get X range
	glm::vec2 getRangeX();
	// Set Y range
	void setRangeY(glm::vec2 range);
	// Get Y range
	glm::vec2 getRangeY();
	// Set Z range
	void setRangeZ(glm::vec2 range);
	// Get Z range
	glm::vec2 getRangeZ();
};

#endif