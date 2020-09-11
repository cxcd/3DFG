#version 330 core

in vec3 vPos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vPos, 1.0);
}