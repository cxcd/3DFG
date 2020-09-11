#ifndef SHADER_H
#define SHADER_H
// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// STD
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

class Shader {
private:
	// Parse a file into a string
	std::string parseFile(const std::string& filePath);

	// Compile a shader from a string of a specified type
	int compileShader(unsigned int type, const std::string& source);

	// Create the shader program from the vertex and fragement shaders
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);

public:
	// Program ID
	unsigned int id;

	// Uniform locations
	std::unordered_map<std::string, int> uniforms;

	// Construct the shader
	Shader();
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	// Use this shader
	void use();
};

#endif