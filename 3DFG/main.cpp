// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
// STD
#include <iostream>
#include <vector>	
#include <fstream>
#include <sstream>
#include <string>
// User
#include "Graph.hpp"
#include "Cube.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Text.hpp"

// Basics

// Default dimensions and title
int windowWidth = 800, windowHeight = 800;
const char* windowTitle = "3DFG - 3D Function Grapher";
// Window
GLFWwindow* window;
// Camera
Camera cam;

// Delta time
double deltaTime = 0;
double oldTime = 0;
double newTime = 0;

// Graph
unsigned int graphRes = 80; // Default 80
Graph graph;
Shader graphShader;

// Background cube
Cube cube;
Shader cubeShader;

// Mouse Input
bool orbitCamera = true;
bool holdingMouseButton = false;
glm::vec2 oldMouse;
glm::vec2 newMouse;
glm::vec2 deltaMouse;
double mouseSensitivity = 15;

// String Input
enum class InputMode {
	Func,
	RangeX,
	RangeY,
	RangeZ
};
InputMode currInMode = InputMode::Func;
std::string inputStr;
bool inputtingStr = false;
int textIndex = 0;

// Animation
bool animating = false;
double animAcc = 0;
int changingExpr = 0;

// Text rendering
Shader textShader;
Text text;

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
T smoothstep(T edge0, T edge1, T x) {
	x = clip((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x * x * (3 - 2 * x);
}

// Callbacks

void reshape(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	cam.orthographic(w, h);
	// Send projection matrix to the text render
	textShader.use();
	glUniformMatrix4fv(textShader.uniforms["projection"], 1, GL_FALSE, glm::value_ptr(cam.projectionMatrix));
}

void errorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << "\n";
}

void cursorPosition(GLFWwindow* window, double x, double y) {
	newMouse.x = x;
	newMouse.y = y;
}

void mouseButton(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		oldMouse = newMouse;
		orbitCamera = false;
		holdingMouseButton = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		// orbitCamera = true;
		holdingMouseButton = false;

	}
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	if (inputtingStr) {
		if (textIndex == inputStr.length()) {
			inputStr += (char)codepoint;
		} else {
			inputStr.insert(textIndex, 1, (char)codepoint);
		}
		textIndex++;
	}
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Restart camera orbit
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (!inputtingStr) {
			orbitCamera = true;
		}
	}
	// Increment text insert index
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT) && inputtingStr) {
		textIndex--;
		if (textIndex < 0) {
			textIndex = 0;
		}
	}
	// Decrement text insert index
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT) && inputtingStr) {
		textIndex++;
		if (textIndex > inputStr.length()) {
			textIndex = inputStr.length();
		}
	}
	//Enter a function
	if (key == GLFW_KEY_I && action == GLFW_RELEASE && !inputtingStr) {
		currInMode = InputMode::Func;
		inputtingStr = true;

	}
	//Enter the x range
	if (key == GLFW_KEY_X && action == GLFW_RELEASE && !inputtingStr) {
		currInMode = InputMode::RangeX;
		inputtingStr = true;
	}
	//Enter the y range
	if (key == GLFW_KEY_Y && action == GLFW_RELEASE && !inputtingStr) {
		currInMode = InputMode::RangeY;
		inputtingStr = true;
	}
	//Enter the z range
	if (key == GLFW_KEY_Z && action == GLFW_RELEASE && !inputtingStr) {
		currInMode = InputMode::RangeZ;
		inputtingStr = true;
	}
	// Delete from end of input string
	if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT) && inputtingStr) {
		if (!inputStr.empty()) {
			if (textIndex >= inputStr.length()) {
				inputStr.pop_back();
			} else if (textIndex != 0) {
					inputStr.erase(textIndex - 1, 1);
			}
			textIndex--;
			if (textIndex < 0) {
				textIndex = 0;
			}
		}
	}
	// Send command
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && inputtingStr) {
		if (!inputStr.empty()) {
			if (currInMode == InputMode::Func) {
				if (graph.setExpression(inputStr)) {
					// Set the heights
					graph.setHeights();
					// Only animate if the function was properly set
					animAcc = 0;
					animating = true;
				}
			} else {
				// Set up stringstream to parse string into number pair
				std::istringstream ss(inputStr);
				glm::vec2 range;
				std::string temp;
				// Get number pair
				getline(ss, temp, ',');
				range.x = std::stof(temp);
				getline(ss, temp, '\n');
				range.y = std::stof(temp);
				// Send to the given range
				switch (currInMode) {
				case InputMode::RangeX:
					graph.setRangeX(range);
					break;
				case InputMode::RangeY:
					graph.setRangeY(range);
					break;
				case InputMode::RangeZ:
					graph.setRangeZ(range);
					break;
				}
				// Set the heights
				graph.setHeights();
				// Animate
				animAcc = 0;
				animating = true;
			}

		}
		// Clear input
		inputStr = "";
		textIndex = 0;
		inputtingStr = false;
	}
}

// Animate graph transition
void animate(double speed) {
	if (animAcc + deltaTime < (1 / speed)) {
		animAcc += deltaTime;
		if (graph.height1Set) {
			glUniform1f(graphShader.uniforms["weight"], smoothstep(0.0, 1.0, animAcc * speed));
		} else {
			glUniform1f(graphShader.uniforms["weight"], smoothstep(1.0, 0.0, animAcc * speed));
		}
	} else {
		animating = false;
	}
}

// Render the scene
void display() {
	// Delta time
	double timeSinceStart = glfwGetTime();
	deltaTime = timeSinceStart - oldTime;
	oldTime = timeSinceStart;
	// Camera
	if (orbitCamera) {
		// Rotate camera over time and update
		cam.lookAngle.x += 20 * deltaTime;
		cam.updateOrbit();
	} else if (holdingMouseButton) {
		// Rotate camera using mouse
		// Get Mouse delta
		deltaMouse = newMouse - oldMouse;
		oldMouse = newMouse;
		// Get new camera angle based on mouse delta
		double newX = cam.lookAngle.x + mouseSensitivity * deltaMouse.x * deltaTime;
		double newY = cam.lookAngle.y - mouseSensitivity * deltaMouse.y * deltaTime;
		cam.lookAngle.x += (newX - cam.lookAngle.x);
		cam.lookAngle.y += (newY - cam.lookAngle.y);
		// Clamp the angle with some tolerance
		if (cam.lookAngle.y <= 0.001) {
			cam.lookAngle.y = 0.001;
		} else if (cam.lookAngle.y >= 179.999) {
			cam.lookAngle.y = 179.999;
		}
		// Update camera
		cam.updateOrbit();
	}
	// Render objects
	glm::mat4 modelMatrix(1.0f);
	// Graph
	graphShader.use();
	// Graph animation
	if (animating) {
		animate(2.5);
	}
	glUniformMatrix4fv(graphShader.uniforms["MVP"], 1, GL_FALSE, glm::value_ptr(cam.projectionMatrix * cam.viewMatrix * modelMatrix));
	graph.render();
	// Cube
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	cubeShader.use();
	glEnable(GL_CULL_FACE);
	glUniformMatrix4fv(cubeShader.uniforms["MVP"], 1, GL_FALSE, glm::value_ptr(cam.projectionMatrix * cam.viewMatrix * modelMatrix));
	cube.render();
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Render text
	if (inputtingStr) {
		switch (currInMode) {
		case InputMode::Func:
			text.render(textShader, "Enter a function:", -0.9f, -0.8f, 0.0014f, glm::vec4(0.5f, 0.0f, 0.7f, 1.0f));
			text.render(textShader, "f(x, y) = ", -0.9f, -0.9f, 0.001f, glm::vec4(0.5f, 0.0f, 0.7f, 0.5f));
			break;
		case InputMode::RangeX:
			text.render(textShader, "Enter the x range:", -0.9f, -0.8f, 0.0014f, glm::vec4(0.5f, 0.0f, 0.7f, 1.0f));
			text.render(textShader, "range x = ", -0.9f, -0.9f, 0.001f, glm::vec4(0.5f, 0.0f, 0.7f, 0.5f));
			break;
		case InputMode::RangeY:
			text.render(textShader, "Enter the y range:", -0.9f, -0.8f, 0.0014f, glm::vec4(0.5f, 0.0f, 0.7f, 1.0f));
			text.render(textShader, "range y = ", -0.9f, -0.9f, 0.001f, glm::vec4(0.5f, 0.0f, 0.7f, 0.5f));
			break;
		case InputMode::RangeZ:
			text.render(textShader, "Enter the z range:", -0.9f, -0.8f, 0.0014f, glm::vec4(0.5f, 0.0f, 0.7f, 1.0f));
			text.render(textShader, "range z = ", -0.9f, -0.9f, 0.001f, glm::vec4(0.5f, 0.0f, 0.7f, 0.5f));
			break;
		}	
		text.render(textShader, inputStr.substr(0, textIndex) + "|" + inputStr.substr(textIndex, (inputStr.length() - textIndex)), -0.7f, -0.9f, 0.0014f, glm::vec4(0.5f, 0.0f, 0.7f, 1.0f));
	}
	// Axes labels
	glm::vec4 posO = cam.viewMatrix * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	glm::vec4 posX = cam.viewMatrix * glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
	glm::vec4 posY = cam.viewMatrix * glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
	glm::vec4 posZ = cam.viewMatrix * glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
	text.render(textShader, "o", posO.x, posO.y, 0.0014f, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
	text.render(textShader, "x", posX.x, posX.y, 0.0014f, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	text.render(textShader, "y", posY.x, posY.y, 0.0014f, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
	text.render(textShader, "z", posZ.x, posZ.y, 0.0014f, glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
}

// Setup

void initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16); // AA
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
	if (window == NULL) {
		std::cout << "ERROR::GLFW: Failed to create window" << std::endl;
		glfwTerminate();
		throw;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
}

void initGL() {
	// GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR::GLAD: Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		throw;
	}
	// Register callbacks
	glfwSetErrorCallback(errorCallback);
	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetCursorPosCallback(window, cursorPosition);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);
	// GL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE); // AA
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClearDepth(1.0f);
	glCullFace(GL_FRONT); // For the background cube
	glEnable(GL_BLEND); // For rendering text
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Get Shaders
	graphShader = Shader("graph.vert", "graph.frag");
	graphShader.uniforms["MVP"] = glGetUniformLocation(graphShader.id, "MVP");
	graphShader.uniforms["weight"] = glGetUniformLocation(graphShader.id, "weight");
	cubeShader = Shader("cube.vert", "cube.frag");
	cubeShader.uniforms["MVP"] = glGetUniformLocation(cubeShader.id, "MVP");
	textShader = Shader("text.vert", "text.frag");
	textShader.uniforms["projection"] = glGetUniformLocation(textShader.id, "projection");
	// Set camera defaults
	cam.lookPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	cam.lookAngle.y = 70;
	cam.orbitRadius = 1;
	// Set default view projection matrices
	reshape(window, windowWidth, windowHeight);
	// Make objects
	graph.build(graphRes);
	cube.build();
	text.build("cmunss.ttf");
}

void mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void cleanup() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void run() {
	initWindow();
	initGL();
	mainLoop();
	cleanup();
}

int main() {
	try {
		run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}