#ifndef TEXT_H
#define TEXT_H

// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// STD
#include <iostream>
#include <map>
// User
#include "Shader.hpp"

// Character
struct Character {
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   dimensions; // Size of glyph
	glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
	unsigned int advance;    // Offset to advance to next glyph
};

// Text object
class Text {
private:
	GLuint vboID, vaoID;
	std::map<char, Character> characters;

public:
	// Destroy text
	~Text();
	// Construct text
	Text();
	// Build object
	void build(const char* filepath);
	// Render the object
	void render(Shader& shader, std::string text, float x, float y, float scale, glm::vec4 color);
};

#endif