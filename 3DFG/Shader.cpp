#include "Shader.hpp"

// Parse a file into a string
std::string Shader::parseFile(const std::string& filePath) {
	std::ifstream stream(filePath);
	std::stringstream strStream;
	strStream << stream.rdbuf();
	return strStream.str();
}

// Compile a shader from a string of a specified type
int Shader::compileShader(unsigned int type, const std::string& source) {
	// Get source and type, compile ID
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // Assume source is null terminated
	glCompileShader(id);
	// Check if shader compiled correctly
	int status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char infoLog[1024];
		glGetShaderInfoLog(id, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER: Compilation error of type: " << type << "\n" << infoLog << "\n";
	}
	return id;
}

// Create the shader program from the vertex and fragement shaders
unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	// Compile shaders
	unsigned int program = glCreateProgram();
	unsigned int vert = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
	// Link shader to program
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glValidateProgram(program);
	// Shaders are already linked to a program, original no longer necessary
	glDeleteShader(vert);
	glDeleteShader(frag);
	return program;
}

// Construct the shader
Shader::Shader() {
	id = 0;
}
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
	id = createShader(parseFile(vertexPath), parseFile(fragmentPath));
}

// Use this shader
void Shader::use() {
	glUseProgram(id);
}