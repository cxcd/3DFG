#version 330 core

in vec4 color;
out vec4 outputF;


void main() {
	vec4 mainColor;
	if (!gl_FrontFacing) {
		mainColor = color;
	} else {
		mainColor = mix(color, vec4(0.0, 0.0, 1.0, 1.0), 0.2);
	}
	outputF = mainColor;
}