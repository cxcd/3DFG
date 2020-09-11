#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in float height1;
layout(location = 2) in float height2;

out vec4 color;

uniform mat4 MVP;
uniform float weight;

void main() {
	// Get new positions
	float newY = mix(height1, height2, weight);
	vec3 finalPos = vec3(vPos.x, newY, vPos.z);

	gl_Position = MVP * vec4(finalPos, 1.0);

	// Color based on vertex y position
	vec3 color1 = vec3(0.5, 0.0, 0.7); // Lowest point
	vec3 color2 = vec3(0.9, 0.9, 1.0); // Highest point
	vec3 heightColor = mix(color1, color2, finalPos.y * 2); // change vPos to finalPos

	color = vec4(heightColor, 1.0);
}