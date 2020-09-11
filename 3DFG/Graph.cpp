#include "Graph.hpp"

// Utility
template <typename T>
T lerp(T a, T b, T f) {
	return a + f * (b - a);
}

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
	return std::max(lower, std::min(n, upper));
}

template <typename T>
T mapRange(T value, T oldMin, T oldMax, T newMin, T newMax) {
	return (value - oldMin) / (oldMax - oldMin) * (newMax - newMin) + newMin;
}

Graph::~Graph() {
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &iboID);
	glDeleteBuffers(1, &height1ID);
	glDeleteBuffers(1, &height2ID);
}

// Create a flat n * n grid on the XZ plane
void Graph::build(int n) {
	res = n;
	// Vertices
	for (int i = 0; i <= res; i++) {
		for (int j = 0; j <= res; j++) {
			// X
			vertices.push_back(((float)j / (float)res) - 0.5f); // X, offset by 0.5 so 0,0,0 is the middle
			// Y
			vertices.push_back(0.0f);
			// Z
			vertices.push_back(((float)i / (float)res) - 0.5f); // Z, offset by 0.5 offset so 0,0,0 is the middle
		}
	}
	// Indices
	for (int i = 0; i < res; i++) {
		for (int j = 0; j < res; j++) {
			int row1 = i * (res + 1);
			int row2 = (i + 1) * (res + 1);
			// triangle 1
			indices.push_back(row1 + j);
			indices.push_back(row1 + j + 1);
			indices.push_back(row2 + j + 1);
			// triangle 2
			indices.push_back(row1 + j);
			indices.push_back(row2 + j + 1);
			indices.push_back(row2 + j);
		}
	}
	// Generate IDs for VBO, IBO, VAO
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);
	glGenBuffers(1, &iboID);
	// Bind vertx array
	glBindVertexArray(vaoID);
	// Copy vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	// Copy indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	// Vertex attrib pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	// Generate buffers for the heights TEST
	glGenBuffers(1, &height1ID);
	glGenBuffers(1, &height2ID);
	heights = std::vector<GLfloat>(res * res, 0); // Reserve space
	glBindBuffer(GL_ARRAY_BUFFER, height1ID);
	glBufferData(GL_ARRAY_BUFFER, heights.size() * sizeof(GLfloat), heights.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, height2ID);
	glBufferData(GL_ARRAY_BUFFER, heights.size() * sizeof(GLfloat), heights.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	// Deselect
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Render the object
void Graph::render() {
	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

// Set the expression that the graph displays
void Graph::setHeights() {
	// Heights
	heights.clear();
	for (size_t i = 0; i <= res; i++) {
		for (size_t j = 0; j <= res; j++) {
			expression.variables["x"] = lerp(rangeX.x, rangeX.y, (float)j / (float)res);
			expression.variables["y"] = lerp(rangeZ.x, rangeZ.y, (float)i / (float)res);
			heights.push_back(clip(mapRange(expression.solve(), rangeY.x, rangeY.y, -1.0f, 1.0f), -0.4999f, 0.4999f));
		}
	}
	// Toggle height
	height1Set = !height1Set;
	// Check which height
	if (!height1Set) {
		// Send to height 1
		glBindBuffer(GL_ARRAY_BUFFER, height1ID);
		glBufferData(GL_ARRAY_BUFFER, heights.size() * sizeof(GLfloat), heights.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	} else {
		// Send to height 2
		glBindBuffer(GL_ARRAY_BUFFER, height2ID);
		glBufferData(GL_ARRAY_BUFFER, heights.size() * sizeof(GLfloat), heights.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
	// Deselect
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Set expression
bool Graph::setExpression(std::string expr) {
	// Set expression
	expression.set(expr);
	// Check if the function is valid
	expression.variables["x"] = 0;
	expression.variables["y"] = 0;
	if (!expression.isValid()) {
		return false;
	}
	return true;
}

// Set X range
void Graph::setRangeX(glm::vec2 range) {
	rangeX = range;
}

// Set Y range
void Graph::setRangeY(glm::vec2 range) {
	rangeZ = range;
}

// Set Z range
void Graph::setRangeZ(glm::vec2 range) {
	rangeY = range;
}